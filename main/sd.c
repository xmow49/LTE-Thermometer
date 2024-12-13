#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "sd.h"
#include <stdio.h>
#include <stdlib.h>
#include "device.hpp"
#include "lcd.h"

#define TAG "SD"
#define CSV_DELIMITER ","

#define MOUNT_POINT "/sdcard"
sdmmc_card_t *card;
void remove_newlines(char *str)
{
    if (str == NULL)
        return;
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
    {
        str[len - 1] = '\0';
    }
    if (len > 0 && str[len - 1] == '\r')
    {
        str[len - 1] = '\0';
    }
}

esp_err_t sd_read_root_ca(char *out, size_t out_size)
{
    ESP_LOGI(TAG, "Reading root CA from file");
    FILE *f = fopen(MOUNT_POINT "/root_ca.pem", "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open root_ca.pem file");
        return ESP_FAIL;
    }

    size_t bytes_read = fread(out, 1, out_size - 1, f);
    fclose(f);

    if (bytes_read == 0)
    {
        ESP_LOGE(TAG, "Failed to read root CA file or file is empty");
        return ESP_FAIL;
    }

    out[bytes_read] = '\0'; // Null-terminate the string
    ESP_LOGI(TAG, "Successfully read root CA (%d bytes)", bytes_read);
    return ESP_OK;
}

esp_err_t sd_read_server_conf(char *server, int *port, char *client_id, char *username, char *password)
{
    ESP_LOGI(TAG, "Reading server and device configuration from file");
    FILE *f = fopen(MOUNT_POINT "/thingsboard.conf", "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open thingsboard.conf file");
        return ESP_FAIL;
    }

#define CONF_SERVER "server="
#define CONF_PORT "port="
#define CONF_CLIENT_ID "client_id="
#define CONF_USERNAME "username="
#define CONF_PASSWORD "password="
#define CONF_MAX_LENGTH 256

    char line[CONF_MAX_LENGTH];
    int port_value = 0;
    while (fgets(line, sizeof(line), f) != NULL)
    {
        // Remove newline character
        char *pos = strchr(line, '\n');
        if (pos)
        {
            *pos = '\0';
        }

        if (strncmp(line, CONF_SERVER, strlen(CONF_SERVER)) == 0)
        {
            strncpy(server, line + strlen(CONF_SERVER), CONF_MAX_LENGTH);
            server[CONF_MAX_LENGTH - 1] = '\0'; // Ensure null-termination
        }
        else if (strncmp(line, CONF_PORT, strlen(CONF_PORT)) == 0)
        {
            port_value = strtol(line + strlen(CONF_PORT), NULL, 10);
        }
        else if (strncmp(line, CONF_CLIENT_ID, strlen(CONF_CLIENT_ID)) == 0)
        {
            strncpy(client_id, line + strlen(CONF_CLIENT_ID), CONF_MAX_LENGTH);
            client_id[CONF_MAX_LENGTH - 1] = '\0';
        }
        else if (strncmp(line, CONF_USERNAME, strlen(CONF_USERNAME)) == 0)
        {
            strncpy(username, line + strlen(CONF_USERNAME), CONF_MAX_LENGTH);
            username[CONF_MAX_LENGTH - 1] = '\0';
        }
        else if (strncmp(line, CONF_PASSWORD, strlen(CONF_PASSWORD)) == 0)
        {
            strncpy(password, line + strlen(CONF_PASSWORD), CONF_MAX_LENGTH);
            password[CONF_MAX_LENGTH - 1] = '\0';
        }
    }

    fclose(f);

    // Remove any trailing whitespace
    remove_newlines(server);
    remove_newlines(client_id);
    remove_newlines(username);
    remove_newlines(password);

    if (strlen(server) == 0)
    {
        ESP_LOGE(TAG, "Failed to read server configuration");
        return ESP_FAIL;
    }
    if (port_value == 0)
    {
        ESP_LOGE(TAG, "Failed to read port configuration");
        return ESP_FAIL;
    }
    if (strlen(client_id) == 0)
    {
        ESP_LOGE(TAG, "Failed to read client ID configuration");
        ESP_LOG_BUFFER_HEXDUMP(TAG, client_id, 32, ESP_LOG_ERROR);
        return ESP_FAIL;
    }
    if (strlen(username) == 0)
    {
        ESP_LOGE(TAG, "Failed to read username configuration");
        return ESP_FAIL;
    }
    if (strlen(password) == 0)
    {
        ESP_LOGE(TAG, "Failed to read password configuration");
        return ESP_FAIL;
    }

    *port = port_value;
    ESP_LOGI(TAG, "Successfully read server and device configuration");
    return ESP_OK;
}

esp_err_t sd_read_devices()
{
    ESP_LOGI(TAG, "Reading devices from CSV file");
    FILE *f = fopen(MOUNT_POINT "/devices.csv", "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open devices.csv file");
        return ESP_FAIL;
    }

    char line[512]; // Buffer to store each line
    int count = 0;

    // Skip the header line
    if (fgets(line, sizeof(line), f) == NULL)
    {
        ESP_LOGE(TAG, "Failed to read header from CSV file");
        fclose(f);
        return ESP_FAIL;
    }

    while (fgets(line, sizeof(line), f) != NULL)
    {
        // Remove newline character
        char *pos = strchr(line, '\n');
        if (pos)
        {
            *pos = '\0';
        }

        // Parse CSV line
        char *token;
        int field = 0;
        char *name = NULL;
        char *mac = NULL;

        token = strtok(line, CSV_DELIMITER);
        while (token != NULL && field < 4)
        {
            switch (field)
            {
            case 0:
                name = strdup(token);
                remove_newlines(name);
                break;
            case 1:
                mac = strdup(token);
                remove_newlines(mac);
                break;
            }
            token = strtok(NULL, CSV_DELIMITER);
            field++;
        }

        // Add the device using the device_add function
        if (name && mac)
        {
            device_add(name, mac);
            count++;
        }
        else
        {
            ESP_LOGE(TAG, "Failed to allocate memory for device fields");
        }

        // Free the allocated memory
        free(name);
        free(mac);
    }

    fclose(f);

    ESP_LOGI(TAG, "Successfully read %d devices from CSV file", count);
    return ESP_OK;
}

void sd_init(void)
{
    esp_err_t ret;

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");
    ESP_LOGI(TAG, "Using SDMMC peripheral");

    // By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
    // For setting a specific frequency, use host.max_freq_khz (range 400kHz - 40MHz for SDMMC)
    // Example: for fixed frequency of 10MHz, use host.max_freq_khz = 10000;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = 1;

    // On chips where the GPIOs used for SD card can be configured, set them in
    // the slot_config structure:
#ifdef CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
    slot_config.clk = 5;
    slot_config.cmd = 4;
    slot_config.d0 = 6;
#endif // CONFIG_SOC_SDMMC_USE_GPIO_MATRIX

    // Enable internal pullups on enabled pins. The internal pullups
    // are insufficient however, please make sure 10k external pullups are
    // connected on the bus. This is for debug / example purpose only.
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                          "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
            lcd_setup_msg("Erreur SD", "Echec montage");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                          "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));
            lcd_setup_msg("Erreur SD", "Echec init");
        }
        return;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);
}

void sd_deinit(void)
{
    // All done, unmount partition and disable SDMMC peripheral
    esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
    ESP_LOGI(TAG, "Card unmounted");
}
