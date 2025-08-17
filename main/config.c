#include "config.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

#define CONFIG_NVS_NAMESPACE "config"
#define CONFIG_NVS_KEY "device_config"

static const char *TAG = "CONFIG";

config_t config;

const config_entry_t config_entries[] = {
    {"interval_modem", CONFIG_ENTRY_TYPE_UINT32, &config.interval_modem},
    {"interval_stats", CONFIG_ENTRY_TYPE_UINT32, &config.interval_stats},
    {"interval_battery", CONFIG_ENTRY_TYPE_UINT32, &config.interval_battery},
    {"interval_ping", CONFIG_ENTRY_TYPE_UINT32, &config.interval_ping},
    {"interval_send_to_tb", CONFIG_ENTRY_TYPE_UINT32, &config.interval_send_to_tb},
    {"time_same_timestamp", CONFIG_ENTRY_TYPE_UINT32, &config.time_same_timestamp},
    {"interval_lcd_timeout", CONFIG_ENTRY_TYPE_UINT32, &config.interval_lcd_timeout},
    {"sensor_temperature_change_threshold", CONFIG_ENTRY_TYPE_FLOAT, &config.sensor_temperature_change_threshold},
    {"sensor_humidity_change_threshold", CONFIG_ENTRY_TYPE_FLOAT, &config.sensor_humidity_change_threshold},
    {"sensor_interval_force_update", CONFIG_ENTRY_TYPE_UINT32, &config.sensor_interval_force_update},
    {"lcd_notify_power_cut", CONFIG_ENTRY_TYPE_BOOL, &config.lcd_notify_power_cut},
    {NULL, CONFIG_ENTRY_TYPE_NONE, NULL} // Sentinel
};

const uint32_t config_entries_count = sizeof(config_entries) / sizeof(config_entries[0]);

void config_init()
{
    if (!config_load())
    {
        ESP_LOGW(TAG, "Failed to load config, using default values");
        config_set_default();
        config_save();
    }
}

void config_set_default()
{
    config.interval_modem = 10 * 60;
    config.interval_stats = 10 * 60;
    config.interval_battery = 10 * 60;
    config.interval_ping = 10 * 60;
    config.interval_send_to_tb = 10 * 60;

    config.time_same_timestamp = 3;
    config.interval_lcd_timeout = 20;

    config.sensor_temperature_change_threshold = 1.0f;
    config.sensor_humidity_change_threshold = 1.0f;
    config.sensor_interval_force_update = (10 * 60);
}

bool config_save()
{
    nvs_handle_t nvs_handle;
    esp_err_t err;

    // Initialize NVS
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) initializing NVS!", esp_err_to_name(err));
        return false;
    }

    // Open NVS handle
    err = nvs_open(CONFIG_NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return false;
    }

    // Save the config structure as a blob
    err = nvs_set_blob(nvs_handle, CONFIG_NVS_KEY, &config, sizeof(config_t));
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) saving config to NVS!", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return false;
    }

    // Commit written value
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) committing config to NVS!", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return false;
    }

    // Close NVS handle
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Configuration saved to NVS successfully");
    return true;
}

bool config_load()
{
    nvs_handle_t nvs_handle;
    esp_err_t err;

    // Initialize NVS
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open NVS handle
    err = nvs_open(CONFIG_NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return false;
    }

    // Get the size of the saved config
    size_t required_size = sizeof(config_t);
    err = nvs_get_blob(nvs_handle, CONFIG_NVS_KEY, &config, &required_size);
    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "No config found in NVS (%s)", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return false;
    }

    // Verify the size matches
    if (required_size != sizeof(config_t))
    {
        ESP_LOGW(TAG, "Config size mismatch (expected %d, got %d)", sizeof(config_t), required_size);
        nvs_close(nvs_handle);
        return false;
    }

    // Close NVS handle
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Configuration loaded from NVS successfully");
    ESP_LOGI(TAG, "\tinterval_modem: %d", config.interval_modem);
    ESP_LOGI(TAG, "\tinterval_stats: %d", config.interval_stats);
    ESP_LOGI(TAG, "\tinterval_battery: %d", config.interval_battery);
    ESP_LOGI(TAG, "\tinterval_ping: %d", config.interval_ping);
    ESP_LOGI(TAG, "\tinterval_send_to_tb: %d", config.interval_send_to_tb);
    ESP_LOGI(TAG, "\ttime_same_timestamp: %d", config.time_same_timestamp);
    ESP_LOGI(TAG, "\tinterval_lcd_timeout: %d", config.interval_lcd_timeout);
    ESP_LOGI(TAG, "\tsensor_temperature_change_threshold: %f", config.sensor_temperature_change_threshold);
    ESP_LOGI(TAG, "\tsensor_humidity_change_threshold: %f", config.sensor_humidity_change_threshold);
    ESP_LOGI(TAG, "\tsensor_interval_force_update: %d", config.sensor_interval_force_update);

    return true;
}
