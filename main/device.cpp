#include "device.hpp"
#include "esp_log.h"
#include <ArduinoJson.h>
#include "thingsboard.hpp"
#include "esp_mac.h"
#include <map>
#include "my_time.h"
#include "thingsboard.hpp"
#include "config.h"

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

Device::Device(char *name, char *mac)
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

bool Device::sendTelemetry(char *key, float value)
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

Device *DeviceList::find_by_mac(const char *mac)
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
        ESP_LOGW(TAG, "Telemetry timestamp is 0: %s", telemetry.name.c_str());
    }

    // round float to 2 decimal places
    // ESP_LOGI(TAG, "TelemetryReport: add %s: %f, timestamp: %lld, stored: %d", telemetry.name.c_str(), telemetry.value, telemetry.timestamp, telemetry_list.size());
    telemetry_list.emplace_back(telemetry);
}

extern "C" void device_add(char *name, char *mac)
{
    deviceList.add(new Device(name, mac));
}

extern "C" void device_receive(char *mac, telemetry_message data)
{
    // Device *device = deviceList.find_by_mac(mac);
    Device *device = deviceList.find_by_mac("00:00:00:00:00:01"); // temp fix for use the old device TODO: remove
    if (device)
    {
        ESP_LOGI(TAG, "Received data from %s: Temp: %f, Hum: %f, Batt: %d", device->getName().c_str(), data.temperature, data.humidity, data.battery);
        device->addTelemetry(TelemetryReport("t", data.temperature));
        device->addTelemetry(TelemetryReport("h", data.humidity));
        device->addTelemetry(TelemetryReport("b", data.battery));
    }
    else
    {
        ESP_LOGE(TAG, "Received data from unknown device %s", mac);
        ESP_LOGE(TAG, "Data: Temp: %f, Hum: %f, Batt: %d", data.temperature, data.humidity, data.battery);
    }
}

extern "C" void device_gateway_send_json_telemetry(char *json)
{
    Device *device = deviceList.find_by_mac(DEVICE_GATEWAY_MAC);
    if (device)
    {
        device->sendJsonTelemetry(json);
    }
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

extern "C" void device_report_telemetry(char *mac, char *key, float value)
{
    Device *device = deviceList.find_by_mac(mac);
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

TelemetryReport::TelemetryReport(std::string name, float value)
{
    this->name = name;
    this->value = value;
    static time_t last_timestamp = 0;
    time_t current_time = my_time_is_set() ? time(nullptr) : 0;

    if (last_timestamp == 0 || current_time - last_timestamp >= TIME_TO_CONSIDER_SAME_TIMESTAMP_S)
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
    std::map<std::string, float> zeroTimestampMap;

    for (const auto &telemetry : telemetry_list)
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

            // Round the value to 2 decimal places
            char roundedValue[10];
            snprintf(roundedValue, sizeof(roundedValue), "%.2f", telemetry.value);
            valuesObj[telemetry.name] = atof(roundedValue);
        }
    }

    // Add telemetry with timestamp 0 to the array
    for (const auto &pair : zeroTimestampMap)
    {
        JsonObject obj = array.createNestedObject();
        char roundedValue[10];
        snprintf(roundedValue, sizeof(roundedValue), "%.2f", pair.second);
        obj[pair.first] = atof(roundedValue);
    }

    std::string jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

void Device::clearTelemetry()
{
    telemetry_list.clear();
}
