/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"
#include "pins.h"
#include "time.h"
#include "lcd.h"
#include "bitmap.h"
#include "config.h"

extern lv_font_t freesans_18;
extern lv_font_t freesans_16;
extern lv_img_dsc_t battery_full;

extern lv_img_dsc_t signal_0;
extern lv_img_dsc_t signal_1;
extern lv_img_dsc_t signal_2;
extern lv_img_dsc_t signal_3;
extern lv_img_dsc_t signal_4;
extern lv_img_dsc_t signal_full;

extern lv_img_dsc_t battery_0;
extern lv_img_dsc_t battery_25;
extern lv_img_dsc_t battery_50;
extern lv_img_dsc_t battery_75;
extern lv_img_dsc_t battery_full;

extern lv_img_dsc_t plug;

#if CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
#include "esp_lcd_sh1107.h"
#else
#include "esp_lcd_panel_vendor.h"
#endif
void lcd_task(void *pvParameters);

static const char *TAG = "example";

float lcd_temperature = 0;
float lcd_humidity = 0;
int lcd_signal_rssi = 0;
int lcd_battery_level = 0;
bool lcd_plugged = false;

bool lcd_on = false;
bool lcd_initialized = false;
bool lcd_booted = false;

TickType_t lcd_last_on = 0;
#define I2C_BUS_PORT 0
void example_lvgl_demo_ui(lv_disp_t *disp)
{
    lv_obj_t *scr = lv_disp_get_scr_act(disp);
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(label, "Hello Espressif, Hello LVGL.");
    /* Size of the screen (if you use rotation 90 or 270, please set disp->driver->ver_res) */
    lv_obj_set_width(label, disp->driver->hor_res);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ (400 * 1000)
#define EXAMPLE_PIN_NUM_RST -1
#define EXAMPLE_I2C_HW_ADDR 0x3C

// The pixel number in horizontal and vertical
#if CONFIG_EXAMPLE_LCD_CONTROLLER_SSD1306
#define EXAMPLE_LCD_H_RES 128
#define EXAMPLE_LCD_V_RES CONFIG_EXAMPLE_SSD1306_HEIGHT
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
#define EXAMPLE_LCD_H_RES 64
#define EXAMPLE_LCD_V_RES 128
#endif
// Bit number used to represent command and parameter
#define EXAMPLE_LCD_CMD_BITS 8
#define EXAMPLE_LCD_PARAM_BITS 8

extern void example_lvgl_demo_ui(lv_disp_t *disp);
i2c_master_bus_handle_t i2c_bus = NULL;

void lcd_init(void)
{
    ESP_LOGI(TAG, "Initialize I2C bus");

    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_BUS_PORT,
        .sda_io_num = PIN_SDA,
        .scl_io_num = PIN_SCL,
        .flags.enable_internal_pullup = false,
    };
    esp_err_t ret = i2c_new_master_bus(&bus_config, &i2c_bus);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize I2C bus: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = EXAMPLE_I2C_HW_ADDR,
        .scl_speed_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .control_phase_bytes = 1,               // According to SSD1306 datasheet
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,   // According to SSD1306 datasheet
        .lcd_param_bits = EXAMPLE_LCD_CMD_BITS, // According to SSD1306 datasheet
#if CONFIG_EXAMPLE_LCD_CONTROLLER_SSD1306
        .dc_bit_offset = 6, // According to SSD1306 datasheet
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
        .dc_bit_offset = 0, // According to SH1107 datasheet
        .flags =
            {
                .disable_control_phase = 1,
            }
#endif
    };
    ret = esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to create panel IO: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "Install SSD1306 panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
    };
#if CONFIG_EXAMPLE_LCD_CONTROLLER_SSD1306
    esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = EXAMPLE_LCD_V_RES,
    };
    panel_config.vendor_config = &ssd1306_config;
    ret = esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle);
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
    ret = esp_lcd_new_panel_sh1107(io_handle, &panel_config, &panel_handle);
#endif
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to create LCD panel: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_lcd_panel_reset(panel_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to reset panel: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_lcd_panel_init(panel_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize panel: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_lcd_panel_disp_on_off(panel_handle, true);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to turn on display: %s", esp_err_to_name(ret));
        return;
    }

#if CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
    ret = esp_lcd_panel_invert_color(panel_handle, true);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to invert panel color: %s", esp_err_to_name(ret));
        return;
    }
#endif

    ESP_LOGI(TAG, "Initialize LVGL");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES,
        .double_buffer = true,
        .hres = EXAMPLE_LCD_H_RES,
        .vres = EXAMPLE_LCD_V_RES,
        .monochrome = true,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        }};
    lv_disp_t *disp = lvgl_port_add_disp(&disp_cfg);

    /* Rotation of the screen */
    lv_disp_set_rotation(disp, LV_DISP_ROT_180);

    lcd_initialized = true;

    // get boot reason
    esp_reset_reason_t reason = esp_reset_reason();
    // Print the reset reason
    const char *reason_str;
    switch (reason)
    {
    case ESP_RST_UNKNOWN:
        reason_str = "Unknown";
        break;
    case ESP_RST_POWERON:
        reason_str = "Power-on";
        break;
    case ESP_RST_EXT:
        reason_str = "External pin";
        break;
    case ESP_RST_SW:
        reason_str = "Software";
        break;
    case ESP_RST_PANIC:
        reason_str = "Exception/Panic";
        break;
    case ESP_RST_INT_WDT:
        reason_str = "Interrupt watchdog";
        break;
    case ESP_RST_TASK_WDT:
        reason_str = "Task watchdog";
        break;
    case ESP_RST_WDT:
        reason_str = "Other watchdog";
        break;
    case ESP_RST_DEEPSLEEP:
        reason_str = "Deep sleep";
        break;
    case ESP_RST_BROWNOUT:
        reason_str = "Brownout";
        break;
    case ESP_RST_SDIO:
        reason_str = "SDIO";
        break;
    case ESP_RST_USB:
        reason_str = "USB";
        break;
    default:
        reason_str = "Other";
        break;
    }
    ESP_LOGI(TAG, "Reset reason: %s", reason_str);

    bool turn_on = false;
    switch (reason)
    {
    case ESP_RST_POWERON:
    case ESP_RST_UNKNOWN:
    case ESP_RST_SW:
    case ESP_RST_USB:
        turn_on = true;
        break;

    default:
        break;
    }
    lcd_set_on(turn_on);

    lcd_setup_msg("Démarrage", NULL);
}

void lcd_setup_msg(const char *title, const char *msg)
{
    if (!lcd_initialized)
    {
        ESP_LOGI(TAG, "LCD not initialized");
        return;
    }
    if (lvgl_port_lock(0))
    {
        lv_obj_t *scr = lv_scr_act();
        lv_obj_clean(scr);

        lv_obj_t *title_label = lv_label_create(scr);
        lv_label_set_text(title_label, title);
        lv_obj_set_style_text_font(title_label, &freesans_18, 0);

        if (msg == NULL)
        {
            lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);
        }
        else
        {
            lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);

            lv_obj_t *msg_label = lv_label_create(scr);
            lv_label_set_text(msg_label, msg);
            lv_obj_set_style_text_font(msg_label, &freesans_16, 0);
            lv_obj_align(msg_label, LV_ALIGN_CENTER, 0, 10);
        }

        lvgl_port_unlock();
    }
}

void lcd_set_temperature(float temperature, float humidity)
{
    lcd_temperature = temperature;
    lcd_humidity = humidity;
    if (lcd_booted)
    {
        lcd_update();
    }
}

void lcd_set_battery_level(int battery_level)
{
    lcd_battery_level = battery_level;
    if (lcd_booted)
    {
        lcd_update();
    }
}

void lcd_set_signal_rssi(int signal_rssi)
{
    lcd_signal_rssi = signal_rssi;
    if (lcd_booted)
    {
        lcd_update();
    }
}

void lcd_update(void)
{
    if (!lcd_initialized)
    {
        return;
    }
    if (lvgl_port_lock(0))
    {
        lv_obj_t *scr = lv_disp_get_scr_act(NULL);

        // Clear the screen
        lv_obj_clean(scr);
        if (!lcd_on)
        {
            lvgl_port_unlock();
            return;
        }
        // Temperature
        lv_obj_t *temp_label = lv_label_create(scr);
        lv_obj_set_style_text_font(temp_label, &freesans_18, 0);
        char temp_str[16];
        snprintf(temp_str, sizeof(temp_str), "%.1f°C", lcd_temperature);
        lv_label_set_text(temp_label, temp_str);
        lv_obj_align(temp_label, LV_ALIGN_TOP_LEFT, 0, 0);

        // Humidity
        lv_obj_t *humid_label = lv_label_create(scr);
        lv_obj_set_style_text_font(humid_label, &freesans_18, 0);
        char humid_str[16];
        snprintf(humid_str, sizeof(humid_str), "%.1f %%", lcd_humidity);
        lv_label_set_text(humid_label, humid_str);
        lv_obj_align(humid_label, LV_ALIGN_TOP_LEFT, 0, 20);

        // Battery level
        const lv_img_dsc_t *battery_img_dsc;
        if (lcd_battery_level > 75)
            battery_img_dsc = &battery_full;
        else if (lcd_battery_level > 50)
            battery_img_dsc = &battery_75;
        else if (lcd_battery_level > 25)
            battery_img_dsc = &battery_50;
        else if (lcd_battery_level > 0)
            battery_img_dsc = &battery_25;
        else
            battery_img_dsc = &battery_0;

        lv_obj_t *battery_img = lv_img_create(scr);
        lv_img_set_src(battery_img, battery_img_dsc);
        lv_obj_align(battery_img, LV_ALIGN_TOP_RIGHT, 0, 0);

        // Plugged
        if (lcd_plugged)
        {
            lv_obj_t *plug_img = lv_img_create(scr);
            lv_img_set_src(plug_img, &plug);
            lv_obj_align(plug_img, LV_ALIGN_TOP_RIGHT, -40, 0);
        }

        // Signal strength
        const lv_img_dsc_t *signal_img_dsc;
        if (lcd_signal_rssi > -50)
            signal_img_dsc = &signal_full;
        else if (lcd_signal_rssi > -60)
            signal_img_dsc = &signal_4;
        else if (lcd_signal_rssi > -70)
            signal_img_dsc = &signal_3;
        else if (lcd_signal_rssi > -80)
            signal_img_dsc = &signal_2;
        else if (lcd_signal_rssi > -90)
            signal_img_dsc = &signal_1;
        else
            signal_img_dsc = &signal_0;

        lv_obj_t *signal_img = lv_img_create(scr);
        lv_img_set_src(signal_img, signal_img_dsc);
        lv_obj_align(signal_img, LV_ALIGN_TOP_RIGHT, 0, 20);

        // Date and Time
        lv_obj_t *datetime_label = lv_label_create(scr);
        lv_obj_set_style_text_font(datetime_label, &freesans_16, 0);
        char datetime_str[32];
        time_t now;
        struct tm timeinfo;
        time(&now);
        localtime_r(&now, &timeinfo);
        strftime(datetime_str, sizeof(datetime_str), "%d/%m/%Y %H:%M", &timeinfo);
        lv_label_set_text(datetime_label, datetime_str);
        lv_obj_align(datetime_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);

        lvgl_port_unlock();
    }
}

void lcd_set_on(bool on)
{
    lcd_on = on;
    if (on)
    {
        lcd_last_on = xTaskGetTickCount();
    }
    if (lcd_booted)
    {
        lcd_update();
    }
}

void lcd_start()
{
    xTaskCreate(lcd_task, "lcd_task", 4 * 1024, NULL, 5, NULL);
    lcd_booted = true;
}

void lcd_set_plugged(bool plugged)
{
    lcd_plugged = plugged;

    lcd_update();
}

void lcd_task(void *pvParameters)
{
    lcd_last_on = xTaskGetTickCount();
    while (1)
    {
        if (lcd_on && (xTaskGetTickCount() - lcd_last_on > pdMS_TO_TICKS(config.interval_lcd_timeout * 1000)))
        {
            ESP_LOGI(TAG, "Turning off LCD");
            lcd_set_on(false);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}