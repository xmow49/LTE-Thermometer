#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

#define UPDATE_THINGSBOARD_INTERVAL_S 60 * 60
#define UPDATE_SENSORS_INTERVAL_S 10 * 60
#define UPDATE_PING_INTERVAL_S 60 * 60
#define UPDATE_MODEM_INTERVAL_S 10 * 60
#define UPDATE_MAIN_INTERVAL_S 10 * 60
#define UPDATE_BATTERY_INTERVAL_S 10 * 60
#define LCD_TIMEOUT_S 20
#define TIME_TO_CONSIDER_SAME_TIMESTAMP_S 3

    // debug
    // #define UPDATE_THINGSBOARD_INTERVAL_S 10
    // #define UPDATE_SENSORS_INTERVAL_S 4
    // #define UPDATE_PING_INTERVAL_S 6
    // #define UPDATE_MODEM_INTERVAL_S 1
    // #define UPDATE_MAIN_INTERVAL_S 1
    // #define UPDATE_BATTERY_INTERVAL_S 1
    // #define LCD_TIMEOUT_S 20
    // #define TIME_TO_CONSIDER_SAME_TIMESTAMP_S 3

#ifdef __cplusplus
}
#endif

#endif