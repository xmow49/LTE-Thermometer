

#ifndef MODEM_H
#define MODEM_H
#ifdef __cplusplus
extern "C"
{
#endif
    void modem_task(void *pvParameters);
    esp_err_t send_sms(const char *phone_number, const char *message);
    esp_err_t init_at_modem();
    esp_err_t get_rssi(int *rssi, int *ber);
    esp_err_t modem_update_telemetry();
#ifdef __cplusplus
}
#endif

#endif // MODEM_H