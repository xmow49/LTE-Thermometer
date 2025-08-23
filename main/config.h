#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
    typedef struct
    {
        uint32_t interval_modem;
        uint32_t interval_stats;
        uint32_t interval_battery;
        uint32_t interval_ping;
        uint32_t interval_send_to_tb;
        uint32_t interval_reboot;
        uint32_t interval_network_error_reboot;

        uint32_t time_same_timestamp;
        uint32_t interval_lcd_timeout;

        float sensor_temperature_change_threshold;
        float sensor_humidity_change_threshold;
        uint32_t sensor_interval_force_update;
        uint32_t keepalive_interval;

        uint32_t time_save_telemetry;
        bool lcd_notify;
    } config_t;

    typedef enum
    {
        CONFIG_ENTRY_TYPE_NONE = 0,
        CONFIG_ENTRY_TYPE_UINT32,
        CONFIG_ENTRY_TYPE_BOOL,
        CONFIG_ENTRY_TYPE_FLOAT,
    } config_entry_type_t;
    typedef struct
    {
        const char *name;
        config_entry_type_t type;
        void *value;
    } config_entry_t;

    extern config_t config;
    extern const config_entry_t config_entries[];
    extern const uint32_t config_entries_count;

    void config_init();
    void config_set_default();
    bool config_load();
    bool config_save();

#ifdef __cplusplus
}
#endif

#endif