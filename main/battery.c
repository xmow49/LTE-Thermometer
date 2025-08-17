#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "device.hpp"
#include "main.h"
#include "lcd.h"
#include "config.h"
#include "pins.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "thingsboard.hpp"
#include "battery.h"

#define I2C_MASTER_SCL_IO 2       // SCL pin
#define I2C_MASTER_SDA_IO 3       // SDA pin
#define I2C_MASTER_FREQ_HZ 100000 // I2C master clock frequency
#define MAX17048_I2C_ADDRESS 0x36 // I2C address of MAX17048

static const char *TAG = "MAX17048";

static i2c_master_bus_handle_t i2c_bus_handle;
static i2c_master_dev_handle_t i2c_device_handle;
static adc_oneshot_unit_handle_t adc2_handle;
static adc_cali_handle_t adc_cali_handle = NULL;

esp_err_t i2c_master_init(void)
{
    esp_err_t ret;
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_1,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ret = i2c_new_master_bus(&bus_config, &i2c_bus_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error initializing I2C master: %s", esp_err_to_name(ret));
        return ret;
    }

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = MAX17048_I2C_ADDRESS,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    ret = i2c_master_bus_add_device(i2c_bus_handle, &dev_config, &i2c_device_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error adding device to I2C master: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

esp_err_t read_battery_percent(float *battery_percent)
{
    uint8_t reg_addr = 0x04;
    uint8_t data[2];

    esp_err_t ret = i2c_master_transmit_receive(i2c_device_handle, &reg_addr, 1, data, 2, -1);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error transmitting data: %s", esp_err_to_name(ret));
        return ret;
    }

    float percent = (data[0] << 8) | data[1];
    percent = percent / 256.0;
    if (percent > 100)
    {
        percent = 100;
    }
    *battery_percent = percent;
    return ESP_OK;
}

esp_err_t read_battery_voltage(float *battery_voltage)
{
    uint8_t reg_addr = 0x02;
    uint8_t data[2];

    esp_err_t ret = i2c_master_transmit_receive(i2c_device_handle, &reg_addr, 1, data, 2, -1);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error transmitting data: %s", esp_err_to_name(ret));
        return ret;
    }

    uint16_t raw_voltage = (data[0] << 8) | data[1];
    *battery_voltage = raw_voltage * 78.125 / 1000000.0; // Convert to volts
    return ESP_OK;
}

esp_err_t read_battery_charge_rate(float *battery_charge_rate)
{
    uint8_t reg_addr = 0x16;
    uint8_t data[2];

    esp_err_t ret = i2c_master_transmit_receive(i2c_device_handle, &reg_addr, 1, data, 2, -1);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error transmitting data: %s", esp_err_to_name(ret));
        return ret;
    }

    int16_t raw_charge_rate = (data[0] << 8) | data[1];
    *battery_charge_rate = raw_charge_rate * 0.208; // Convert to %/hr
    return ESP_OK;
}

esp_err_t battery_update_telemetry()
{
    esp_err_t ret;
    float battery_percent, battery_voltage, battery_charge_rate;

    ret = read_battery_percent(&battery_percent);
    if (ret == ESP_OK)
    {
        device_report_telemetry(DEVICE_GATEWAY_MAC, "b_p", battery_percent);
        lcd_set_battery_level(battery_percent);
    }
    else
    {
        ESP_LOGE(TAG, "Error reading battery level: %s", esp_err_to_name(ret));
    }

    ret = read_battery_voltage(&battery_voltage);
    if (ret == ESP_OK)
    {
        device_report_telemetry(DEVICE_GATEWAY_MAC, "b_v", battery_voltage);
    }
    else
    {
        ESP_LOGE(TAG, "Error reading battery voltage: %s", esp_err_to_name(ret));
    }

    ret = read_battery_charge_rate(&battery_charge_rate);
    if (ret == ESP_OK)
    {
        device_report_telemetry(DEVICE_GATEWAY_MAC, "b_r", battery_charge_rate);
    }
    else
    {
        ESP_LOGE(TAG, "Error reading battery charge rate: %s", esp_err_to_name(ret));
    }

    return ESP_OK;
}

void battery_task(void *pvParameters)
{
    esp_err_t ret = i2c_master_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error initializing I2C master: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }

    while (1)
    {

        if (!main_network_connected())
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        ret = battery_update_telemetry();
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error updating battery telemetry: %s", esp_err_to_name(ret));
        }

        vTaskDelay(pdMS_TO_TICKS(config.interval_battery * 1000));
    }
}

static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated)
    {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK)
        {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated)
    {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK)
        {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Calibration Success");
    }
    else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated)
    {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    }
    else
    {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}

uint16_t battery_get_voltage(bool with_calibration)
{
    esp_err_t ret;
    int adc_raw, voltage_mv;
    ret = adc_oneshot_read(adc2_handle, PIN_BATTERY_ADC, &adc_raw);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error reading ADC: %s", esp_err_to_name(ret));
        return 0; // Return 0 if ADC read fails
    }

    if (with_calibration)
    {
        adc_cali_raw_to_voltage(adc_cali_handle, adc_raw, &voltage_mv);
    }
    else
    {
        voltage_mv = (adc_raw * 3300) / 4095; // Assuming 3.3V reference and 12-bit ADC
    }

    voltage_mv *= 2; // Because of voltage divider

    return voltage_mv;
}

void power_cut_task(void *pvParameters)
{
    esp_err_t ret;
    bool last_power_cut = false;

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ret = adc_oneshot_new_unit(&init_config, &adc2_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error initializing ADC: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }

    bool do_calibration = adc_calibration_init(ADC_UNIT_2, PIN_BATTERY_ADC, ADC_ATTEN_DB_12, &adc_cali_handle);

    // ADC Configuration
    adc_oneshot_chan_cfg_t adc_config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    ret = adc_oneshot_config_channel(adc2_handle, PIN_BATTERY_ADC, &adc_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error configuring ADC channel: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }

    bool first_cycle = true;

    while (1)
    {

        // 4.36V --> batt
        // 4.60V --> power

        bool power_cut = false;

        // Take 3 measurements and calculate average
        uint16_t voltage_sum = 0;
        uint8_t valid_readings = 0;

        for (int i = 0; i < 3; i++)
        {
            uint16_t reading = battery_get_voltage(do_calibration);
            if (reading > 0)
            {
                voltage_sum += reading;
                valid_readings++;
            }
            vTaskDelay(pdMS_TO_TICKS(10)); // Small delay between readings
        }

        uint16_t voltage_mv = 0;
        if (valid_readings > 0)
        {
            voltage_mv = voltage_sum / valid_readings;
        }
        else
        {
            ESP_LOGW(TAG, "All battery voltage readings were 0!");
        }

        if (voltage_mv < 4600)
        {
            power_cut = true;
        }

        if (power_cut != last_power_cut || first_cycle)
        {
            last_power_cut = power_cut;
            lcd_set_plugged(!power_cut);

            if (!first_cycle)
            {
                if (power_cut)
                {
                    ESP_LOGI(TAG, "Power Cut: %d mV", voltage_mv);
                    if (config.lcd_notify)
                    {
                        lcd_setup_msg("Alimentation", "sur batterie");
                        vTaskDelay(pdMS_TO_TICKS(2000));
                        lcd_update();
                    }
                }
                else
                {
                    ESP_LOGI(TAG, "Power Restored: %d mV", voltage_mv);
                    if (config.lcd_notify)
                    {
                        lcd_setup_msg("Alimentation", "sur secteur");
                        vTaskDelay(pdMS_TO_TICKS(2000));
                        lcd_update();
                    }
                }
            }
            device_report_telemetry(DEVICE_GATEWAY_MAC, "pw", !power_cut);
            tb_force_update();
            first_cycle = false;
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for 1 second
    }
}

void battery_init()
{
    xTaskCreate(battery_task, "battery_task", 4 * 1024, NULL, 5, NULL);
    xTaskCreate(power_cut_task, "power_cut_task", 4 * 1024, NULL, 5, NULL);
}
