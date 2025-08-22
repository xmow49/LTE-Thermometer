#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_netif.h"
#include "led_indicator.h"
#include "led_indicator_blink_default.h"
#include "usbh_modem_board.h"
#include "usbh_modem_wifi.h"
#include "driver/temperature_sensor.h"

#include "thingsboard.hpp"
#include "ping.h"
#include "espnow.h"
#include "lcd.h"
#include "modem.h"
#include "sd.h"
#include "sensors.hpp"
#include "button.h"
#include "battery.h"
#include "my_time.h"
#include "device.hpp"
#include "config.h"
#include "crash.h"
#include "main.h"
#include "logs.h"
#define TAG "main"

#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/event_groups.h"

#define USE_WIFI 0

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

#define REBOOT_INTERVAL_DAYS 7
#define SECONDS_PER_DAY (24 * 60 * 60)

static const uint32_t REBOOT_INTERVAL_S = REBOOT_INTERVAL_DAYS * SECONDS_PER_DAY;

static EventGroupHandle_t s_wifi_event_group;
static bool network_connected = false;
static bool sim_ok = false;
static bool modem_initialized = false;
static temperature_sensor_handle_t temp_handle = NULL;

void reboot_task(void *pvParameters);

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
        network_connected = false;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        esp_wifi_connect();
        network_connected = false;
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        network_connected = true;
    }
}

esp_err_t wifi_connect_sta(const char *ssid, const char *pass, uint32_t timeout_ms)
{
    s_wifi_event_group = xEventGroupCreate();

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = "",
        },
    };
    strlcpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strlcpy((char *)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());

    ESP_LOGI(TAG, "wifi_connect_sta finished.");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           pdMS_TO_TICKS(timeout_ms));

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s", ssid);
        network_connected = true;
        return ESP_OK;
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s", ssid);
        network_connected = false;
        return ESP_FAIL;
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        network_connected = false;
        return ESP_ERR_TIMEOUT;
    }
}

static void on_modem_event(void *arg, esp_event_base_t event_base,
                           int32_t event_id, void *event_data)
{
    if (event_base == MODEM_BOARD_EVENT)
    {
        switch (event_id)
        {
        case MODEM_EVENT_SIMCARD_DISCONN:
            ESP_LOGW(TAG, "Modem Board Event: SIM Card disconnected");
            // lcd_setup_msg("Erreur", "SIM déconnectée");
            sim_ok = false;
            // led_indicator_start(s_led_system_handle, BLINK_CONNECTED);
            break;

        case MODEM_EVENT_SIMCARD_CONN:
            ESP_LOGI(TAG, "Modem Board Event: SIM Card Connected");
            // led_indicator_stop(s_led_system_handle, BLINK_CONNECTED);
            lcd_setup_msg("SIM OK", NULL);
            sim_ok = true;
            break;

        case MODEM_EVENT_DTE_DISCONN:
            ESP_LOGW(TAG, "Modem Board Event: USB disconnected");
            // led_indicator_start(s_led_system_handle, BLINK_CONNECTING);
            network_connected = false;
            break;

        case MODEM_EVENT_DTE_CONN:
            ESP_LOGI(TAG, "Modem Board Event: USB connected");
            // led_indicator_stop(s_led_system_handle, BLINK_CONNECTED);
            // led_indicator_stop(s_led_system_handle, BLINK_CONNECTING);
            break;

        case MODEM_EVENT_DTE_RESTART:
            ESP_LOGW(TAG, "Modem Board Event: Hardware restart");
            // led_indicator_start(s_led_system_handle, BLINK_CONNECTED);
            break;

        case MODEM_EVENT_DTE_RESTART_DONE:
            ESP_LOGI(TAG, "Modem Board Event: Hardware restart done");
            // led_indicator_stop(s_led_system_handle, BLINK_CONNECTED);
            break;

        case MODEM_EVENT_NET_CONN:
            ESP_LOGI(TAG, "Modem Board Event: Network connected");
            lcd_setup_msg("Réseau OK", NULL);
            // led_indicator_start(s_led_4g_handle, BLINK_CONNECTED);
            network_connected = true;
            break;

        case MODEM_EVENT_NET_DISCONN:
            ESP_LOGW(TAG, "Modem Board Event: Network disconnected");
            if (sim_ok)
            {
                lcd_setup_msg("Réseau", "déconnecté");
            }
            else
            {
                lcd_setup_msg("Erreur", "SIM");
            }
            // led_indicator_stop(s_led_4g_handle, BLINK_CONNECTED);
            network_connected = false;
            break;

        case MODEM_EVENT_WIFI_STA_CONN:
            ESP_LOGI(TAG, "Modem Board Event: Station connected");
            // led_indicator_start(s_led_wifi_handle, BLINK_CONNECTED);
            break;

        case MODEM_EVENT_WIFI_STA_DISCONN:
            ESP_LOGW(TAG, "Modem Board Event: All stations disconnected");
            // led_indicator_stop(s_led_wifi_handle, BLINK_CONNECTED);
            break;

        default:
            ESP_LOGW(TAG, "Unhandled modem event: %ld", event_id);
            break;
        }
    }
    else
    {
        ESP_LOGW(TAG, "Unknown event: 0x%lx 0x%lx", (unsigned long)event_base, event_id);
    }
}

bool main_network_attached()
{
    return network_connected && modem_initialized;
}

bool main_network_connected()
{
    return network_connected && modem_initialized && ping_get_value() != -1;
}

void app_main(void)
{
    logs_init();
    button_init();
    // xTaskCreate(reboot_task, "reboot_task", 4 * 1024, NULL, 1, NULL);

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    config_init();
    lcd_init();

    if (button_get_state())
    {
        ESP_LOGI(TAG, "Button pressed on startup, stop boot here");
        config.lcd_notify = true;
        lcd_setup_msg("DEBUG MODE", "Please restart");
        while (button_get_state())
        {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
        TickType_t start_time = xTaskGetTickCount();
        while (xTaskGetTickCount() - start_time < pdMS_TO_TICKS(600 * 1000))
        {
            if (button_get_state())
            {
                ESP_LOGI(TAG, "Button pressed again, rebooting...");
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        ESP_LOGI(TAG, "Rebooting...");
        esp_restart();
    }

#if !USE_WIFI // espnow conflicts with wifi
    espnow_init();
#endif

#if !USE_WIFI
    init_at_modem();
#endif

    ESP_LOGI(TAG, "Initializing modem in 3s");
    lcd_setup_msg("Thermomètre", "4G");
    vTaskDelay(pdMS_TO_TICKS(2000));
    /* Initialize modem board. Dial-up internet */
    modem_config_t modem_config = MODEM_DEFAULT_CONFIG();
    /* Modem init flag, used to control init process */
    // #ifndef CONFIG_EXAMPLE_ENTER_PPP_DURING_INIT
    //     /* if Not enter ppp, modem will enter command mode after init */
    //     modem_config.flags |= MODEM_FLAGS_INIT_NOT_ENTER_PPP;
    //     /* if Not waiting for modem ready, just return after modem init */
    // modem_config.flags |= MODEM_FLAGS_INIT_NOT_BLOCK;
    // #endif
    modem_config.handler = on_modem_event;

    lcd_setup_msg("Démarrage", "du modem");

#if USE_WIFI
    wifi_connect_sta("AFDD-IoT", "fAvaD4eWp7pI5W", 10000);
#else
    modem_board_init(&modem_config);
#endif

    modem_initialized = true;
    ESP_LOGI(TAG, "Modem initialized");
    ping_init();

    lcd_setup_msg("Modem OK", NULL);

    vTaskDelay(pdMS_TO_TICKS(2000));

    lcd_setup_msg("Connexion", "à Internet...");
    while (!main_network_connected())
    {
        ESP_LOGI(TAG, "Waiting for network connection...: %d ms", ping_get_value());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    lcd_setup_msg("Récupération", "de l'heure");
    esp_err_t err = my_time_update();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to update time: %s", esp_err_to_name(err));
        lcd_setup_msg("Erreur", "heure");
    }
    else
    {
        lcd_setup_msg("Heure OK", NULL);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    tb_init();

    lcd_start();
    modem_update_telemetry();

    xTaskCreate(sensors_task, "sensors_task", 4 * 1024, NULL, 5, NULL);
    device_start_telemetry_save_task(); // Start telemetry save task
    button_start();
    battery_init();

    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(20, 50);
    ret = temperature_sensor_install(&temp_sensor_config, &temp_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to install temperature sensor: %s", esp_err_to_name(ret));
    }

    // send_sms("+33000000000", "Hello.");
    while (1)
    {
        main_report_telemetry();
        vTaskDelay(pdMS_TO_TICKS(config.interval_stats * 1000));
    }
}

void main_report_telemetry()
{
    esp_err_t ret;
    device_report_telemetry(DEVICE_GATEWAY_MAC, "ram", esp_get_free_heap_size());
    device_report_telemetry(DEVICE_GATEWAY_MAC, "heap", esp_get_free_internal_heap_size());
    device_report_telemetry(DEVICE_GATEWAY_MAC, "up", pdTICKS_TO_MS(xTaskGetTickCount()) / 1000);

    ret = temperature_sensor_enable(temp_handle);
    if (ret == ESP_OK)
    {
        float tsens_out;
        ret = temperature_sensor_get_celsius(temp_handle, &tsens_out);
        if (ret == ESP_OK)
        {
            device_report_telemetry(DEVICE_GATEWAY_MAC, "tcpu", tsens_out);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to get temperature sensor reading: %s", esp_err_to_name(ret));
        }
        ret = temperature_sensor_disable(temp_handle);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to disable temperature sensor: %s", esp_err_to_name(ret));
        }
    }
}

void reboot_task(void *pvParameters)
{
    uint32_t hours_count = REBOOT_INTERVAL_S / (60 * 60);
    uint32_t counter = 0;
    ESP_LOGI(TAG, "Reboot scheduled in %ld hours", hours_count);
    while (1)
    {
        counter++;
        if (counter >= hours_count)
        {
            ESP_LOGI(TAG, "Rebooting...");
            vTaskDelay(pdMS_TO_TICKS(1000)); // Small delay to allow log to be printed
            esp_restart();
        }
        vTaskDelay(pdMS_TO_TICKS(60 * 60 * 1000)); // Check every hour
    }
}