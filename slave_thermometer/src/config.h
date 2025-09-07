#ifndef CONFIG_H
#define CONFIG_H

// 3c:84:27:26:dc:30
// static uint8_t PEER[]{0x3c, 0x84, 0x27, 0x26, 0xdc, 0x30}; // MAC address of the server
static uint8_t PEER[]{0x84, 0x0D, 0x8E, 0xD2, 0x31, 0xE4}; // MAC Adress of Server

#define BATTERYPIN A0 // Analog pin of battery
#define DHTPIN 13     // pin of DHT22
#define DHTTYPE DHT22 // DHT type

#define EEPROM_SIZE 512 // Size of EEPROM
#define USE_AHT20

#define DEBUG 0

#if DEBUG
#define DBG_PRINT(x) Serial.print(x)
#define DBG_PRINTLN(x) Serial.println(x)
#define DBG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DBG_PRINT(x)
#define DBG_PRINTLN(x)
#define DBG_PRINTF(...)
#endif

#endif // CONFIG_H