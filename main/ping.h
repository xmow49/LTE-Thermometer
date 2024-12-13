#ifndef PING_H
#define PING_H

#ifdef __cplusplus
extern "C"
{
#endif

    // Initialize the ping functionality
    void ping_init(void);

    // Task function for periodic pinging
    int ping_task(void *pvParameters);

    // Get the latest ping value
    int ping_get_value(void);

#ifdef __cplusplus
}
#endif

#endif // PING_H
