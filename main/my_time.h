#ifndef MY_TIME_H
#define MY_TIME_H

#include "esp_err.h"
#ifdef __cplusplus
extern "C"
{
#endif

    esp_err_t my_time_update();
    bool my_time_is_set();
#ifdef __cplusplus
}
#endif

#endif // MY_TIME_H
