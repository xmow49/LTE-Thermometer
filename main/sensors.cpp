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
#include "esp_bit_defs.h"
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
static i2c_master_dev_handle_t aht20_dev_handle = NULL;

#define SHT21_SENSOR_ADDR 0x40
#define AHT20_SENSOR_ADDR 0x38
#define AHT20_CMD_TRIGGER 0xAC               // Command to trigger measurement
#define AHT20_CMD_SOFTRESET 0xBA             // Command to reset the sensor
#define AHT20_CMD_INIT 0xBE                  // Command to initialize the sensor
#define AT581X_STATUS_CMP_INT (2)            /* 1 --Out threshold range; 0 --In threshold range */
#define AT581X_STATUS_Calibration_Enable (3) /* 1 --Calibration enable; 0 --Calibration disable */
#define AT581X_STATUS_CRC_FLAG (4)           /* 1 --CRC ok; 0 --CRC failed */
#define AT581X_STATUS_MODE_STATUS (5)        /* 00 -NOR mode; 01 -CYC mode; 1x --CMD mode */
#define AT581X_STATUS_BUSY_INDICATION (7)    /* 1 --Equipment is busy; 0 --Equipment is idle */
static uint8_t aht20_calc_crc(uint8_t *data, uint8_t len)
{
    uint8_t i;
    uint8_t byte;
    uint8_t crc = 0xFF;

    for (byte = 0; byte < len; byte++)
    {
        crc ^= data[byte];
        for (i = 8; i > 0; --i)
        {
            if ((crc & 0x80) != 0)
            {
                crc = (crc << 1) ^ 0x31;
            }
            else
            {
                crc = crc << 1;
            }
        }
    }

    return crc;
}
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

esp_err_t aht20_init(void)
{
    esp_err_t ret;

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = AHT20_SENSOR_ADDR,
        .scl_speed_hz = 100000,
        .scl_wait_us = 1000,
        .flags = {
            .disable_ack_check = false}};

    ret = i2c_master_bus_add_device(i2c_bus, &dev_config, &aht20_dev_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add AHT20 device: %s", esp_err_to_name(ret));
        return ret;
    }

    // reset the sensor
    uint8_t cmd = AHT20_CMD_SOFTRESET;
    ret = i2c_master_transmit(aht20_dev_handle, &cmd, 1, pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send AHT20 reset command: %s", esp_err_to_name(ret));
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(20)); // wait for reset

    // initialize the sensor
    uint8_t cmd2[] = {AHT20_CMD_INIT, 0x08, 0x00};
    ret = i2c_master_transmit(aht20_dev_handle, cmd2, sizeof(cmd2), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send AHT20 init command: %s", esp_err_to_name(ret));
        return ret;
    }
    vTaskDelay(pdMS_TO_TICKS(20)); // wait for init
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

    return ESP_OK;
}

esp_err_t sensors_read_aht20(dht_data_t *data)
{
    if (aht20_dev_handle == NULL)
    {
        ESP_LOGE(TAG, "AHT20 device not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t raw[7];
    // Trigger measurement
    uint8_t cmd[3] = {AHT20_CMD_TRIGGER, 0x33, 0x00}; // Trigger measurement command
    esp_err_t ret = i2c_master_transmit(aht20_dev_handle, cmd, sizeof(cmd), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send AHT20 trigger command: %s", esp_err_to_name(ret));
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(100)); // wait for measurement

    // read status
    uint8_t status;
    ret = i2c_master_receive(aht20_dev_handle, &status, 1, pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read AHT20 status: %s", esp_err_to_name(ret));
        return ret;
    }

    if ((status & BIT(AT581X_STATUS_Calibration_Enable)) &&
        (status & BIT(AT581X_STATUS_CRC_FLAG)) &&
        ((status & BIT(AT581X_STATUS_BUSY_INDICATION)) == 0))
    {
        ret = i2c_master_receive(aht20_dev_handle, raw, sizeof(raw), pdMS_TO_TICKS(1000));
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to read AHT20 data: %s", esp_err_to_name(ret));
            return ret;
        }

        uint8_t crc = aht20_calc_crc(raw, 6);
        if (crc != raw[6])
        {
            ESP_LOGE(TAG, "AHT20 CRC check failed: %02X != %02X", crc, raw[6]);
            ESP_LOG_BUFFER_HEXDUMP(TAG, raw, sizeof(raw), ESP_LOG_INFO);

            return ESP_ERR_INVALID_CRC;
        }

        uint32_t raw_humidity = ((raw[1] << 16) | (raw[2] << 8) | raw[3]) >> 4;
        uint32_t raw_temperature = ((raw[3] & 0x0F) << 16) | (raw[4] << 8) | raw[5];

        data->humidity = ((float)raw_humidity / 1048576.0) * 100.0;
        data->temperature = ((float)raw_temperature / 1048576.0) * 200.0 - 50.0;
    }
    else
    {
        ESP_LOGE(TAG, "AHT20 not ready or calibration failed, status: 0x%02X", status);
        return ESP_ERR_NOT_FINISHED;
    }

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

esp_err_t sensors_read(dht_data_t *data)
{
    esp_err_t res = sensors_read_aht20(data);
    if (res == ESP_OK)
    {
        ESP_LOGI(TAG, "AHT20 read success: T=%.2f, H=%.2f", data->temperature, data->humidity);
        return res;
    }
    else
    {
        ESP_LOGW(TAG, "AHT20 read failed, trying SHT21: %s", esp_err_to_name(res));
    }

    res = sensors_read_sht21(data);
    if (res != ESP_OK)
    {
        ESP_LOGW(TAG, "SHT21 read failed: %s", esp_err_to_name(res));
        return res;
    }
    ESP_LOGI(TAG, "SHT21 read success: T=%.2f, H=%.2f", data->temperature, data->humidity);

    return res;
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

    ret = aht20_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize AHT20 sensor: %s", esp_err_to_name(ret));
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
        dht_data_t data_aht = {NAN, NAN};
        dht_data_t data_sht = {NAN, NAN};
        esp_err_t res = sensors_read_aht20(&data_aht);
        if (res != ESP_OK)
        {
            ESP_LOGW(TAG, "AHT20 read failed: %s", esp_err_to_name(res));
        }

        res = sensors_read_sht21(&data_sht);
        if (res != ESP_OK)
        {
            ESP_LOGW(TAG, "SHT21 read failed: %s", esp_err_to_name(res));
        }
        lcd_set_temperature(data_aht.temperature, data_aht.humidity);

        // if (res != ESP_OK)
        // {
        //     EventBits_t bits = xEventGroupGetBits(sensors_event_group);
        //     if (bits & READING_REQUESTED_BIT)
        //     {
        //         xEventGroupClearBits(sensors_event_group, READING_REQUESTED_BIT);
        //         xEventGroupSetBits(sensors_event_group, READING_FAILED_BIT);
        //     }
        //     vTaskDelay(pdMS_TO_TICKS(1000));
        //     continue;
        // }

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

        if (force_report || (fabs(data_aht.temperature - last_data.temperature) >= config.sensor_temperature_change_threshold))
        {
            ESP_LOGI(TAG, "Temperature %schange detected: %.2f -> %.2f", (force_report ? "forced " : ""), last_data.temperature, data_aht.temperature);
            ESP_LOGI(TAG, "Temperature2: %.2f", data_sht.temperature);
            device->addTelemetry(TelemetryReport("t", data_aht.temperature));
            device->addTelemetry(TelemetryReport("t2", data_sht.temperature));
            last_update = xTaskGetTickCount();
            last_data.temperature = data_aht.temperature;
        }

        if (force_report || (fabs(data_aht.humidity - last_data.humidity) >= config.sensor_humidity_change_threshold))
        {
            ESP_LOGI(TAG, "Humidity %schange detected: %.2f -> %.2f", (force_report ? "forced " : ""), last_data.humidity, data_aht.humidity);
            ESP_LOGI(TAG, "Humidity2: %.2f", data_sht.humidity);
            device->addTelemetry(TelemetryReport("h", data_aht.humidity));
            device->addTelemetry(TelemetryReport("h2", data_sht.humidity));
            last_update = xTaskGetTickCount();
            last_data.humidity = data_aht.humidity;
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