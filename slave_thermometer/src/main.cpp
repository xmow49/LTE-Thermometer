#include "function.h"

void setup()
{
	getConfig();
	setupEspNow();
	sendTemp();

	uint32_t start = millis();
	while (!config_received && millis() - start < 2000) // Wait for config for 2 seconds
	{
		delay(100);
	}
	if (config_received)
	{
		DBG_PRINTLN("NEW interval: " + String(config.interval_s));
		EEPROM.put(0, config);
		EEPROM.commit();
		DBG_PRINTLN("Config saved");
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
