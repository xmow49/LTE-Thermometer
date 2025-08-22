#include "function.h"

void setup()
{
#if DEBUG
	Serial.begin(115200);
	Serial.setTimeout(2000);
	delay(1000);
#endif

	getConfig();
	config_t read_config = config;
	setupEspNow();
	sendTemp();

	uint32_t start = millis();
	while (!config_received && millis() - start < 2000) // Wait for config for 2 seconds
	{
		delay(100);
	}
	if (config_received)
	{
		DBG_PRINTLN("old config: " + String(read_config.interval_s));
		if (memcmp(&read_config, &config, sizeof(config)) == 0)
		{
			DBG_PRINTLN("Config received but unchanged: sleep");
			sleep();
		}
		DBG_PRINTLN("NEW interval: " + String(config.interval_s));
		EEPROM.begin(EEPROM_SIZE);
		EEPROM.put(0, config);
		EEPROM.commit();
		EEPROM.end();
		DBG_PRINTLN("Config saved");
		delay(1000);
		sleep();
	}
	else
	{
		DBG_PRINTLN("Config not received: sleep");
	}
	sleep();
}
void loop()
{
}
