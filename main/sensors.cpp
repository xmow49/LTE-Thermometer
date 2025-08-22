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
#include "driver/i2c_master.h"

#define TAG "SENSORS"

// Event group bits
#define READING_REQUESTED_BIT BIT0
#define READING_COMPLETED_BIT BIT1
#define READING_FAILED_BIT BIT2

bool force_report = false;
static EventGroupHandle_t sensors_event_group = NULL;
extern i2c_master_bus_handle_t i2c_bus;
static i2c_master_dev_handle_t sht21_dev_handle = NULL;
#define SHT21_SENSOR_ADDR 0x40

esp_err_t sht21_i2c_init(void)
{
    esp_err_t ret;

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = SHT21_SENSOR_ADDR,
        .scl_speed_hz = 100000,
        .scl_wait_us = 1000,
        .flags = {
            .disable_ack_check = false}};

    ret = i2c_master_bus_add_device(i2c_bus, &dev_config, &sht21_dev_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add SHT21 device: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "SHT21 I2C initialized successfully");
    return ESP_OK;
}

esp_err_t sensors_read_sht21(dht_data_t *data)
{
    if (sht21_dev_handle == NULL)
    {
        ESP_LOGE(TAG, "SHT21 device not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t raw_data[3];
    uint8_t cmd = 0xF5;

    esp_err_t ret = i2c_master_transmit(sht21_dev_handle, &cmd, 1, pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send humidity command: %s", esp_err_to_name(ret));
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(50));

    ret = i2c_master_receive(sht21_dev_handle, raw_data, 3, pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read humidity data: %s", esp_err_to_name(ret));
        return ret;
    }

    uint16_t raw_humidity = (raw_data[0] << 8) | (raw_data[1] & 0xFC);
    float humidity = -6.0 + 125.0 * (raw_humidity / 65536.0);

    cmd = 0xF3;
    ret = i2c_master_transmit(sht21_dev_handle, &cmd, 1, pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send temperature command: %s", esp_err_to_name(ret));
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(50));

    ret = i2c_master_receive(sht21_dev_handle, raw_data, 3, pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read temperature data: %s", esp_err_to_name(ret));
        return ret;
    }

    uint16_t raw_temperature = (raw_data[0] << 8) | (raw_data[1] & 0xFC);
    float temperature = -46.85 + 175.72 * (raw_temperature / 65536.0);

    data->temperature = temperature;
    data->humidity = humidity;

    lcd_set_temperature(temperature, humidity);

    return ESP_OK;
}
esp_err_t sensors_read_dht22(dht_data_t *data)
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
Device *device = devices.find_by_str_mac(DEVICE_GATEWAY_MAC);

void sensors_task(void *pvParameters)
{
    esp_err_t ret = sht21_i2c_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize SHT21 sensor: %s", esp_err_to_name(ret));
    }

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
        dht_data_t data;
        esp_err_t res = sensors_read_sht21(&data);

        if (res != ESP_OK)
        {
            EventBits_t bits = xEventGroupGetBits(sensors_event_group);
            if (bits & READING_REQUESTED_BIT)
            {
                xEventGroupClearBits(sensors_event_group, READING_REQUESTED_BIT);
                xEventGroupSetBits(sensors_event_group, READING_FAILED_BIT);
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (device == nullptr)
        {
            device = devices.find_by_str_mac(DEVICE_GATEWAY_MAC);
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

        EventBits_t bits = xEventGroupGetBits(sensors_event_group);
        if (bits & READING_REQUESTED_BIT)
        {
            xEventGroupClearBits(sensors_event_group, READING_REQUESTED_BIT);
            xEventGroupSetBits(sensors_event_group, READING_COMPLETED_BIT);
        }

        EventBits_t result = xEventGroupWaitBits(
            sensors_event_group,
            READING_REQUESTED_BIT,
            pdFALSE,
            pdFALSE,
            pdMS_TO_TICKS(10000));

        if (result & READING_REQUESTED_BIT)
        {
            ESP_LOGI(TAG, "Reading requested, processing immediately...");
        }
    }
}

bool sensors_report_telemetry()
{
    if (sensors_event_group == NULL)
    {
        ESP_LOGE(TAG, "Event group not initialized");
        return false;
    }

    xEventGroupClearBits(sensors_event_group, READING_COMPLETED_BIT | READING_FAILED_BIT);
    force_report = true;
    xEventGroupSetBits(sensors_event_group, READING_REQUESTED_BIT);

    EventBits_t bits = xEventGroupWaitBits(
        sensors_event_group,
        READING_COMPLETED_BIT | READING_FAILED_BIT,
        pdTRUE,
        pdFALSE,
        pdMS_TO_TICKS(5000));

    if (bits & READING_FAILED_BIT)
    {
        ESP_LOGW(TAG, "Sensor reading failed");
        return false;
    }

    if (bits & READING_COMPLETED_BIT)
    {
        ESP_LOGI(TAG, "Telemetry reading completed successfully");
        return true;
    }

    ESP_LOGW(TAG, "Timeout waiting for telemetry reading");
    return false;
}