
#ifndef LCD_H
#define LCD_H

#ifdef __cplusplus
extern "C"
{
#endif

    void lcd_init();
    void lcd_update(void);
    void lcd_set_temperature(float temperature, float humidity);
    void lcd_set_battery_level(int battery_level);
    void lcd_set_signal_rssi(int signal_strength);
    void lcd_set_on(bool on);
    void lcd_start();
    void lcd_set_plugged(bool plugged);
    void lcd_setup_msg(const char *title, const char *msg);

#ifdef __cplusplus
}
#endif

#endif // LCD_H
