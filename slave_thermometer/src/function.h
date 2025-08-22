#include <WifiEspNow.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <DHT.h>
#include "config.h"
#include <EEPROM.h>
#include <string.h> // Pour memcpy

#define MAGIC_NUMBER 0xDBE49EB0

typedef struct telemetry_message_t
{
  float temperature;
  float humidity;
  uint8_t battery_percentage;
  uint16_t battery_voltage_mv;
} telemetry_message_t;

typedef struct config_t
{
  uint32_t magic_number;
  uint32_t interval_s;
} config_t;

config_t config;

bool config_received = false;

void sleep()
{
  DBG_PRINTLN("sleep for " + String(config.interval_s) + " s");
  ESP.deepSleep(config.interval_s * 1000000);
}

void printReceivedMessage(const uint8_t mac[6], const uint8_t *buf, size_t count, void *cbarg)
{

  DBG_PRINTF("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Vérifier que buf n'est pas null
  if (buf == nullptr)
  {
    DBG_PRINTLN("Received null buffer");
    return;
  }

  if (count != sizeof(config_t))
  {
    DBG_PRINTLN("Received message size mismatch");
    DBG_PRINTF("Expected: %d, Received: %d\n", sizeof(config_t), count);
    return;
  }

  // Copier les données de manière sécurisée au lieu d'utiliser un cast direct
  config_t received_config;
  memcpy(&received_config, buf, sizeof(config_t));

  if (received_config.magic_number != MAGIC_NUMBER)
  {
    DBG_PRINTLN("Received message has invalid magic number");
    DBG_PRINTF("Expected: 0x%08X, Received: 0x%08X\n", MAGIC_NUMBER, received_config.magic_number);
    return;
  }

  config = received_config;
  config_received = true;
  DBG_PRINTLN("Config received and validated successfully");
}

void setupEspNow()
{
  WiFi.persistent(false);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESPNOW", nullptr, 3);
  WiFi.softAPdisconnect(false);

  DBG_PRINT("MAC address of this node is ");
  DBG_PRINTLN(WiFi.softAPmacAddress());

  bool ok = WifiEspNow.begin();
  if (!ok)
  {
    DBG_PRINTLN("WifiEspNow.begin() failed");
    sleep();
  }

  WifiEspNow.onReceive(printReceivedMessage, nullptr);

  ok = WifiEspNow.addPeer(PEER);
  if (!ok)
  {
    DBG_PRINTLN("WifiEspNow.addPeer() failed");
    sleep();
  }
}

float TempClient()
{
  float t = dht.readTemperature(); // get temperature
  int ntry = 0;
  while (isnan(t) && ntry < 5)
  {
    if (ntry != 0)
    {
      delay(100);
    }
    ntry++;
    t = dht.readTemperature();
  }
  return t;
}
float HumClient()
{
  float h = dht.readHumidity(); // get humidity
  int ntry = 0;
  while (isnan(h) && ntry < 5)
  {
    if (ntry != 0)
    {
      delay(100);
    }
    ntry++;
    h = dht.readHumidity();
  }
  return h;
}
float getBatteryVoltage()
{
  // Pont diviseur: R1 = 22kΩ, R2 = 56kΩ
  float voltage_divider_factor = (22000.0 + 56000.0) / (56000.0);

  DBG_PRINTLN("Battery Voltage (raw): " + String(analogRead(BATTERYPIN)));
  return (analogRead(BATTERYPIN) * (3.3 / 1024.0)) * voltage_divider_factor * 1000;
}

// Voltage to percentage lookup table (more accurate)
const float voltageTable[][2] = {
    {4.20, 100},
    {4.15, 95},
    {4.11, 90},
    {4.08, 85},
    {4.02, 80},
    {3.98, 75},
    {3.95, 70},
    {3.91, 65},
    {3.87, 60},
    {3.85, 55},
    {3.84, 50},
    {3.82, 45},
    {3.80, 40},
    {3.79, 35},
    {3.77, 30},
    {3.75, 25},
    {3.73, 20},
    {3.71, 15},
    {3.69, 10},
    {3.61, 5},
    {3.27, 0},
};

int getBattery()
{
  float voltage_v = getBatteryVoltage() / 1000.0;

  // Linear interpolation in the lookup table
  for (int i = 0; i < sizeof(voltageTable) / sizeof(voltageTable[0]) - 1; i++)
  {
    if (voltage_v >= voltageTable[i + 1][0])
    {
      float v1 = voltageTable[i + 1][0];
      float v2 = voltageTable[i][0];
      float p1 = voltageTable[i + 1][1];
      float p2 = voltageTable[i][1];

      return (int)(p1 + (voltage_v - v1) * (p2 - p1) / (v2 - v1));
    }
  }
  return 0;
}

void sendTemp()
{
  dht.begin();

  telemetry_message_t msg;
  //----Get values-----------
  msg.temperature = TempClient();
  msg.humidity = HumClient();
  msg.battery_percentage = getBattery();
  msg.battery_voltage_mv = getBatteryVoltage();

  DBG_PRINTLN("Temperature: " + String(msg.temperature) + " °C");
  DBG_PRINTLN("Humidity: " + String(msg.humidity) + " %");
  DBG_PRINTLN("Battery: " + String(msg.battery_percentage) + " %");
  DBG_PRINTLN("Battery Voltage: " + String(msg.battery_voltage_mv) + " mV");

  bool success = WifiEspNow.send(PEER, (uint8_t *)&msg, sizeof(msg)); // send to the server

  if (!success)
  {
    DBG_PRINTLN("Data send failed");
  }
  else
  {
    DBG_PRINTLN("Data sent");
  }
}

void getConfig()
{
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, config);
  DBG_PRINTLN("Config read: " + String(config.interval_s));
  if (config.magic_number != MAGIC_NUMBER)
  {
    DBG_PRINTLN("Config not found, using default values");
    config.magic_number = MAGIC_NUMBER;
    config.interval_s = 10 * 60;
    EEPROM.put(0, config);
    EEPROM.commit();
  }
  else
  {
    DBG_PRINTLN("Config loaded successfully");
  }
  EEPROM.end();
}