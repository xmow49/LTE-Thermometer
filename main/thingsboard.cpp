#include "thingsboard.hpp"
#include <ThingsBoard.h>
#include <Espressif_MQTT_Client.h>
#include <Server_Side_RPC.h>

#include <Shared_Attribute_Callback.h>
#include "esp_log.h"
#include "sd.h"
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "esp_netif.h"
#include <ArduinoJson.h>
#include <string>
#include "ping.h"
#include "device.hpp"
#include "main.h"
#include "lcd.h"
#include "config.h"
#include "sensors.hpp"
#include "modem.h"
#include "espnow.h"
#include "battery.h"
#include "logs.h"

#ifdef __has_include
#if __has_include("default_conf.h")
#include "default_conf.h"
#define HAS_DEFAULT_CONF 1
#else
#define HAS_DEFAULT_CONF 0
#endif
#else
// Fallback for older compilers
#include "default_conf.h"
#define HAS_DEFAULT_CONF 1
#endif

#include "crash.h"

#include <OTA_Firmware_Update.h>
#include <Espressif_Updater.h>

#define TAG "TB_TASK"
#define MAX_ATTRIBUTES (32)

#define FW_MAX_CHUNK_RETRIES (10)
#define FW_PACKET_SIZE (16 * 1024)

#define LOGS_MAX_PACKET_SIZE (4096)

#define MAX_RPC_SUBSCRIPTIONS (16)

#define MAX_CONNECT_TRIES (3)
#define RECONNECT_DELAY_AFTER_FAILURE_S (60 * 60) // seconds

#define REQUEST_TIMEOUT_MICROSECONDS (1000ULL * 1000ULL)

bool force_update = false;
Espressif_MQTT_Client mqttClient;

OTA_Firmware_Update<> ota;
Attribute_Request<2U, MAX_ATTRIBUTES> attr_request;
Shared_Attribute_Update<1U, MAX_ATTRIBUTES> shared_update;
Server_Side_RPC<MAX_RPC_SUBSCRIPTIONS, 32> rpc;

const std::array<IAPI_Implementation *, 4U> apis = {
    &ota,
    &attr_request,
    &shared_update,
    &rpc};
//                            receive      send
ThingsBoardSized<MAX_ATTRIBUTES> tb(mqttClient, MAX_MESSAGE_SIZE, 0xFFFF, Default_Max_Stack_Size, apis);
Espressif_Updater<> updater;
std::array<const char *, MAX_ATTRIBUTES> SUBSCRIBED_SHARED_ATTRIBUTES = {};

SemaphoreHandle_t mqtt_mutex = NULL;

void update_starting_callback();
void progress_callback(const size_t &current, const size_t &total);
void finished_callback(const bool &success);

void processSharedAttribute(const JsonObjectConst &data)
{
    config_t before_config = config;
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        // Safely convert value to string for logging
        std::string valueStr;
        serializeJson(it->value(), valueStr);
        ESP_LOGI(TAG, "Received attribute: %s = %s", it->key().c_str(), valueStr.c_str());

        for (const config_entry_t *entry = config_entries; entry->name != NULL; entry++)
        {
            if (strcmp(entry->name, it->key().c_str()) == 0)
            {
                switch (entry->type)
                {
                case CONFIG_ENTRY_TYPE_UINT32:
                    *(uint32_t *)entry->value = it->value().as<uint32_t>();
                    break;
                case CONFIG_ENTRY_TYPE_FLOAT:
                    *(float *)entry->value = it->value().as<float>();
                    break;
                case CONFIG_ENTRY_TYPE_BOOL:
                    *(bool *)entry->value = it->value().as<bool>();
                    break;
                default:
                    ESP_LOGW(TAG, "Unsupported config entry type for %s", entry->name);
                    break;
                }
                break;
            }
        }
    }

    if (memcmp(&before_config, &config, sizeof(config_t)) != 0)
    {
        ESP_LOGI(TAG, "Configuration changed");
        config_save();
    }
    else
    {
        ESP_LOGI(TAG, "Configuration unchanged");
    }
}
void processSharedAttributeUpdate(const JsonObjectConst &data)
{
    ESP_LOGI(TAG, "shared attribute update");
    processSharedAttribute(data);
}
void requestTimedOut()
{
    ESP_LOGI(TAG, "Attribute request timed out did not receive a response in (%llu) microseconds", REQUEST_TIMEOUT_MICROSECONDS);
}

void processSharedAttributeRequest(const JsonObjectConst &data)
{
    ESP_LOGI(TAG, "Processing shared attribute request");
    processSharedAttribute(data);
}

void processRPCPing(const JsonVariantConst &data, JsonDocument &response)
{
    ESP_LOGI(TAG, "Received RPC method 'ping'");
    response["status"] = "pong";
}

void processRPCRestart(const JsonVariantConst &data, JsonDocument &response)
{
    ESP_LOGI(TAG, "Received RPC method 'restart', restarting device");
    response["status"] = "restarting";
    vTaskDelay(pdMS_TO_TICKS(3000));
    esp_restart();
}

void processRPCFetch(const JsonVariantConst &data, JsonDocument &response)
{
    ESP_LOGI(TAG, "Received RPC method 'fetch'");
    sensors_report_telemetry();
    modem_update_telemetry();
    battery_update_telemetry();
    main_report_telemetry();
    tb_force_update();

    response["status"] = "success";
}

void processRPCLogs(const JsonVariantConst &data, JsonDocument &response)
{
    ESP_LOGI(TAG, "Received RPC method 'logs'");
    DeviceList &devices = get_devices_list();
    Device *gateway = devices.get_gateway();
    if (!gateway)
    {
        response["status"] = "not_found";
        return;
    }

    int logs_size = logs_get_available_bytes();
    if (logs_size <= 0)
    {
        response["status"] = "no_logs";
        return;
    }

    // Save current read index to restore it if needed
    int initial_read_index = logs_get_read_index();
    bool success = true;
    int total_bytes_sent = 0;
    int packet_count = 0;

    // Calculate number of packets needed
    int remaining_bytes = logs_size;
    int total_parts = (logs_size + LOGS_MAX_PACKET_SIZE - 1) / LOGS_MAX_PACKET_SIZE;

    char *log_data = (char *)heap_caps_malloc(LOGS_MAX_PACKET_SIZE + 1, MALLOC_CAP_SPIRAM);
    if (!log_data)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for log data");
        response["status"] = "no_memory";
        return;
    }

    while (remaining_bytes > 0 && success)
    {
        // Calculate the size of the current packet
        int packet_size = (remaining_bytes > LOGS_MAX_PACKET_SIZE) ? LOGS_MAX_PACKET_SIZE : remaining_bytes;

        // Read logs into buffer
        int bytes_read = logs_read(log_data, packet_size);
        if (bytes_read <= 0)
        {
            ESP_LOGE(TAG, "Failed to read logs for packet %d", packet_count);
            success = false;
            break;
        }

        // Null-terminate for safety
        log_data[bytes_read] = '\0';

        // Create JSON document using ArduinoJson
        // Size estimation: log data + metadata (part, total_parts, bytes, status)
        DynamicJsonDocument packetJson(bytes_read + 512);

        // Structure the JSON with metadata inside the logs object
        JsonObject logsObj = packetJson.createNestedObject("logs");
        logsObj["part"] = packet_count + 1;
        logsObj["total_parts"] = total_parts;
        logsObj["bytes"] = bytes_read;
        logsObj["data"] = log_data;                        // ArduinoJson will handle proper escaping
        logsObj["end"] = (remaining_bytes <= packet_size); // true if this is the last packet

        // Serialize JSON to string
        std::string jsonString;
        serializeJson(packetJson, jsonString);

        // Send this packet
        ESP_LOGI(TAG, "Sending log packet %d/%d (%d bytes)",
                 packet_count + 1, total_parts, bytes_read);

        if (!gateway->sendJsonTelemetry((char *)jsonString.c_str()))
        {
            ESP_LOGE(TAG, "Failed to send log packet %d", packet_count + 1);
            success = false;
            break;
        }

        total_bytes_sent += bytes_read;
        packet_count++;

        // Update remaining bytes
        remaining_bytes -= bytes_read;

        // Small delay between packets to avoid overwhelming the network
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // Free the buffer
    heap_caps_free(log_data);

    if (success && packet_count > 0)
    {
        response["status"] = "success";
        response["total_bytes"] = total_bytes_sent;
        response["packets"] = packet_count;
    }
    else
    {
        response["status"] = "partial_success";
        response["bytes_sent"] = total_bytes_sent;
        response["packets_sent"] = packet_count;

        // Restore read index if we failed partway through
        if (packet_count > 0)
        {
            logs_reset_read_index();
            // Seek to the original position more efficiently
            char temp_buffer[1024];
            int bytes_to_skip = initial_read_index;

            while (bytes_to_skip > 0)
            {
                int chunk = (bytes_to_skip > sizeof(temp_buffer)) ? sizeof(temp_buffer) : bytes_to_skip;
                logs_read(temp_buffer, chunk);
                bytes_to_skip -= chunk;
            }
        }
    }
}

void processRPCGnss(const JsonVariantConst &data, JsonDocument &response)
{
    ESP_LOGI(TAG, "Received RPC method 'gnss'");

    if (!data.containsKey("timeout_s"))
    {
        response["status"] = "missing timeout_s key";
    }

    // check is a long int positive: error otherwise
    if (!data["timeout_s"].is<long>())
    {
        response["status"] = "invalid timeout_s value";
        return;
    }

    long timeout = data["timeout_s"];
    if (timeout <= 0)
    {
        response["status"] = "invalid timeout_s value";
        return;
    }

    if (!data.containsKey("interval_s"))
    {
        response["status"] = "missing interval_s key";
        return;
    }

    if (!data["interval_s"].is<long>())
    {
        response["status"] = "invalid interval_s value";
        return;
    }

    long interval = data["interval_s"];
    if (interval <= 0)
    {
        response["status"] = "invalid interval_s value";
        return;
    }

    esp_err_t ret = request_gnss_data(timeout, interval);
    if (ret == ESP_ERR_INVALID_STATE)
    {
        ESP_LOGE(TAG, "GNSS data not available");
        response["status"] = "fix_not_available";
        return;
    }

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get GNSS data: %s", esp_err_to_name(ret));
        response["status"] = "error";
        response["message"] = "Failed setup GNSS";
        return;
    }

    response["status"] = "success";
}

extern "C" void tb_task(void *pvParameters)
{
    esp_err_t ret;
    sd_init();
    const esp_app_desc_t *app_desc = esp_app_get_description();

    mqtt_mutex = xSemaphoreCreateMutex();
    if (mqtt_mutex == NULL)
    {
        ESP_LOGE(TAG, "Failed to create MQTT mutex");
        vTaskDelete(NULL);
        return;
    }

    char thingsboard_server[512] = "";
    int thingsboard_port = 0;
    char root_ca[2048] = "";
    char client_id[256] = "";
    char username[256] = "";
    char password[256] = "";

    bool fw_info_sent = false;
    bool fw_update_requested = false;
    bool shared_attributes_subscribed = false;
    bool rpc_subscribed = false;

    bool connecting = false;
    bool first_cycle = true;
    int device_with_telemetry_sent = 0;

    uint32_t connect_tries = 0;

    ret = sd_read_root_ca(root_ca, sizeof(root_ca));
    if (ret == ESP_OK && strlen(root_ca) > 0)
    {
        mqttClient.set_server_certificate(root_ca);
    }
    else
    {
        ESP_LOGE("tb_task", "Failed to read root CA");
        lcd_setup_msg("Erreur SD", "Erreur Root CA");

#if HAS_DEFAULT_CONF
        ESP_LOGI("tb_task", "Using default root CA");
        strcpy(root_ca, DEFAULT_ROOT_CA);
        mqttClient.set_server_certificate(root_ca);
#else
        ESP_LOGE("tb_task", "No default root CA available, cannot set up secure connection");
        vTaskDelete(NULL);
#endif
    }

    ret = sd_read_server_conf(thingsboard_server, (int *)&thingsboard_port, client_id, username, password);
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Successfully read server configuration:");
        ESP_LOGI(TAG, "Server: %s", thingsboard_server);
        ESP_LOGI(TAG, "Port: %d", thingsboard_port);
        ESP_LOGI(TAG, "Client ID: %s", client_id);
        ESP_LOGI(TAG, "Username: %s", username);
        ESP_LOGI(TAG, "Password: %s", password);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to read server configuration: using defaults");
        lcd_setup_msg("Erreur SD", "Erreur Conf");

#if HAS_DEFAULT_CONF
        ESP_LOGI(TAG, "Using default server configuration");
        strcpy(thingsboard_server, DEFAULT_THINGSBOARD_SERVER);
        thingsboard_port = DEFAULT_THINGSBOARD_PORT;
        strcpy(client_id, DEFAULT_DEVICE_ID);
        strcpy(username, DEFAULT_USERNAME);
        strcpy(password, DEFAULT_PASSWORD);
#else
        ESP_LOGE(TAG, "No default server configuration available");
        vTaskDelete(NULL);
#endif
    }

    ret = sd_read_devices();
    if (ret == ESP_OK)
    {
        device_list();
    }
    else
    {
        ESP_LOGE(TAG, "Failed to read devices: using defaults");
        lcd_setup_msg("Erreur SD", "Erreur Devices");

#if HAS_DEFAULT_CONF
        ESP_LOGI(TAG, "Using default device configuration");
        device_add("B", "00:00:00:00:00:00");
        device_add(DEFAULT_DEVICE2_NAME, DEFAULT_DEVICE2_MAC);
#else
        ESP_LOGE(TAG, "No default device configuration available");
        vTaskDelete(NULL);
#endif
    }

    DeviceList devices = get_devices_list();
    std::vector<Device *> &device_list = devices.get_all();

    for (int i = 0; i < device_list.size(); i++)
    {
        Device *device = device_list[i];
        if (!device || device->getMacString() == DEVICE_GATEWAY_MAC)
        {
            continue;
        }
        ret = espnow_register_device(device->getMac());
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to register device: %s", device->getMacString().c_str());
        }
    }
    uint32_t keepalive = config.keepalive_interval;
    mqttClient.set_keep_alive_timeout(keepalive);
    TickType_t last_update = xTaskGetTickCount();

    while (1)
    {
        if (!main_network_connected())
        {
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue;
        }

        if (keepalive != config.keepalive_interval)
        {
            ESP_LOGI(TAG, "Updating MQTT keepalive from %d to %d seconds", keepalive, config.keepalive_interval);
            keepalive = config.keepalive_interval;
            mqttClient.set_keep_alive_timeout(keepalive);
        }

        if (!tb.connected() && !connecting)
        {
            connecting = true;
            ESP_LOGI(TAG, "Connecting to %s:%d", thingsboard_server, thingsboard_port);
            lcd_setup_msg("Connexion", "au serveur");

            ESP_LOGI(TAG, "Free heap total: %ld, minimum free heap: %ld", esp_get_free_heap_size(), esp_get_minimum_free_heap_size());
            ESP_LOGI(TAG, "Free internal heap: %ld", esp_get_free_internal_heap_size());

            connect_tries++;
            ESP_LOGI(TAG, "Connect try %d/%d", connect_tries, MAX_CONNECT_TRIES);
            tb.connect(thingsboard_server, username, thingsboard_port, client_id, password);
            vTaskDelay(pdMS_TO_TICKS(1000));
            if (!tb.connected())
            {
                ESP_LOGE(TAG, "Failed to connect to TB, retrying in 1 second");
                lcd_setup_msg("Erreur", "de connexion");

                if (connect_tries >= MAX_CONNECT_TRIES)
                {
                    tb.disconnect();
                    mqttClient.set_disable_auto_reconnect(true);
                    ESP_LOGE(TAG, "Failed to connect to TB after %d tries, waiting %d seconds before retrying", connect_tries, RECONNECT_DELAY_AFTER_FAILURE_S);
                    vTaskDelay(pdMS_TO_TICKS(RECONNECT_DELAY_AFTER_FAILURE_S * 1000));
                    connect_tries = 0;
                    mqttClient.set_disable_auto_reconnect(false);
                }
                else
                {
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }

                connecting = false;
                continue;
            }
            else
            {
                connect_tries = 0;
                ESP_LOGI(TAG, "Connected to TB");
                lcd_setup_msg("Connexion", "réussie");

                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            connecting = false;
        }

        bool time_to_send = (xTaskGetTickCount() - last_update > pdMS_TO_TICKS(config.interval_send_to_tb * 1000));
        if (time_to_send || first_cycle || force_update)
        {
            device_with_telemetry_sent = 0;
            std::string reason;
            if (time_to_send)
            {
                reason = "Regular update interval";
            }
            else if (first_cycle)
            {
                reason = "First cycle";
                crash_check_dump();
                char *boot_msg = lcd_get_boot_msg();
                if (boot_msg && strlen(boot_msg) > 0)
                {
                    ESP_LOGI(TAG, "Sending boot message: %s", boot_msg);
                    if (!device_gateway_send_json_telemetry(boot_msg))
                    {
                        ESP_LOGE(TAG, "Failed to send boot message");
                    }
                    else
                    {
                        device_with_telemetry_sent++;
                    }
                }
            }
            else if (force_update)
            {
                reason = "Forced update";
            }
            else
            {
                reason = "Unknown reason";
            }
            ESP_LOGI(TAG, "Sending telemetry to TB: %s", reason.c_str());
            DeviceList &devices = get_devices_list();
            for (const auto &device : devices.get_all())
            {

                std::string json = device->computeTelemetryJson();
                if (json == "")
                {
                    ESP_LOGE(TAG, "Failed to compute telemetry JSON for device %s", device->getName().c_str());
                    continue;
                }
                if (!device->sendJsonTelemetry((char *)json.c_str()))
                {
                    ESP_LOGE(TAG, "Failed to send telemetry for device %s", device->getName().c_str());
                    device->moveBackTelemetry();
                }
                else
                {
                    device_with_telemetry_sent++;
                    device->clearTelemetryJson();
                }
            }

            last_update = xTaskGetTickCount();

            if (!fw_info_sent)
            {
                ESP_LOGI(TAG, "Sending firmware info to TB");

                fw_info_sent = ota.Firmware_Send_Info(app_desc->project_name, app_desc->version);
                if (fw_info_sent)
                {
                    ESP_LOGI(TAG, "Firmware info sent successfully");
                }
                else
                {
                    ESP_LOGE(TAG, "Failed to send firmware info");
                }
            }

            if (!fw_update_requested)
            {
                const OTA_Update_Callback callback(app_desc->project_name,
                                                   app_desc->version,
                                                   &updater,
                                                   &finished_callback,
                                                   &progress_callback,
                                                   &update_starting_callback,
                                                   FW_MAX_CHUNK_RETRIES,
                                                   FW_PACKET_SIZE);
                fw_update_requested = ota.Subscribe_Firmware_Update(callback);
            }

            if (!shared_attributes_subscribed)
            {
                size_t index = 0;

                for (const config_entry_t *entry = config_entries; entry->name != NULL && index < config_entries_count; entry++, index++)
                {
                    SUBSCRIBED_SHARED_ATTRIBUTES[index] = entry->name;
                }

                const Shared_Attribute_Callback<MAX_ATTRIBUTES> callback(processSharedAttributeUpdate, SUBSCRIBED_SHARED_ATTRIBUTES);
                ESP_LOGI(TAG, "Subscribing for shared attribute updates");
                if (shared_update.Shared_Attributes_Subscribe(callback))
                {
                    ESP_LOGI(TAG, "Subscribed successfully");
                    shared_attributes_subscribed = true;
                }
                else
                {
                    ESP_LOGE(TAG, "Failed to subscribe for shared attribute updates");
                    shared_attributes_subscribed = false;
                }

                const Attribute_Request_Callback<MAX_ATTRIBUTES> sharedCallback(&processSharedAttributeRequest, REQUEST_TIMEOUT_MICROSECONDS, &requestTimedOut, SUBSCRIBED_SHARED_ATTRIBUTES);

                if (attr_request.Shared_Attributes_Request(sharedCallback))
                {
                    ESP_LOGI(TAG, "Shared attribute request sent successfully");
                }
                else
                {
                    ESP_LOGE(TAG, "Failed to send shared attribute request");
                }
            }

            if (!rpc_subscribed)
            {
                const RPC_Callback callbacks[MAX_RPC_SUBSCRIPTIONS] = {
                    // Requires additional memory in the JsonDocument for the JsonDocument that will be copied into the response
                    {"ping", processRPCPing},
                    {"restart", processRPCRestart},
                    {"fetch", processRPCFetch},
                    {"logs", processRPCLogs},
                    {"gnss", processRPCGnss},
                };

                if (rpc.RPC_Subscribe(callbacks + 0U, callbacks + MAX_RPC_SUBSCRIPTIONS))
                {
                    ESP_LOGI(TAG, "RPC subscribed successfully");
                    rpc_subscribed = true;
                }
                else
                {
                    ESP_LOGE(TAG, "Failed to subscribe for RPC");
                    rpc_subscribed = false;
                }
            }

            if (first_cycle)
            {
                if (device_with_telemetry_sent > 0)
                {
                    ESP_LOGI(TAG, "First cycle completed with telemetry");
                    first_cycle = false;
                }
                else
                {
                    ESP_LOGW(TAG, "First cycle telemetry not sent");
                    vTaskDelay(pdMS_TO_TICKS(10000));
                }
            }

            force_update = false;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

bool sendTelemetryJson(const char *topic, char *json)
{

    if (xSemaphoreTake(mqtt_mutex, pdMS_TO_TICKS(3000)) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to take MQTT mutex");
        return false;
    }

    ESP_LOGI(TAG, "Sent telemetry to %s", topic);
    printf("%s\n", json);
    if (tb.connected())
    {
        tb.Send_Json_String(topic, json);
        xSemaphoreGive(mqtt_mutex);
        return true;
    }
    else
    {
        ESP_LOGW(TAG, "Failed to send telemetry, not connected to TB");
        xSemaphoreGive(mqtt_mutex);
        return false;
    }
}

extern "C" void tb_init()
{
    xTaskCreate(tb_task, "tb_task", 16 * 1024, NULL, 5, NULL);
}

extern "C" void tb_force_update()
{
    force_update = true;
}

void update_starting_callback()
{
    ESP_LOGI(TAG, "Firmware update starting");
}

void progress_callback(const size_t &current, const size_t &total)
{
    ESP_LOGI(TAG, "Downloading firmware progress %.2f%%", static_cast<float>(current * 100U) / total);
}

void finished_callback(const bool &success)
{
    if (success)
    {
        ESP_LOGI("MAIN", "OTA update finished successfully, restarting in 5s");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        esp_restart();
        return;
    }
    ESP_LOGI(TAG, "Downloading firmware failed");
}
