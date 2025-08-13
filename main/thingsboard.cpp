#include "thingsboard.hpp"
#include <ThingsBoard.h>
#include <Espressif_MQTT_Client.h>

#include <Shared_Attribute_Callback.h>
#include "esp_log.h"
#include "sd.h"
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "esp_netif.h"
#include <ArduinoJson.h>
#include "ping.h"
#include "device.hpp"
#include "main.h"
#include "lcd.h"
#include "config.h"

#define TAG "TB_TASK"
#define MAX_ATTRIBUTES 5

bool force_update = false;
Espressif_MQTT_Client mqttClient;
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

// Add this function outside the tb_task
void processSharedAttributeUpdate(const JsonObjectConst &data)
{
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        if (strcmp(it->key().c_str(), "refreshrate") == 0)
        {
            int refreshRate = it->value().as<int>();
            ESP_LOGI(TAG, "Received refreshrate: %d", refreshRate);
            // Handle the refreshrate update here
        }
    }
}

extern "C" void tb_task(void *pvParameters)
{
    esp_err_t ret;
    sd_init();

    char thingsboard_server[512] = "";
    int thingsboard_port = 0;
    char root_ca[2048] = "";
    char client_id[256] = "";
    char username[256] = "";
    char password[256] = "";

    ret = sd_read_root_ca(root_ca, sizeof(root_ca));
    if (ret == ESP_OK && strlen(root_ca) > 0)
    {
        mqttClient.set_server_certificate(root_ca);
    }
    else
    {
        ESP_LOGE("tb_task", "Failed to read root CA");
        lcd_setup_msg("Erreur SD", "Erreur Root CA");
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
        ESP_LOGE(TAG, "Failed to read server configuration, exiting");
        lcd_setup_msg("Erreur SD", "Erreur Conf");
        vTaskDelete(NULL);
        return;
    }

    ret = sd_read_devices();
    if (ret == ESP_OK)
    {
        device_list();
    }
    else
    {
        ESP_LOGE(TAG, "Failed to read devices");
        lcd_setup_msg("Erreur SD", "Erreur Devices");
    }

    const std::array<const char *, MAX_ATTRIBUTES> SUBSCRIBED_SHARED_ATTRIBUTES = {"refreshrate"};
    const Shared_Attribute_Callback<MAX_ATTRIBUTES> callback(processSharedAttributeUpdate, SUBSCRIBED_SHARED_ATTRIBUTES.cbegin(), SUBSCRIBED_SHARED_ATTRIBUTES.cend());

    // bool subscribed = false;

    mqttClient.set_keep_alive_timeout(300); // 30 minutes

    // mqttClient.set_disable_keep_alive(true);

    TickType_t last_update = xTaskGetTickCount();
    bool connecting = false;
    bool last_connected = false;
    bool first_cycle = true;

    while (1)
    {
        if (!main_network_connected())
        {
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue;
        }

        if (!tb.connected() && !connecting)
        {
            connecting = true;
            ESP_LOGI(TAG, "Connecting to %s:%d with client ID %s, username %s, password %s", thingsboard_server, thingsboard_port, client_id, username, password);
            lcd_setup_msg("Connexion", "au serveur");
            bool connected = tb.connect(thingsboard_server, username, thingsboard_port, client_id, password);
            ESP_LOGI(TAG, "Connected result: %d", connected);
            if (!connected)
            {
                ESP_LOGE(TAG, "Failed to connect to TB, retrying in 1 second");
                lcd_setup_msg("Erreur", "de connexion");
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                connecting = false;
                continue;
            }
            last_connected = true;
            connecting = false;
        }

        if (last_connected)
        {
            ESP_LOGI(TAG, "Connected to TB");
            lcd_setup_msg("Connexion", "réussie");
            vTaskDelay(pdMS_TO_TICKS(1000));
            last_connected = false;
        }

        // if (tb.connected() && !subscribed)
        // {
        //     ESP_LOGI(TAG, "Subscribing for shared attribute updates");

        //     if (tb.Shared_Attributes_Subscribe(callback))
        //     {
        //         ESP_LOGI(TAG, "Subscribed successfully");
        //         subscribed = true;
        //     }
        //     else
        //     {
        //         ESP_LOGE(TAG, "Failed to subscribe for shared attribute updates");
        //         subscribed = false;
        //     }
        // }
        bool time_to_send = (xTaskGetTickCount() - last_update > pdMS_TO_TICKS(UPDATE_THINGSBOARD_INTERVAL_S * 1000));
        if (time_to_send || first_cycle || force_update)
        {
            std::string reason;
            if (time_to_send)
            {
                reason = "Regular update interval";
            }
            else if (first_cycle)
            {
                reason = "First cycle";
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
            int device_with_telemetry_sent = 0;
            for (const auto &device : devices.get_all())
            {
                if (device->getTelemetryList().size() > 0)
                {
                    std::string json = device->computeTelemetryJson();
                    printf("%s\n", json.c_str());
                    if (!device->sendJsonTelemetry((char *)json.c_str()))
                    {
                        ESP_LOGE(TAG, "Failed to send telemetry for device %s", device->getName().c_str());
                    }
                    else
                    {
                        device_with_telemetry_sent++;
                        device->clearTelemetry();
                    }
                }
            }

            // clear flag if at least one device has sent telemetry and more than 10 seconds have passed
            if (device_with_telemetry_sent > 0 && (xTaskGetTickCount() - last_update > pdMS_TO_TICKS(10000)))
            {
                // wait for the telemetry to be sent before setting first_cycle to false
                first_cycle = false;
                last_update = xTaskGetTickCount();
            }
            force_update = false;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

bool sendTelemetryJson(const char *topic, char *json)
{
    ESP_LOGI(TAG, "Sent telemetry to %s", topic);
    printf("%s\n", json);
    if (tb.connected())
    {
        tb.Send_Json_String(topic, json);
        return true;
    }
    else
    {
        ESP_LOGW(TAG, "Failed to send telemetry, not connected to TB");
        return false;
    }
}

extern "C" void tb_init()
{
    xTaskCreate(tb_task, "tb_task", 8 * 1024, NULL, 5, NULL);
}

extern "C" void tb_force_update()
{
    force_update = true;
}
