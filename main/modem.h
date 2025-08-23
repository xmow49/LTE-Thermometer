

#ifndef MODEM_H
#define MODEM_H

// #define MODEM_BYPASS_MODE

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        int mode;
        int gps_svs;
        int glonass_svs;
        int beidou_svs;
        double latitude;
        double longitude;
        float altitude;
        float speed;
        float course;
        float pdop;
        float hdop;
        float vdop;
        char ns_indicator;
        char ew_indicator;
        char date[7];
        char utc_time[10];
    } gnss_data_t;

    void modem_task(void *pvParameters);
    esp_err_t send_sms(const char *phone_number, const char *message);
    esp_err_t init_at_modem();
    esp_err_t get_rssi(int *rssi, int *ber);
    esp_err_t modem_update_telemetry();
    esp_err_t get_gnss_data(gnss_data_t *data);
#ifdef MODEM_BYPASS_MODE
    void modem_bypass_task(void *pvParameters);
#endif

#ifdef __cplusplus
}
#endif

#endif // MODEM_H