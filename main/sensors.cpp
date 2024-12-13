#include "sensors.hpp"
#include "dht.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "device.hpp"
#include "ping.h"
#include "pins.h"
#include "lcd.h"
#include "config.h"

#define TAG "SENSORS"

esp_err_t sensors_read_temp(dht_data_t *data)
{
    dht_data_t tmp;
    esp_err_t res = dht_read_float_data(DHT_TYPE_AM2301, PIN_DHT, &tmp.humidity, &tmp.temperature);
    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read temperature");
        return res;
    }
    lcd_set_temperature(tmp.temperature, tmp.humidity);
    if (data != NULL)
    {
        *data = tmp;
        return res;
    }
    else
    {
        return ESP_ERR_INVALID_ARG;
    }
}

Device *device = nullptr;
DeviceList &devices = get_devices_list();

bool sensors_report_telemetry(dht_data_t *data)
{
    if (device == nullptr)
    {
        device = devices.find_by_mac(DEVICE_GATEWAY_MAC);
        if (device == nullptr)
        {
            ESP_LOGW(TAG, "Device with gateway (me) not found.");
            return false;
        }
    }

    device->addTelemetry(TelemetryReport("t", data->temperature));
    device->addTelemetry(TelemetryReport("h", data->humidity));
    return true;
}

void sensors_task(void *pvParameters)
{
    uint32_t refresh_rate_ms = UPDATE_SENSORS_INTERVAL_S * 1000;

    while (1)
    {
        dht_data_t data;
        esp_err_t res = sensors_read_temp(&data);
        if (res != ESP_OK)
        {
            vTaskDelay(pdMS_TO_TICKS(refresh_rate_ms));
            continue;
        }
        ESP_LOGI(TAG, "Temperature: %.2f, Humidity: %.2f", data.temperature, data.humidity);

        if (!sensors_report_telemetry(&data))
        {
            ESP_LOGW(TAG, "Failed to report telemetry, retrying next cycle");
        }

        vTaskDelay(pdMS_TO_TICKS(refresh_rate_ms));
    }
}
