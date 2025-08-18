#include "esp_sntp.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "MY_TIME"

static bool is_time_set = false;

esp_err_t my_time_update()
{
    // Synchronize time with NTP server
    ESP_LOGI(TAG, "Initializing SNTP");

    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_setservername(1, "time.google.com");
    esp_sntp_setservername(2, "time.windows.com");
    esp_sntp_setservername(3, "time.apple.com");
    esp_sntp_init();

    // Wait for time to be set
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
    {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    if (retry == retry_count)
    {
        ESP_LOGE(TAG, "Failed to sync time with NTP server");
        is_time_set = false;
        return ESP_FAIL;
    }

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time is: %s", strftime_buf);
    is_time_set = true;
    return ESP_OK;
}

bool my_time_is_set()
{
    return is_time_set;
}
