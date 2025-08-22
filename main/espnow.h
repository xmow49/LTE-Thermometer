#ifndef ESPNOW_H
#define ESPNOW_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

    esp_err_t espnow_init(void);
    esp_err_t espnow_register_device(const uint8_t *mac_addr);

#ifdef __cplusplus
}
#endif

#endif // ESPNOW_H
