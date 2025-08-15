#include "sensors.hpp"
#include "dht.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "device.hpp"
#include "ping.h"
#include "pins.h"
#include "lcd.h"
#include "config.h"
#include "math.h"

#define TAG "SENSORS"

// Event group bits
#define READING_REQUESTED_BIT BIT0
#define READING_COMPLETED_BIT BIT1

bool force_report = false;
static EventGroupHandle_t sensors_event_group = NULL;

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

DeviceList &devices = get_devices_list();
Device *device = devices.find_by_mac(DEVICE_GATEWAY_MAC);

void sensors_task(void *pvParameters)
{
    // Créer l'event group
    sensors_event_group = xEventGroupCreate();
    if (sensors_event_group == NULL)
    {
        ESP_LOGE(TAG, "Failed to create event group");
        vTaskDelete(NULL);
        return;
    }

    dht_data_t last_data = {0.0f, 0.0f};
    uint32_t last_update = 0;
    while (1)
    {
        // Vérifier si une lecture a été demandée
        EventBits_t bits = xEventGroupGetBits(sensors_event_group);
        bool reading_requested = (bits & READING_REQUESTED_BIT) != 0;

        dht_data_t data;
        esp_err_t res = sensors_read_temp(&data);
        if (res != ESP_OK)
        {
            // Si une lecture était demandée, signaler l'échec
            if (reading_requested)
            {
                xEventGroupClearBits(sensors_event_group, READING_REQUESTED_BIT);
                xEventGroupSetBits(sensors_event_group, READING_COMPLETED_BIT);
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        // ESP_LOGI(TAG, "Temperature: %.2f, Humidity: %.2f", data.temperature, data.humidity);

        if (device == nullptr)
        {
            device = devices.find_by_mac(DEVICE_GATEWAY_MAC);
            if (device == nullptr)
            {
                ESP_LOGW(TAG, "Device with gateway (me) not found.");
                vTaskDelay(pdMS_TO_TICKS(1000));
                continue;
            }
        }

        if (xTaskGetTickCount() - last_update >= pdMS_TO_TICKS(config.sensor_interval_force_update * 1000))
        {
            ESP_LOGI(TAG, "Forcing sensor report due to interval: %d", xTaskGetTickCount() - last_update);
            force_report = true;
        }

        if (force_report || (fabs(data.temperature - last_data.temperature) >= config.sensor_temperature_change_threshold))
        {
            ESP_LOGI(TAG, "Temperature %schange detected: %.2f -> %.2f", (force_report ? "forced " : ""), last_data.temperature, data.temperature);
            device->addTelemetry(TelemetryReport("t", data.temperature));
            last_update = xTaskGetTickCount();
            last_data.temperature = data.temperature;
        }

        if (force_report || (fabs(data.humidity - last_data.humidity) >= config.sensor_humidity_change_threshold))
        {
            ESP_LOGI(TAG, "Humidity %schange detected: %.2f -> %.2f", (force_report ? "forced " : ""), last_data.humidity, data.humidity);
            device->addTelemetry(TelemetryReport("h", data.humidity));
            last_update = xTaskGetTickCount();
            last_data.humidity = data.humidity;
        }

        force_report = false;

        // Si une lecture était demandée, signaler qu'elle est terminée
        if (reading_requested)
        {
            xEventGroupClearBits(sensors_event_group, READING_REQUESTED_BIT);
            xEventGroupSetBits(sensors_event_group, READING_COMPLETED_BIT);
        }

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

bool sensors_report_telemetry()
{
    if (sensors_event_group == NULL)
    {
        ESP_LOGE(TAG, "Event group not initialized");
        return false;
    }

    // Demander une lecture forcée
    force_report = true;

    // Signaler qu'une lecture est demandée
    xEventGroupSetBits(sensors_event_group, READING_REQUESTED_BIT);

    // Attendre que la lecture soit terminée (timeout de 30 secondes)
    EventBits_t bits = xEventGroupWaitBits(
        sensors_event_group,
        READING_COMPLETED_BIT,
        pdTRUE,              // Effacer le bit après lecture
        pdFALSE,             // Attendre un seul bit
        pdMS_TO_TICKS(30000) // Timeout de 30 secondes
    );

    if (bits & READING_COMPLETED_BIT)
    {
        ESP_LOGI(TAG, "Telemetry reading completed successfully");
        return true;
    }
    else
    {
        ESP_LOGW(TAG, "Timeout waiting for telemetry reading");
        return false;
    }
}