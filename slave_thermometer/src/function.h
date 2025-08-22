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
  // Debug:
  DBG_PRINTF("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]); // Debug

  // Vérifier que buf n'est pas null
  if (buf == nullptr)
  {
    DBG_PRINTLN("Received null buffer"); // Debug
    return;
  }

  if (count != sizeof(config_t))
  {
    DBG_PRINTLN("Received message size mismatch");                       // Debug
    DBG_PRINTF("Expected: %d, Received: %d\n", sizeof(config_t), count); // Debug
    return;
  }

  // Copier les données de manière sécurisée au lieu d'utiliser un cast direct
  config_t received_config;
  memcpy(&received_config, buf, sizeof(config_t));

  if (received_config.magic_number != MAGIC_NUMBER)
  {
    DBG_PRINTLN("Received message has invalid magic number");                                       // Debug
    DBG_PRINTF("Expected: 0x%08X, Received: 0x%08X\n", MAGIC_NUMBER, received_config.magic_number); // Debug
    return;
  }

  config = received_config;
  config_received = true;
  DBG_PRINTLN("Config received and validated successfully"); // Debug
}

void setupEspNow()
{
  Serial.begin(115200); // Debug
  Serial.setTimeout(2000);

  WiFi.persistent(false);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESPNOW", nullptr, 3);
  WiFi.softAPdisconnect(false);

  // DBG_PRINT("MAC address of this node is "); //Debug
  // DBG_PRINTLN(WiFi.softAPmacAddress());

  bool ok = WifiEspNow.begin();
  if (!ok)
  {
    DBG_PRINTLN("WifiEspNow.begin() failed"); // Debug
    sleep();
  }

  WifiEspNow.onReceive(printReceivedMessage, nullptr);

  ok = WifiEspNow.addPeer(PEER);
  if (!ok)
  {
    DBG_PRINTLN("WifiEspNow.addPeer() failed"); // Debug
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
      delay(200);
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
      delay(200);
    }
    ntry++;
    h = dht.readHumidity();
  }
  return h;
}
int getBattery()
{
  float b = analogRead(BATTERYPIN); // get battery
  DBG_PRINTLN(b);
  b = (b - 677) / (840 - 677) * 100; // set in percentage
  if (b > 100)
  {
    b = 100;
  }
  else if (b < 0)
  {
    b = 0;
  }
  DBG_PRINTLN(b);
  return b;
}

float getBatteryVoltage()
{
  return analogRead(BATTERYPIN); // get battery
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

  bool success = WifiEspNow.send(PEER, (uint8_t *)&msg, sizeof(msg)); // send to the server

  if (!success)
  {
    DBG_PRINTLN("Data send failed"); // Debug
  }
  else
  {
    DBG_PRINTLN("Data sent"); // Debug
  }
  // sleep();
}

void getConfig()
{
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, config);
  if (config.magic_number != MAGIC_NUMBER)
  {
    DBG_PRINTLN("Config not found, using default values");
    config.magic_number = MAGIC_NUMBER;
    config.interval_s = 5 * 60;
    EEPROM.put(0, config);
    EEPROM.commit();
  }
  else
  {
    DBG_PRINTLN("Config loaded successfully");
  }
  EEPROM.end();
}