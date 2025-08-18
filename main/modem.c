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

// Décommentez cette ligne pour activer le mode bypass
// #define MODEM_BYPASS_MODE

static const char *TAG = "MODEM";

#define BUF_SIZE 1024
#define UART_NUM UART_NUM_1
#define RX_PIN 17
#define TX_PIN 18

// UART0 est l'UART du PC (USB) - peut créer des conflits avec la console ESP-IDF
// Alternative: utiliser UART2 avec des pins dédiées pour éviter les conflits
#define PC_UART_NUM UART_NUM_0
#define PC_RX_PIN 44 // Pin RX pour UART2
#define PC_TX_PIN 43 // Pin TX pour UART2

void modem_task(void *pvParameters);

#ifdef MODEM_BYPASS_MODE
void modem_bypass_task(void *pvParameters);
#endif

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

esp_err_t send_at_command_wait_response(const char *command, char *wanted_response, char *out_response, size_t response_size, int timeout_ms)
{
    uart_write_bytes(UART_NUM, command, strlen(command));
    uart_write_bytes(UART_NUM, "\r\n", 2);

    TickType_t start_time = xTaskGetTickCount();
    while (xTaskGetTickCount() - start_time < pdMS_TO_TICKS(timeout_ms))
    {
        int len = uart_read_bytes(UART_NUM, out_response, response_size - 1, pdMS_TO_TICKS(100));
        if (len > 0)
        {
            out_response[len] = 0;
            if (strstr(out_response, wanted_response) != NULL)
            {
                return ESP_OK; // Réponse trouvée
            }
        }
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

    // Configuration de l'UART du modem (UART1)
    ret = uart_param_config(UART_NUM, &uart_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure modem UART");
        return ret;
    }
    ret = uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set modem UART pins");
        return ret;
    }
    ret = uart_driver_install(UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to install modem UART driver");
        return ret;
    }

#ifdef MODEM_BYPASS_MODE
    // Configuration de l'UART alternatif (UART2) pour le mode bypass
    ESP_LOGI(TAG, "Configuring UART2 for bypass mode on pins RX:%d TX:%d", PC_RX_PIN, PC_TX_PIN);

    ret = uart_param_config(PC_UART_NUM, &uart_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure PC UART: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = uart_set_pin(PC_UART_NUM, PC_TX_PIN, PC_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set PC UART pins: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = uart_driver_install(PC_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to install PC UART driver: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "Starting in BYPASS MODE - UART2 <-> Modem direct communication");
    ESP_LOGI(TAG, "Connect your PC to UART2: RX=%d, TX=%d, GND", PC_RX_PIN, PC_TX_PIN);
    xTaskCreate(modem_bypass_task, "modem_bypass_task", 8 * 1024, NULL, 5, NULL);
#else
    ESP_LOGI(TAG, "Starting in NORMAL MODE");
    xTaskCreate(modem_task, "modem_task", 4096, NULL, 5, NULL);
#endif
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

esp_err_t get_gnss_data(gnss_data_t *data)
{
    char response[BUF_SIZE];
    static bool gnss_initialized = false;
    esp_err_t ret;

    if (!main_network_connected())
    {
        return ESP_ERR_INVALID_STATE;
    }

    gnss_data_t gnss_data;

    // Initialiser toutes les valeurs à 0
    memset(&gnss_data, 0, sizeof(gnss_data_t));

    if (!gnss_initialized)
    {
        // power on gnss
        ret = send_at_command_wait_response("AT+CGNSSPWR=1", "+CGNSSPWR: READY!", response, sizeof(response), 16000);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to power on GNSS: %s", response);
            return ret;
        }
        ESP_LOGI(TAG, "GNSS powered on");

        ret = send_at_command_wait_response("AT+CGNSSTST=1", "OK", response, sizeof(response), 2000);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to start GNSS nmea: %s", response);
            return ret;
        }
        ESP_LOGI(TAG, "GNSS NMEA started");

        ret = send_at_command_wait_response("AT+CGNSSPORTSWITCH=1,0", "OK", response, sizeof(response), 2000);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to switch GNSS port: %s", response);
            return ret;
        }
        ESP_LOGI(TAG, "GNSS port switched");

        ret = send_at_command_wait_response("AT+CAGPS", "+AGPS: success.", response, sizeof(response), 16000);
        if (ret != ESP_OK)
        {
            ESP_LOGW(TAG, "Failed to get AGPS: %s", response);
        }
        ESP_LOGI(TAG, "GNSS Assisted positioning started");
        gnss_initialized = true;
    }

    ret = send_at_command_wait_response("AT+CGNSSINFO", "+CGNSSINFO:", response, sizeof(response), 2000);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get GNSS info: %s", response);
        return ret;
    }

    // Parser les données de façon robuste, même si certains champs sont vides
    char *line_start = strstr(response, "+CGNSSINFO:");
    if (line_start)
    {
        // Sauter "+CGNSSINFO: "
        line_start += strlen("+CGNSSINFO: ");

        // Utiliser strtok pour parser chaque champ séparément
        char *token;
        char *line_copy = strdup(line_start);
        int field_count = 0;

        token = strtok(line_copy, ",");
        while (token != NULL && field_count < 16)
        {
            switch (field_count)
            {
            case 0:
                if (strlen(token) > 0)
                    gnss_data.mode = atoi(token);
                break;
            case 1:
                if (strlen(token) > 0)
                    gnss_data.gps_svs = atoi(token);
                break;
            case 2:
                if (strlen(token) > 0)
                    gnss_data.glonass_svs = atoi(token);
                break;
            case 3:
                if (strlen(token) > 0)
                    gnss_data.beidou_svs = atoi(token);
                break;
            case 4:
                if (strlen(token) > 0)
                    gnss_data.latitude = atof(token);
                break;
            case 5:
                if (strlen(token) > 0)
                    gnss_data.ns_indicator = token[0];
                break;
            case 6:
                if (strlen(token) > 0)
                    gnss_data.longitude = atof(token);
                break;
            case 7:
                if (strlen(token) > 0)
                    gnss_data.ew_indicator = token[0];
                break;
            case 8:
                if (strlen(token) > 0)
                    strncpy(gnss_data.date, token, sizeof(gnss_data.date) - 1);
                break;
            case 9:
                if (strlen(token) > 0)
                    strncpy(gnss_data.utc_time, token, sizeof(gnss_data.utc_time) - 1);
                break;
            case 10:
                if (strlen(token) > 0)
                    gnss_data.altitude = atof(token);
                break;
            case 11:
                if (strlen(token) > 0)
                    gnss_data.speed = atof(token);
                break;
            case 12:
                if (strlen(token) > 0)
                    gnss_data.course = atof(token);
                break;
            case 13:
                if (strlen(token) > 0)
                    gnss_data.pdop = atof(token);
                break;
            case 14:
                if (strlen(token) > 0)
                    gnss_data.hdop = atof(token);
                break;
            case 15:
                if (strlen(token) > 0)
                    gnss_data.vdop = atof(token);
                break;
            }
            token = strtok(NULL, ",");
            field_count++;
        }
        free(line_copy);
        ret = field_count; // Nombre de champs traités
    }
    else
    {
        ret = 0;
    }

    ESP_LOGI(TAG, "GNSS Parse result: %d fields parsed", ret);
    if (ret < 1)
    {
        ESP_LOGW(TAG, "No GNSS fields parsed successfully");
        ESP_LOGE(TAG, "Response: %s", response);
        return ESP_ERR_INVALID_STATE;
    }
    else
    {
        ESP_LOGI(TAG, "Mode: %d, GPS SVs: %d, GLONASS SVs: %d, BEIDOU SVs: %d",
                 gnss_data.mode, gnss_data.gps_svs, gnss_data.glonass_svs, gnss_data.beidou_svs);
        ESP_LOGI(TAG, "Lat: %.5f%c, Lon: %.5f%c", gnss_data.latitude, gnss_data.ns_indicator, gnss_data.longitude, gnss_data.ew_indicator);
        ESP_LOGI(TAG, "Date: %s, Time: %s", gnss_data.date, gnss_data.utc_time);
        ESP_LOGI(TAG, "Alt: %.2f, Speed: %.2f, Course: %.2f", gnss_data.altitude, gnss_data.speed, gnss_data.course);
        ESP_LOGI(TAG, "PDOP: %.2f, HDOP: %.2f, VDOP: %.2f", gnss_data.pdop, gnss_data.hdop, gnss_data.vdop);

        // Ne rapporter les données que si elles sont valides (non nulles)
        if (gnss_data.latitude != 0.0 && gnss_data.longitude != 0.0)
        {
            device_report_telemetry(DEVICE_GATEWAY_MAC, "latitude", gnss_data.latitude);
            device_report_telemetry(DEVICE_GATEWAY_MAC, "longitude", gnss_data.longitude);
        }
        if (gnss_data.altitude != 0.0)
        {
            device_report_telemetry(DEVICE_GATEWAY_MAC, "altitude", gnss_data.altitude);
        }
        if (gnss_data.speed != 0.0)
        {
            device_report_telemetry(DEVICE_GATEWAY_MAC, "speed", gnss_data.speed);
        }

        if (data)
        {
            *data = gnss_data;
        }
    }

    return ESP_OK;
}

void modem_task(void *pvParameters)
{
#ifdef MODEM_BYPASS_MODE
    // Ne fait rien en mode bypass, la tâche bypass prend le relais
    ESP_LOGI(TAG, "Modem task disabled in bypass mode");
    vTaskDelete(NULL);
#else
    int rssi, ber;
    TickType_t last_update = 0;
    bool first_update = true;
    while (1)
    {
        if ((xTaskGetTickCount() - last_update) >= pdMS_TO_TICKS(config.interval_modem * 1000) || first_update)
        {
            modem_update_telemetry(&rssi, &ber);
            last_update = xTaskGetTickCount();
            first_update = false;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
#endif
}

#ifdef MODEM_BYPASS_MODE
void modem_bypass_task(void *pvParameters)
{
    uint8_t pc_to_modem_buffer[BUF_SIZE];
    uint8_t modem_to_pc_buffer[BUF_SIZE];
    int pc_len, modem_len;

    ESP_LOGI(TAG, "Modem bypass task started - Bridging UART2 <-> Modem UART");
    ESP_LOGI(TAG, "Connect PC serial terminal to: RX=%d, TX=%d, Baud=115200", PC_RX_PIN, PC_TX_PIN);

    // Petite attente pour s'assurer que les UARTs sont prêts
    vTaskDelay(pdMS_TO_TICKS(100));

    while (1)
    {
        // Lire les données du PC (UART2) et les transférer vers le modem (UART1)
        pc_len = uart_read_bytes(PC_UART_NUM, pc_to_modem_buffer, BUF_SIZE - 1, 10 / portTICK_PERIOD_MS);
        if (pc_len > 0)
        {
            // Transférer les données du PC vers le modem
            uart_write_bytes(UART_NUM, pc_to_modem_buffer, pc_len);
            uart_flush(UART_NUM);

            // Log pour debug (optionnel)
            pc_to_modem_buffer[pc_len] = 0;
        }
        else if (pc_len < 0)
        {
            // ESP_LOGW(TAG, "PC UART read error: %d", pc_len);
        }

        // Lire les données du modem (UART1) et les transférer vers le PC (UART2)
        modem_len = uart_read_bytes(UART_NUM, modem_to_pc_buffer, BUF_SIZE - 1, 10 / portTICK_PERIOD_MS);
        if (modem_len > 0)
        {
            // Transférer les données du modem vers le PC
            uart_write_bytes(PC_UART_NUM, modem_to_pc_buffer, modem_len);
            uart_flush(PC_UART_NUM);

            // Log pour debug (optionnel)
            modem_to_pc_buffer[modem_len] = 0;
        }
        else if (modem_len < 0)
        {
            // ESP_LOGW(TAG, "Modem UART read error: %d", modem_len);
        }

        // Petite pause pour éviter une utilisation CPU excessive
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
#endif
