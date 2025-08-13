#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "ping/ping_sock.h"
#include "esp_log.h"
#include "device.hpp"
#include "main.h"
#include "config.h"
#include "esp_core_dump.h"
#include "esp_partition.h"
#include "mbedtls/base64.h"

#define TAG "crash"
void process_valid_coredump(void);

extern "C" void crash_check_dump()
{
    esp_err_t check_result = esp_core_dump_image_check();
    switch (check_result)
    {
    case ESP_OK:
        ESP_LOGI(TAG, "Coredump detected");
        process_valid_coredump();
        break;

    case ESP_ERR_NOT_FOUND:
        ESP_LOGI(TAG, "No coredump found");
        break;

    case ESP_ERR_INVALID_SIZE:
        ESP_LOGE(TAG, "Coredump corrupted: invalid size");
        esp_core_dump_image_erase(); // Nettoyer le coredump corrompu
        break;

    case ESP_ERR_INVALID_CRC:
        ESP_LOGE(TAG, "Coredump corrupted: invalid checksum");
        esp_core_dump_image_erase(); // Nettoyer le coredump corrompu
        break;

    default:
        ESP_LOGE(TAG, "Flash access error: 0x%x", check_result);
        break;
    }
}

void process_valid_coredump(void)
{
    size_t out_addr, out_size;

    // 1. Obtenir l'adresse et la taille du coredump
    esp_err_t err = esp_core_dump_image_get(&out_addr, &out_size);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Crash dump reading failed: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "Coredump found: %d bytes at address 0x%x", out_size, out_addr);

    // // 2. Obtenir le résumé du crash
    // esp_core_dump_summary_t summary;
    // if (esp_core_dump_get_summary(&summary) == ESP_OK)
    // {
    //     ESP_LOGI(TAG, "Task crashed: %s", summary.exc_task);
    //     ESP_LOGI(TAG, "PC: 0x%x", summary.exc_pc);
    //     ESP_LOGI(TAG, "Version: %d", summary.core_dump_version);
    // }

    // 3. Lire les données depuis la partition flash
    const esp_partition_t *coredump_partition = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_COREDUMP,
        NULL);

    if (coredump_partition == NULL)
    {
        ESP_LOGE(TAG, "Coredump partition not found");
        return;
    }

    // Allouer buffer pour les données
    uint32_t buffer_size = out_size + 128;
    char *coredump_data = (char *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
    if (coredump_data == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for coredump data");
        return;
    }

    // Lire depuis la partition
    err = esp_partition_read(coredump_partition, 0, coredump_data, out_size);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "Coredump data read successfully");
    }

    size_t required_size;
    int ret = mbedtls_base64_encode(NULL, 0, &required_size, (unsigned char *)coredump_data, out_size);
    if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL)
    {
        ESP_LOGE("BASE64", "Error: can't calculate buffer size: %d", ret);
        free(coredump_data);
        return;
    }

    char *coredump_base64 = (char *)heap_caps_malloc(required_size + 64, MALLOC_CAP_SPIRAM);
    if (coredump_base64 == NULL)
    {
        ESP_LOGE("BASE64", "Error: can't allocate memory for base64 buffer");
        free(coredump_data);

        return;
    }

    const char *json_start = "{\"coredump\": \"";
    const uint32_t json_start_len = strlen(json_start);
    memcpy(coredump_base64, json_start, json_start_len);

    size_t output_len;
    ret = mbedtls_base64_encode((unsigned char *)coredump_base64 + json_start_len, required_size, &output_len, (unsigned char *)coredump_data, out_size);
    if (ret != 0)
    {
        ESP_LOGE("BASE64", "Error: can't encode to base64: %d", ret);
        free(coredump_data);
        free(coredump_base64);
        return;
    }

    const char *json_end = "\"}";
    const uint32_t json_end_len = strlen(json_end);
    memcpy(coredump_base64 + json_start_len + output_len, json_end, json_end_len);

    coredump_base64[json_start_len + output_len + json_end_len] = '\0';

    // print 64 char per line
    // for (size_t i = 0; i < json_start_len + output_len + json_end_len; i += 64)
    // {
    //     printf("%.*s\n", 64, coredump_base64 + i);
    // }

    device_gateway_send_json_telemetry(coredump_base64);
    free(coredump_data);
    free(coredump_base64);
    esp_core_dump_image_erase();
}
