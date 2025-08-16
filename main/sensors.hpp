#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "esp_err.h"
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    typedef struct
    {
        float humidity;
        float temperature;
    } dht_data_t;

    esp_err_t sensors_read_dht22(dht_data_t *data);
    void sensors_task(void *pvParameters);
    bool sensors_report_telemetry();
#ifdef __cplusplus
}
#endif

#endif // DHT_SENSOR_H