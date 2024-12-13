#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_now.h"
#include "espnow.h"
#include "device.hpp"
#include "esp_mac.h"
#include "esp_wifi.h"

static const char *TAG = "ESP-NOW";

#define ESPNOW_CHANNEL 1
static QueueHandle_t s_espnow_queue;

typedef struct
{
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    esp_now_send_status_t status;
} espnow_event_send_cb_t;

typedef struct
{
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    uint8_t *data;
    int data_len;
} espnow_event_recv_cb_t;

typedef union
{
    espnow_event_send_cb_t send_cb;
    espnow_event_recv_cb_t recv_cb;
} espnow_event_info_t;

typedef enum
{
    ESPNOW_SEND_CB,
    ESPNOW_RECV_CB,
} espnow_event_id_t;

typedef struct
{
    espnow_event_id_t id;
    espnow_event_info_t info;
} espnow_event_t;

static void espnow_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "WiFi started, initializing ESP-NOW");
        esp_now_init();
    }
}

static void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    espnow_event_t evt;
    evt.id = ESPNOW_SEND_CB;
    memcpy(evt.info.send_cb.mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
    evt.info.send_cb.status = status;
    if (xQueueSend(s_espnow_queue, &evt, portMAX_DELAY) != pdTRUE)
    {
        ESP_LOGW(TAG, "Send send queue fail");
    }
}

static void espnow_recv_cb(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int len)
{
    espnow_event_t evt;
    evt.id = ESPNOW_RECV_CB;
    memcpy(evt.info.recv_cb.mac_addr, esp_now_info->src_addr, ESP_NOW_ETH_ALEN);
    evt.info.recv_cb.data = malloc(len);
    if (evt.info.recv_cb.data == NULL)
    {
        ESP_LOGE(TAG, "Malloc receive data fail");
        return;
    }
    memcpy(evt.info.recv_cb.data, data, len);
    evt.info.recv_cb.data_len = len;
    if (xQueueSend(s_espnow_queue, &evt, portMAX_DELAY) != pdTRUE)
    {
        ESP_LOGW(TAG, "Send receive queue fail");
        free(evt.info.recv_cb.data);
    }
}

void espnow_task(void *pvParameter)
{
    espnow_event_t evt;
    while (xQueueReceive(s_espnow_queue, &evt, portMAX_DELAY) == pdTRUE)
    {
        switch (evt.id)
        {
        case ESPNOW_SEND_CB:
            ESP_LOGI(TAG, "Send data to " MACSTR ", status: %d", MAC2STR(evt.info.send_cb.mac_addr), evt.info.send_cb.status);
            break;
        case ESPNOW_RECV_CB:
            ESP_LOGI(TAG, "Receive data from: " MACSTR " len: %d", MAC2STR(evt.info.recv_cb.mac_addr), evt.info.recv_cb.data_len);
            // Process received data
            if (evt.info.recv_cb.data_len == sizeof(telemetry_message))
            {
                telemetry_message data = *(telemetry_message *)evt.info.recv_cb.data;
                char mac_str[18];
                snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
                         evt.info.recv_cb.mac_addr[0], evt.info.recv_cb.mac_addr[1], evt.info.recv_cb.mac_addr[2],
                         evt.info.recv_cb.mac_addr[3], evt.info.recv_cb.mac_addr[4], evt.info.recv_cb.mac_addr[5]);
                device_receive(mac_str, data);
            }
            free(evt.info.recv_cb.data);
            break;
        default:
            ESP_LOGE(TAG, "Callback type error: %d", evt.id);
            break;
        }
    }
}

esp_err_t espnow_init(void)
{
    s_espnow_queue = xQueueCreate(10, sizeof(espnow_event_t));
    if (s_espnow_queue == NULL)
    {
        ESP_LOGE(TAG, "Create mutex fail");
        return ESP_FAIL;
    }

    // Set custom MAC address
    uint8_t custom_mac[6] = {0x84, 0x0D, 0x8E, 0xD2, 0x31, 0xE4};
    esp_base_mac_addr_set(custom_mac);
    ESP_LOGI(TAG, "Custom MAC Address set: " MACSTR, MAC2STR(custom_mac));

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &espnow_event_handler, NULL, NULL);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE));

    // Ajout de l'affichage de l'adresse MAC
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    ESP_LOGI(TAG, "My MAC Address: " MACSTR, MAC2STR(mac));

    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));

    esp_now_peer_info_t broadcast_peer = {
        .channel = 0,
        .ifidx = ESP_IF_WIFI_STA,
        .encrypt = false,
        .peer_addr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    };
    ESP_ERROR_CHECK(esp_now_add_peer(&broadcast_peer));

    xTaskCreate(espnow_task, "espnow_task", 4 * 1024, NULL, 4, NULL);

    return ESP_OK;
}
