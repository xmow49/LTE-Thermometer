#include "button.h"
#include "pins.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "lcd.h"
#include "esp_log.h"
#include "sensors.hpp"
#include "thingsboard.hpp"
#include "modem.h"
#include "battery.h"
#include "main.h"

const char *TAG = "button";

void button_task(void *pvParameters)
{
    bool last_state = false;
    bool state = false;

    while (1)
    {
        state = !gpio_get_level(PIN_BUTTON);
        if (state != last_state)
        {
            last_state = state;
            if (state)
            {
                lcd_set_on(true);
                ESP_LOGI(TAG, "Button pressed");

                sensors_report_telemetry();
                modem_update_telemetry();
                battery_update_telemetry();
                main_report_telemetry();
                tb_force_update();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void button_init()
{
    // init button pullup
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << PIN_BUTTON,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
}

void button_start()
{
    xTaskCreate(button_task, "button_task", 4 * 1024, NULL, 5, NULL);
}

bool button_get_state()
{
    bool state = !gpio_get_level(PIN_BUTTON);
    return state;
}