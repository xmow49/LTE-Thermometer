#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "ping/ping_sock.h"
#include "esp_log.h"
#include "device.hpp"
#include "main.h"
#include "config.h"

#define TAG "ping"

static esp_ping_handle_t ping;
static int ping_value = -1;
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
    device_report_telemetry(DEVICE_GATEWAY_MAC, "ping", ping_value);
}

void ping_init()
{
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
    xTaskCreate(ping_task, "ping_task", 4096, NULL, 5, NULL);
}

void ping_task(void *pvParameters)
{

    while (1)
    {
        if (!main_network_connected())
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        esp_ping_start(ping);
        vTaskDelay(pdMS_TO_TICKS(UPDATE_PING_INTERVAL_S * 1000));
    }
}

int ping_get_value()
{
    return ping_value;
}
