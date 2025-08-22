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
#include "zlib.h"

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
        ESP_LOGI(TAG, "Coredump: empty");
        esp_core_dump_image_erase(); // Nettoyer le coredump corrompu
        break;

    case ESP_ERR_INVALID_CRC:
        ESP_LOGI(TAG, "Coredump: invalid checksum");
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

    esp_err_t err = esp_core_dump_image_get(&out_addr, &out_size);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Crash dump reading failed: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "Coredump found: %d bytes at address 0x%x", out_size, out_addr);

    const esp_partition_t *coredump_partition = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_COREDUMP,
        NULL);

    if (coredump_partition == NULL)
    {
        ESP_LOGE(TAG, "Coredump partition not found");
        return;
    }

    uint32_t buffer_size = out_size + 128;
    char *coredump_data = (char *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
    if (coredump_data == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for coredump data");
        return;
    }

    err = esp_partition_read(coredump_partition, 0, coredump_data, out_size);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read coredump data: %s", esp_err_to_name(err));
        free(coredump_data);
        return;
    }

    ESP_LOGI(TAG, "Coredump data read successfully");

    // Compress the coredump data using zlib
    uLong compressed_size = compressBound(out_size);
    char *compressed_data = (char *)heap_caps_malloc(compressed_size, MALLOC_CAP_SPIRAM);
    if (compressed_data == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for compressed data");
        free(coredump_data);
        return;
    }

    int compress_result = compress((Bytef *)compressed_data, &compressed_size, (const Bytef *)coredump_data, out_size);
    if (compress_result != Z_OK)
    {
        ESP_LOGE(TAG, "Failed to compress coredump data: %d", compress_result);
        free(coredump_data);
        free(compressed_data);
        return;
    }

    ESP_LOGI(TAG, "Coredump compressed: %lu bytes -> %lu bytes (%.1f%% reduction)",
             out_size, compressed_size, 100.0 * (1.0 - (double)compressed_size / out_size));

    // Free original data as we now have compressed version
    free(coredump_data);

    size_t required_size;
    int ret = mbedtls_base64_encode(NULL, 0, &required_size, (unsigned char *)compressed_data, compressed_size);
    if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL)
    {
        ESP_LOGE("BASE64", "Error: can't calculate buffer size: %d", ret);
        free(compressed_data);
        return;
    }

    char *coredump_base64 = (char *)heap_caps_malloc(required_size + 64, MALLOC_CAP_SPIRAM);
    if (coredump_base64 == NULL)
    {
        ESP_LOGE("BASE64", "Error: can't allocate memory for base64 buffer");
        free(compressed_data);
        return;
    }

    const char *json_start = "{\"coredump\": \"";
    const uint32_t json_start_len = strlen(json_start);
    memcpy(coredump_base64, json_start, json_start_len);

    size_t output_len;
    ret = mbedtls_base64_encode((unsigned char *)coredump_base64 + json_start_len, required_size, &output_len, (unsigned char *)compressed_data, compressed_size);
    if (ret != 0)
    {
        ESP_LOGE("BASE64", "Error: can't encode to base64: %d", ret);
        free(compressed_data);
        free(coredump_base64);
        return;
    }

    const char *json_end = "\"}";
    const uint32_t json_end_len = strlen(json_end);
    memcpy(coredump_base64 + json_start_len + output_len, json_end, json_end_len);

    coredump_base64[json_start_len + output_len + json_end_len] = '\0';

    device_gateway_send_json_telemetry(coredump_base64);
    free(compressed_data);
    free(coredump_base64);
    esp_core_dump_image_erase();
}
