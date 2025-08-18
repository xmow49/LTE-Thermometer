#ifndef BUTTON_H
#define BUTTON_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdbool.h>
    void button_init();
    void button_start();
    bool button_get_state();

#ifdef __cplusplus
}
#endif

#endif // BUTTON_H
