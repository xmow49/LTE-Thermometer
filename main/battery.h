#ifndef BATTERY_H
#define BATTERY_H

#ifdef __cplusplus
extern "C"
{
#endif

    void battery_init();
    esp_err_t battery_update_telemetry();

#ifdef __cplusplus
}
#endif

#endif // BATTERY_H
