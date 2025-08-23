#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "ping/ping_sock.h"
#include "esp_log.h"
#include "device.hpp"
#include "main.h"
#include "config.h"

#define TAG "ping"
#define PING_END_BIT BIT0

static esp_ping_handle_t ping;
static int ping_value = -1;
static EventGroupHandle_t ping_event_group;

static void ping_task(void *pvParameters);

static void on_ping_success(esp_ping_handle_t hdl, void *args)
{
    uint8_t ttl;
    uint16_t seqno;
    uint32_t elapsed_time, recv_len;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
    ESP_LOGI(TAG, "%" PRIu32 " bytes from %s icmp_seq=%u ttl=%u time=%" PRIu32 " ms\n", recv_len, ipaddr_ntoa(&target_addr), seqno, ttl, elapsed_time);
    ping_value = elapsed_time;
    xEventGroupSetBits(ping_event_group, PING_END_BIT);
    device_report_telemetry(DEVICE_GATEWAY_MAC, "ping", ping_value);
}

static void on_ping_timeout(esp_ping_handle_t hdl, void *args)
{
    uint16_t seqno;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    ESP_LOGW(TAG, "From %s icmp_seq=%u timeout\n", ipaddr_ntoa(&target_addr), seqno);
    ping_value = -1;
    xEventGroupSetBits(ping_event_group, PING_END_BIT);
    device_report_telemetry(DEVICE_GATEWAY_MAC, "ping", ping_value);
}

void ping_init()
{
    ping_event_group = xEventGroupCreate();
    if (ping_event_group == NULL)
    {
        ESP_LOGE(TAG, "Failed to create ping event group");
        return;
    }

    ip_addr_t target_addr;
    memset(&target_addr, 0, sizeof(target_addr));
    char *ping_addr_s = NULL;
    ping_addr_s = "8.8.8.8";

    esp_ping_config_t ping_config = ESP_PING_DEFAULT_CONFIG();
    ipaddr_aton(ping_addr_s, &target_addr);
    ping_config.target_addr = target_addr;
    ping_config.timeout_ms = 1000;
    ping_config.task_stack_size = 4096;
    ping_config.count = 1;
    ping_config.data_size = 16;

    /* set callback functions */
    esp_ping_callbacks_t cbs = {
        .on_ping_success = on_ping_success,
        .on_ping_timeout = on_ping_timeout,
        .on_ping_end = NULL,
        .cb_args = NULL,
    };
    esp_ping_new_session(&ping_config, &cbs, &ping);
    xTaskCreate(ping_task, "ping_task", 8 * 1024, NULL, 5, NULL);
}

void ping_task(void *pvParameters)
{

    uint32_t tries = 0;
    uint32_t network_lost_time = 0;
    bool network_lost_time_set = false;

    uint32_t last_ping_time = 0;
    bool first_ping = true;
    while (1)
    {
        if (!main_network_attached())
        {
            ESP_LOGW(TAG, "Network not attached, waiting for connection");

            // Only set the network lost time once when network is first lost
            if (!network_lost_time_set)
            {
                network_lost_time = pdMS_TO_TICKS(xTaskGetTickCount());
                network_lost_time_set = true;
                ESP_LOGI(TAG, "Network lost time recorded");
            }

            // Check if network has been lost for more than 1 hour
            if (pdMS_TO_TICKS(xTaskGetTickCount()) - network_lost_time > pdMS_TO_TICKS(config.interval_network_error_reboot * 1000))
            {
                ESP_LOGI(TAG, "Network lost for more than %d seconds: restarting", config.interval_network_error_reboot);
                vTaskDelay(pdMS_TO_TICKS(1000));
                esp_restart();
            }

            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        // Network is attached, reset the network lost time tracking
        if (network_lost_time_set)
        {
            ESP_LOGI(TAG, "Network reconnected, resetting lost time tracking");
            network_lost_time_set = false;
            network_lost_time = 0;
        }

        if (first_ping || (pdMS_TO_TICKS(xTaskGetTickCount()) - last_ping_time) > pdMS_TO_TICKS(config.interval_ping * 1000)) // time to ping
        {
            xEventGroupClearBits(ping_event_group, PING_END_BIT);
            ESP_LOGI(TAG, "Starting ping");
            esp_ping_start(ping);
            tries++;
            xEventGroupWaitBits(ping_event_group, PING_END_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
            // Process ping result
            if (ping_value != -1)
            {
                ESP_LOGI(TAG, "Ping successful: %d ms", ping_value);
                tries = 0;
                last_ping_time = pdMS_TO_TICKS(xTaskGetTickCount()); // Update last ping time on success
                first_ping = false;
            }
            else
            {
                ESP_LOGW(TAG, "Ping failed (attempt %d)", tries);
                if (tries >= 3)
                {
                    ESP_LOGE(TAG, "Ping failed 3 times in a row, retrying in a minute");
                    tries = 0;                                           // Reset tries after waiting
                    last_ping_time = pdMS_TO_TICKS(xTaskGetTickCount()); // Update time to wait full interval
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Wait before checking again
    }
}

int ping_get_value()
{
    return ping_value;
}
