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
#include <map>
#include <vector>
#include <string>
#include <dirent.h>

#define TAG "SD"
#define CSV_DELIMITER ","

#define MOUNT_POINT "/sdcard"
#define TELEMETRY_DIR "/sdcard/telemetry"
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
        .allocation_unit_size = 16 * 1024,
        .disk_status_check_enable = false,
        .use_one_fat = false,
    };
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
    slot_config.clk = (gpio_num_t)5;
    slot_config.cmd = (gpio_num_t)4;
    slot_config.d0 = (gpio_num_t)6;
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

// Fonction pour créer le dossier telemetry s'il n'existe pas
void ensure_telemetry_dir(void)
{
    struct stat st;
    memset(&st, 0, sizeof(st));
    if (stat(TELEMETRY_DIR, &st) == -1)
    {
        if (mkdir(TELEMETRY_DIR, 0700) != 0)
        {
            ESP_LOGE(TAG, "Failed to create telemetry directory");
        }
        else
        {
            ESP_LOGI(TAG, "Created telemetry directory");
        }
    }
}

// Fonction pour sauvegarder une seule mesure de télémétrie
esp_err_t sd_save_telemetry(const char *name, float value)
{
    if (!name)
    {
        ESP_LOGE(TAG, "Invalid name parameter");
        return ESP_ERR_INVALID_ARG;
    }

    ensure_telemetry_dir();

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s.csv", TELEMETRY_DIR, name);

    FILE *f = fopen(filepath, "a"); // Mode append
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file %s", filepath);
        return ESP_FAIL;
    }

    // Vérifier si le fichier est vide pour ajouter l'en-tête
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);

    if (file_size == 0)
    {
        // Fichier vide, ajouter l'en-tête
        fprintf(f, "timestamp,value\n");
    }

    // Obtenir le timestamp actuel
    time_t now = time(NULL);
    fprintf(f, "%lld,%.2f\n", (long long)now, value);

    fflush(f);        // Forcer l'écriture du buffer
    fsync(fileno(f)); // Synchroniser avec le disque
    fclose(f);
    ESP_LOGI(TAG, "Saved telemetry: %s = %.2f (ts: %lld)", name, value, (long long)now);
    return ESP_OK;
}

// Fonction pour sauvegarder un vecteur de TelemetryReport
esp_err_t sd_save_telemetrys(const std::vector<TelemetryReport> &telemetry_reports)
{
    if (telemetry_reports.empty())
    {
        ESP_LOGW(TAG, "No telemetry data to save");
        return ESP_OK;
    }

    ensure_telemetry_dir();

    // Grouper les données par nom pour optimiser l'écriture
    std::map<std::string, std::vector<const TelemetryReport *>> grouped_data;

    for (const auto &report : telemetry_reports)
    {
        grouped_data[report.name].push_back(&report);
    }

    // Sauvegarder chaque groupe dans son fichier respectif
    for (const auto &pair : grouped_data)
    {
        const std::string &name = pair.first;
        const std::vector<const TelemetryReport *> &reports = pair.second;

        char filepath[256];
        snprintf(filepath, sizeof(filepath), "%s/%s.csv", TELEMETRY_DIR, name.c_str());

        FILE *f = fopen(filepath, "a"); // Mode append
        if (f == NULL)
        {
            ESP_LOGE(TAG, "Failed to open file %s", filepath);
            continue;
        }

        // Vérifier si le fichier est vide pour ajouter l'en-tête
        fseek(f, 0, SEEK_END);
        long file_size = ftell(f);

        if (file_size == 0)
        {
            // Fichier vide, ajouter l'en-tête
            fprintf(f, "timestamp,value\n");
        }

        // Écrire toutes les données pour ce nom
        for (const auto *report : reports)
        {
            fprintf(f, "%lld,%.2f\n", (long long)report->timestamp, report->value);
        }

        fflush(f);        // Forcer l'écriture du buffer
        fsync(fileno(f)); // Synchroniser avec le disque
        fclose(f);
        ESP_LOGI(TAG, "Saved %d telemetry entries for %s", reports.size(), name.c_str());
    }

    return ESP_OK;
}

// Fonction pour lire les données de télémétrie et les convertir en vecteur
std::vector<TelemetryReport> sd_read_telemetry(const char *name, bool clear_after_read)
{
    std::vector<TelemetryReport> telemetry_list;

    if (!name)
    {
        ESP_LOGE(TAG, "Invalid name parameter");
        return telemetry_list;
    }

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s.csv", TELEMETRY_DIR, name);

    FILE *f = fopen(filepath, "r");
    if (f == NULL)
    {
        ESP_LOGW(TAG, "File %s does not exist", filepath);
        return telemetry_list;
    }

    char line[128];
    bool header_skipped = false;
    int count = 0;

    while (fgets(line, sizeof(line), f) != NULL)
    {
        // Supprimer le caractère de nouvelle ligne
        char *pos = strchr(line, '\n');
        if (pos)
        {
            *pos = '\0';
        }

        // Ignorer la ligne d'en-tête
        if (!header_skipped)
        {
            header_skipped = true;
            continue;
        }

        // Parser la ligne CSV: timestamp,value
        char *timestamp_str = strtok(line, ",");
        char *value_str = strtok(NULL, ",");

        if (timestamp_str && value_str)
        {
            time_t timestamp = strtoll(timestamp_str, NULL, 10);
            float value = strtof(value_str, NULL);

            // Créer un TelemetryReport
            TelemetryReport report(name, value);
            report.timestamp = timestamp; // Override le timestamp auto-généré

            telemetry_list.push_back(report);
            count++;
        }
    }

    fclose(f);

    if (clear_after_read && count > 0)
    {
        // Supprimer le fichier après lecture
        if (remove(filepath) == 0)
        {
            ESP_LOGI(TAG, "Cleared file %s after reading %d entries", filepath, count);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to clear file %s", filepath);
        }
    }
    else
    {
        ESP_LOGI(TAG, "Read %d telemetry entries from %s", count, filepath);
    }

    return telemetry_list;
}

// Fonction pour lire toutes les données de télémétrie d'un dossier
std::vector<TelemetryReport> sd_read_all_telemetry(bool clear_after_read)
{
    std::vector<TelemetryReport> all_telemetry;

    DIR *dir = opendir(TELEMETRY_DIR);
    if (dir == NULL)
    {
        ESP_LOGW(TAG, "Telemetry directory does not exist");
        return all_telemetry;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        // Vérifier que c'est un fichier .csv
        char *ext = strrchr(entry->d_name, '.');
        if (ext && strcmp(ext, ".csv") == 0)
        {
            // Extraire le nom sans l'extension
            std::string name = entry->d_name;
            name = name.substr(0, name.find_last_of("."));

            // Lire les données de ce fichier
            auto telemetry_data = sd_read_telemetry(name.c_str(), clear_after_read);

            // Ajouter à la liste principale
            all_telemetry.insert(all_telemetry.end(), telemetry_data.begin(), telemetry_data.end());
        }
    }

    closedir(dir);
    ESP_LOGI(TAG, "Read total of %d telemetry entries from all files", all_telemetry.size());
    return all_telemetry;
}

// Fonction d'exemple pour tester les fonctionnalités
void sd_test_telemetry_functions(void)
{
    ESP_LOGI(TAG, "Testing telemetry functions...");

    // Test 1: Sauvegarder des données individuelles
    sd_save_telemetry("temperature", 23.5);
    sd_save_telemetry("humidity", 65.2);
    sd_save_telemetry("temperature", 24.1);

    // Test 2: Créer un vecteur de TelemetryReport et le sauvegarder
    std::vector<TelemetryReport> test_data;
    test_data.emplace_back("pressure", 1013.25);
    test_data.emplace_back("pressure", 1012.8);
    test_data.emplace_back("voltage", 3.7);

    sd_save_telemetrys(test_data);

    // Test 3: Lire les données
    auto temp_data = sd_read_telemetry("temperature", false);
    ESP_LOGI(TAG, "Read %d temperature entries", temp_data.size());

    auto all_data = sd_read_all_telemetry(false);
    ESP_LOGI(TAG, "Read total %d entries from all files", all_data.size());

    ESP_LOGI(TAG, "Telemetry functions test completed");
}

// Fonction de debug pour lister le contenu du dossier telemetry
void sd_list_telemetry_files(void)
{
    ESP_LOGI(TAG, "Listing telemetry directory contents...");

    DIR *dir = opendir(TELEMETRY_DIR);
    if (dir == NULL)
    {
        ESP_LOGE(TAG, "Failed to open telemetry directory");
        return;
    }

    struct dirent *entry;
    int file_count = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG) // Regular file
        {
            char filepath[512];
            snprintf(filepath, sizeof(filepath), "%s/%s", TELEMETRY_DIR, entry->d_name);

            FILE *f = fopen(filepath, "r");
            if (f != NULL)
            {
                fseek(f, 0, SEEK_END);
                long file_size = ftell(f);
                fclose(f);
                ESP_LOGI(TAG, "File: %s (size: %ld bytes)", entry->d_name, file_size);
                file_count++;
            }
        }
    }

    closedir(dir);
    ESP_LOGI(TAG, "Found %d files in telemetry directory", file_count);
}
