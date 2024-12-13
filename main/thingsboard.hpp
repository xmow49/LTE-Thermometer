// tb.h
#ifndef TB_H
#define TB_H

#ifdef __cplusplus
extern "C"
{
#endif
#define MAX_MESSAGE_SIZE 2048U

    void tb_init();
    void tb_task(void *pvParameters);
    bool sendTelemetryJson(char *topic, char *json);
    void tb_set_network_connected(bool connected);
    void tb_force_update();
#ifdef __cplusplus
}
#endif

#endif // TB_H