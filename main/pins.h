#ifndef PINS_H
#define PINS_H

#include <driver/gpio.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define PIN_DHT GPIO_NUM_35
#define PIN_SCL GPIO_NUM_39
#define PIN_SDA GPIO_NUM_41
#define PIN_BUTTON GPIO_NUM_42

#define PIN_POWER GPIO_NUM_14
#define PIN_BATTERY_ADC ADC_CHANNEL_3
#ifdef __cplusplus
}
#endif

#endif // PINS_H
