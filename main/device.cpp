#include "device.hpp"
#include "esp_log.h"
#include <ArduinoJson.h>
#include "thingsboard.hpp"
#include "esp_mac.h"
#include <map>
#include "my_time.h"
#include "thingsboard.hpp"
#include "config.h"
#include "sd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cmath>

#define TAG "DEVICE"
DeviceList deviceList;

static const uint8_t gateway_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 0 = gateway (this device)

void mac_from_string(const char *mac_str, uint8_t *mac)
{
    sscanf(mac_str, "%hhx%*1[:.]%hhx%*1[:.]%hhx%*1[:.]%hhx%*1[:.]%hhx%*1[:.]%hhx",
           &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
}

void mac_to_string(const uint8_t *mac, char *mac_str)
{
    snprintf(mac_str, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

Device::Device(const char *name, const char *mac)
{
    this->name = name;
    mac_from_string(mac, this->mac);
    this->isGateway = memcmp(this->mac, gateway_mac, 6) == 0;

    telemetry_list.reserve(50);
    ESP_LOGI(TAG, "Device %s: %s, isGateway: %d", name, getMacString().c_str(), isGateway);
}

bool Device::sendJsonTelemetry(char *json)
{
    DynamicJsonDocument doc(MAX_MESSAGE_SIZE);

    if (isGateway)
    {
        // Pour le gateway, on envoie le JSON tel quel
        return sendTelemetryJson("v1/devices/me/telemetry", json);
    }
    else
    {
        // Pour les autres appareils, on englobe le JSON avec le nom de l'appareil
        JsonObject root = doc.to<JsonObject>();
        JsonArray deviceArray = root.createNestedArray(this->name);

        // Désérialiser le JSON d'origine
        DynamicJsonDocument jsonDoc(MAX_MESSAGE_SIZE);
        DeserializationError error = deserializeJson(jsonDoc, json);

        if (error)
        {
            ESP_LOGE(TAG, "deserializeJson() failed: %s", error.c_str());
            return false;
        }

        // Vérifier si le JSON désérialisé est un objet ou un tableau
        if (jsonDoc.is<JsonObject>())
        {
            deviceArray.add(jsonDoc.as<JsonObject>());
        }
        else if (jsonDoc.is<JsonArray>())
        {
            JsonArray inputArray = jsonDoc.as<JsonArray>();
            for (JsonVariant v : inputArray)
            {
                deviceArray.add(v);
            }
        }
        else
        {
            ESP_LOGE(TAG, "Unexpected JSON format");
            return false;
        }

        // Sérialiser le nouveau document JSON
        char buffer[MAX_MESSAGE_SIZE];
        serializeJson(doc, buffer);

        // Envoyer le nouveau JSON
        return sendTelemetryJson("v1/gateway/telemetry", buffer);
    }
}

bool Device::sendTelemetry(char *key, double value)
{
    DynamicJsonDocument doc(MAX_MESSAGE_SIZE);
    JsonObject root = doc.to<JsonObject>();
    root[key] = value;
    char buffer[MAX_MESSAGE_SIZE];
    serializeJson(doc, buffer);
    return sendJsonTelemetry(buffer);
}

void DeviceList::add(Device *device)
{
    devices.emplace_back(device);
}

Device *DeviceList::find_by_str_mac(const char *mac)
{
    // Convert MAC to uint8_t array
    uint8_t mac_array[6];
    mac_from_string(mac, mac_array);

    for (const auto &device : devices)
    {
        if (memcmp(device->getMac(), mac_array, 6) == 0)
        {
            return device;
        }
    }
    return nullptr;
}

Device *DeviceList::find_by_mac(const uint8_t *mac)
{
    // Convert MAC to uint8_t array
    uint8_t mac_array[6];
    memcpy(mac_array, mac, 6);

    for (const auto &device : devices)
    {
        if (memcmp(device->getMac(), mac_array, 6) == 0)
        {
            return device;
        }
    }
    return nullptr;
}

std::vector<Device *> &DeviceList::get_all()
{
    return devices;
}

std::string Device::getMacString() const
{
    char mac_str[18];
    mac_to_string(mac, mac_str);
    return std::string(mac_str);
}

void Device::addTelemetry(TelemetryReport telemetry)
{
    if (telemetry.timestamp == 0)
    {
        ESP_LOGW(TAG, "Telemetry timestamp is 0: %s", telemetry.name);
    }

    if (xSemaphoreTake(telemetry_mutex, pdMS_TO_TICKS(3000)) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to take telemetry mutex");
        return;
    }

    telemetry_list.emplace_back(telemetry);

    xSemaphoreGive(telemetry_mutex);
}

extern "C" void device_add(const char *name, const char *mac)
{
    deviceList.add(new Device(name, mac));
}

extern "C" void device_receive(char *mac, telemetry_message data)
{
    ESP_LOGI(TAG, "Received data from: %s", mac);
    Device *device = deviceList.find_by_str_mac(mac);
    if (device)
    {
        ESP_LOGI(TAG, "Received data from %s: Temp: %f, Hum: %f, Batt: %d%% %d", device->getName().c_str(), data.temperature, data.humidity, data.battery_percentage, data.battery_voltage_mv);

        if (!std::isnan(data.temperature))
        {
            device->addTelemetry(TelemetryReport("t", data.temperature));
        }

        if (!std::isnan(data.humidity))
        {
            device->addTelemetry(TelemetryReport("h", data.humidity));
        }

        device->addTelemetry(TelemetryReport("b", data.battery_percentage));

        device->addTelemetry(TelemetryReport("b_v", data.battery_voltage_mv / 1000.0)); // Convert mV to V
        device->addTelemetry(TelemetryReport("b_p", data.battery_percentage));
    }
    else
    {
        ESP_LOGE(TAG, "Received data from unknown device %s", mac);
        ESP_LOGE(TAG, "Data: Temp: %f, Hum: %f, Batt: %d%% %d", data.temperature, data.humidity, data.battery_percentage, data.battery_voltage_mv);
    }
}

extern "C" bool device_gateway_send_json_telemetry(char *json)
{
    Device *device = deviceList.find_by_str_mac(DEVICE_GATEWAY_MAC);
    if (device)
    {
        return device->sendJsonTelemetry(json);
    }
    return false;
}

void device_list()
{
    for (const auto &device : deviceList.get_all())
    {
        ESP_LOGI(TAG, "Device %s: %s", device->getName().c_str(), device->getMacString().c_str());
    }
}

DeviceList &get_devices_list()
{
    return deviceList;
}

Device *DeviceList::get_gateway()
{
    return find_by_str_mac(DEVICE_GATEWAY_MAC);
}

extern "C" void device_report_telemetry(char *mac, char *key, double value)
{
    Device *device = deviceList.find_by_str_mac(mac);
    if (device)
    {
        // device->sendTelemetry(key, value);
        device->addTelemetry(TelemetryReport(key, value));
    }
    else
    {
        ESP_LOGE(TAG, "device_report_telemetry: Device %s not found", mac);
    }
}

TelemetryReport::TelemetryReport(const char *name, double value)
{
    strncpy(this->name, name, sizeof(this->name) - 1);
    this->name[sizeof(this->name) - 1] = '\0';
    this->value = value;
    static time_t last_timestamp = 0;
    time_t current_time = my_time_is_set() ? time(nullptr) : 0;

    if (last_timestamp == 0 || current_time - last_timestamp >= config.time_same_timestamp)
    {
        this->timestamp = current_time;
        last_timestamp = current_time;
    }
    else
    {
        this->timestamp = last_timestamp;
    }
}

TelemetryReport::~TelemetryReport()
{
}

std::string Device::computeTelemetryJson()
{
    DynamicJsonDocument doc(MAX_MESSAGE_SIZE);
    JsonArray array = doc.to<JsonArray>();

    // Map for grouping telemetry by timestamp
    std::map<time_t, JsonObject> timestampMap;

    // Map for storing the latest values of telemetry with timestamp 0
    std::map<std::string, double> zeroTimestampMap;

    if (xSemaphoreTake(telemetry_mutex, pdMS_TO_TICKS(3000)) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to take telemetry mutex");
        return "";
    }

    telemetry_list_json.clear();
    telemetry_list_json = std::move(telemetry_list);

    // Lire les données sauvegardées sur SD et les ajouter
    auto sd_telemetry = sd_read_device_telemetry(this->getName().c_str(), true);
    telemetry_list_json.insert(telemetry_list_json.end(), sd_telemetry.begin(), sd_telemetry.end());

    xSemaphoreGive(telemetry_mutex);

    if (telemetry_list_json.empty())
    {
        ESP_LOGI(TAG, "No telemetry to send for device %s", getName().c_str());
        return "";
    }

    for (const auto &telemetry : telemetry_list_json)
    {
        if (telemetry.timestamp == 0)
        {
            // For timestamp 0, store only the latest value for each key
            zeroTimestampMap[telemetry.name] = telemetry.value;
        }
        else
        {
            if (timestampMap.find(telemetry.timestamp) == timestampMap.end())
            {
                JsonObject reportObj = array.createNestedObject();
                reportObj["ts"] = telemetry.timestamp * 1000; // Convert to milliseconds
                JsonObject valuesObj = reportObj.createNestedObject("values");
                timestampMap[telemetry.timestamp] = valuesObj;
            }

            JsonObject valuesObj = timestampMap[telemetry.timestamp];

            if (strncmp(telemetry.name, "lat", 3) == 0 || strncmp(telemetry.name, "lon", 3) == 0)
            {
                // Round latitude and longitude to 7 decimal places
                char roundedValue[32];
                snprintf(roundedValue, sizeof(roundedValue), "%.7f", telemetry.value);
                valuesObj[telemetry.name] = atof(roundedValue);
            }
            else
            {
                // Round the value to 2 decimal places
                char roundedValue[32];
                snprintf(roundedValue, sizeof(roundedValue), "%.2f", telemetry.value);
                valuesObj[telemetry.name] = atof(roundedValue);
            }
        }
    }

    // Add telemetry with timestamp 0 to the array
    for (const auto &pair : zeroTimestampMap)
    {
        JsonObject obj = array.createNestedObject();
        char roundedValue[32];

        if (strncmp(pair.first.c_str(), "lat", 3) == 0 || strncmp(pair.first.c_str(), "lon", 3) == 0)
        {
            // Round latitude and longitude to 7 decimal places
            snprintf(roundedValue, sizeof(roundedValue), "%.7f", pair.second);
        }
        else
        {
            // Round the value to 2 decimal places
            snprintf(roundedValue, sizeof(roundedValue), "%.2f", pair.second);
        }

        obj[pair.first] = atof(roundedValue);
    }

    std::string jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

void Device::clearTelemetryJson()
{
    telemetry_list_json.clear();
}

void Device::moveBackTelemetry()
{
    if (xSemaphoreTake(telemetry_mutex, pdMS_TO_TICKS(3000)) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to take telemetry mutex");
        return;
    }
    telemetry_list = std::move(telemetry_list_json);
    telemetry_list_json.clear();

    if (sd_save_telemetrys(this, telemetry_list) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to save telemetry to SD");
    }
    else
    {
        telemetry_list.clear(); // Clear the telemetry list after saving
    }

    xSemaphoreGive(telemetry_mutex);
}

bool Device::saveTelemetryToSD()
{
    if (xSemaphoreTake(telemetry_mutex, pdMS_TO_TICKS(3000)) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to take telemetry mutex for device %s", getName().c_str());
        return false;
    }

    bool result = false;
    if (!telemetry_list.empty())
    {
        ESP_LOGI(TAG, "Saving %d telemetry entries for device %s",
                 telemetry_list.size(), getName().c_str());

        if (sd_save_telemetrys(this, telemetry_list) == ESP_OK)
        {
            telemetry_list.clear();
            last_telemetry_saved = xTaskGetTickCount();
            ESP_LOGI(TAG, "Telemetry saved successfully for device %s", getName().c_str());
            result = true;
        }
        else
        {
            ESP_LOGE(TAG, "Failed to save telemetry for device %s", getName().c_str());
        }
    }

    xSemaphoreGive(telemetry_mutex);
    return result;
}

static void telemetry_save_task(void *pvParameters)
{
    TickType_t last_wake_time = xTaskGetTickCount();

    while (1)
    {
        // Wait for the configured interval
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(config.time_save_telemetry * 1000));

        ESP_LOGI(TAG, "Telemetry save task running...");

        // Iterate through all devices and save their telemetry
        for (Device *device : deviceList.get_all())
        {
            if (device == nullptr)
                continue;

            // Use the public method to save telemetry
            device->saveTelemetryToSD();
        }

        // List telemetry files after saving
        sd_list_telemetry_files();
    }
}

extern "C" void device_start_telemetry_save_task(void)
{
    xTaskCreate(telemetry_save_task, "telemetry_save", 4 * 1024, NULL, 3, NULL);
    ESP_LOGI(TAG, "Telemetry save task started");
}
