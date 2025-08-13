#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_modem_api.h"
#include "device.hpp"
#include "main.h"
#include "config.h"
#include "lcd.h"
#include "modem.h"

static const char *TAG = "MODEM";

#define BUF_SIZE 1024
#define UART_NUM UART_NUM_1
#define RX_PIN 17
#define TX_PIN 18

void modem_task(void *pvParameters);

esp_err_t send_at_command(const char *command, char *response, size_t response_size, int timeout_ms)
{
    uart_write_bytes(UART_NUM, command, strlen(command));
    uart_write_bytes(UART_NUM, "\r\n", 2);

    int len = uart_read_bytes(UART_NUM, response, response_size - 1, timeout_ms / portTICK_PERIOD_MS);
    if (len > 0)
    {
        response[len] = 0;
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t send_sms(const char *phone_number, const char *message)
{
    char response[BUF_SIZE];
    esp_err_t ret;

    // Set SMS mode to text
    ret = send_at_command("AT+CMGF=1", response, sizeof(response), 1000);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set SMS mode");
        return ret;
    }

    // Prepare the send SMS command
    char send_cmd[64];
    snprintf(send_cmd, sizeof(send_cmd), "AT+CMGS=\"%s\"", phone_number);

    // Send the command and wait for '>' prompt
    ret = send_at_command(send_cmd, response, sizeof(response), 5000);
    if (ret != ESP_OK || strstr(response, ">") == NULL)
    {
        ESP_LOGE(TAG, "Failed to start SMS sending");
        return ESP_FAIL;
    }

    // Send the message content
    uart_write_bytes(UART_NUM, message, strlen(message));

    // Send Ctrl+Z (0x1A) to indicate end of message
    uart_write_bytes(UART_NUM, "\x1A", 1);

    // Wait for confirmation
    ret = send_at_command("", response, sizeof(response), 10000);
    if (ret == ESP_OK && strstr(response, "+CMGS:") != NULL)
    {
        ESP_LOGI(TAG, "SMS sent successfully");
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to send SMS");
        return ESP_FAIL;
    }
}

esp_err_t init_at_modem()
{
    esp_err_t ret;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};

    ret = uart_param_config(UART_NUM, &uart_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure UART");
        return ret;
    }
    ret = uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set UART pins");
        return ret;
    }
    ret = uart_driver_install(UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to install UART driver");
        return ret;
    }
    xTaskCreate(modem_task, "modem_task", 4096, NULL, 5, NULL);
    return ESP_OK;
}

esp_err_t get_rssi(int *rssi, int *ber)
{
    char response[BUF_SIZE];
    esp_err_t ret;

    if (!main_network_connected())
    {
        return ESP_ERR_INVALID_STATE;
    }

    int _rssi, _ber;

    // Send AT command to get signal quality
    ret = send_at_command("AT+CSQ", response, sizeof(response), 1000);
    if (ret != ESP_OK)
    {
        return ret;
    }

    // Parse the response
    if (strstr(response, "+CSQ:"))
    {
        ret = sscanf(strstr(response, "+CSQ:"), "+CSQ: %d,%d", &_rssi, &_ber);
        if (ret != 2)
        {
            ESP_LOGI(TAG, "Ret: %d", ret);
            ESP_LOGI(TAG, "strstr: %s", strstr(response, "+CSQ:"));
            ESP_LOGE(TAG, "Failed to parse signal quality response");
            ESP_LOGE(TAG, "Response: %s", response);
            return ESP_FAIL;
        }
    }
    else
    {
        ESP_LOGE(TAG, "Failed to parse signal quality response");
        ESP_LOGE(TAG, "Response: %s", response);
        return ESP_FAIL;
    }

    if (rssi)
    {
        if (_rssi == 99)
            *rssi = -113;
        else
            *rssi = -113 + _rssi * 2;
    }
    if (ber)
        *ber = _ber;
    return ESP_OK;
}

esp_err_t modem_update_telemetry()
{
    esp_err_t ret;
    int rssi, ber;

    ret = get_rssi(&rssi, &ber);
    if (ret == ESP_OK)
    {
        if (main_network_connected())
        {
            ESP_LOGI(TAG, "RSSI: %d dBm, BER: %d", rssi, ber);
            device_report_telemetry(DEVICE_GATEWAY_MAC, "rssi", rssi);
        }
        else
        {
            // lcd_setup_msg("Réseau", "très faible !");
        }
        lcd_set_signal_rssi(rssi);
    }
    else
    {
        if (ret != ESP_ERR_INVALID_STATE)
        {
            ESP_LOGE(TAG, "Failed to get RSSI: %s", esp_err_to_name(ret));
        }
    }
    return ret;
}

void modem_task(void *pvParameters)
{
    int rssi, ber;
    esp_err_t ret;
    TickType_t last_update = 0;
    bool first_update = true;
    while (1)
    {
        if ((xTaskGetTickCount() - last_update) >= pdMS_TO_TICKS(UPDATE_MODEM_INTERVAL_S * 1000) || first_update)
        {
            ret = modem_update_telemetry(&rssi, &ber);
            if (ret == ESP_OK)
            {
                last_update = xTaskGetTickCount();
                first_update = false;
            }
            else
            {
                ESP_LOGE(TAG, "Failed to update modem telemetry: %s", esp_err_to_name(ret));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
