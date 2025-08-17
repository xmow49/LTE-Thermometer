#ifndef SD_H
#define SD_H

#include "esp_err.h"

#ifdef __cplusplus

#include <vector>

// Forward declaration for C++ only
class TelemetryReport;

extern "C"
{
#endif

#include <stdbool.h>

    void sd_init(void);
    void sd_deinit(void);
    esp_err_t sd_read_devices();
    esp_err_t sd_read_root_ca(char *out, size_t out_size);
    esp_err_t sd_read_server_conf(char *server, int *port, char *client_id, char *username, char *password);

    // Fonctions de télémétrie
    esp_err_t sd_save_telemetry(const char *name, float value);

#define MAX_DEVICES 10

#ifdef __cplusplus
}

// Fonctions C++ only
esp_err_t sd_save_telemetrys(const std::vector<TelemetryReport> &telemetry_reports);
std::vector<TelemetryReport> sd_read_telemetry(const char *name, bool clear_after_read = false);
std::vector<TelemetryReport> sd_read_all_telemetry(bool clear_after_read = false);
void sd_test_telemetry_functions(void);
void sd_list_telemetry_files(void);

#endif

#endif // SD_H
