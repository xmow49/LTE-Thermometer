#ifndef SD_H
#define SD_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

    void sd_init(void);
    esp_err_t sd_read_devices();
    esp_err_t sd_read_root_ca(char *out, size_t out_size);
    esp_err_t sd_read_server_conf(char *server, int *port, char *client_id, char *username, char *password);

#define MAX_DEVICES 10

#ifdef __cplusplus
}
#endif

#endif // SD_H
