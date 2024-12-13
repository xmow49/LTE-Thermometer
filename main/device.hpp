#ifndef DEVICE_HPP
#define DEVICE_HPP

#define DEVICE_GATEWAY_MAC "00:00:00:00:00:00"

typedef struct telemetry_message
{
    float temperature;
    float humidity;
    int battery;
} telemetry_message;

#ifdef __cplusplus

#include <vector>
#include <string>
#include <ThingsBoard.h>
#include <Espressif_MQTT_Client.h>

class TelemetryReport
{
public:
    std::string name;
    float value;
    time_t timestamp;
    TelemetryReport(std::string name, float value);
    ~TelemetryReport();
};

class Device
{
public:
    Device(char *name, char *mac);
    ~Device();
    bool sendJsonTelemetry(char *json);
    bool sendTelemetry(char *key, float value);

    bool isConnected() const { return connected; }
    const uint8_t *getMac() const { return mac; }

    const std::string &getName() const { return name; }

    std::string getMacString() const;
    std::vector<TelemetryReport> &getTelemetryList() { return telemetry_list; }
    void addTelemetry(TelemetryReport telemetry);
    std::string computeTelemetryJson();
    void clearTelemetry();

private:
    std::string name;
    uint8_t mac[6];
    bool connected = false;
    bool isGateway = false;
    std::vector<TelemetryReport> telemetry_list;
};

class DeviceList
{
public:
    void add(Device *device);
    Device *find_by_mac(const char *mac);
    std::vector<Device *> &get_all();

private:
    std::vector<Device *> devices;
};

DeviceList &get_devices_list();

extern "C"
{
#endif

    void device_list();
    void device_add(char *name, char *mac);
    void device_receive(char *mac, telemetry_message data);
    void device_report_telemetry(char *mac, char *key, float value);
#ifdef __cplusplus
}
#endif

#endif // DEVICE_HPP