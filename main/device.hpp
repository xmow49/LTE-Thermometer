#ifndef DEVICE_HPP
#define DEVICE_HPP

#define DEVICE_GATEWAY_MAC "00:00:00:00:00:00"

#include <stdint.h>
typedef struct telemetry_message
{
    float temperature;
    float humidity;
    uint8_t battery_percentage;
    uint16_t battery_voltage_mv;
} telemetry_message;

#define MAGIC_NUMBER 0xDBE49EB0

typedef struct config_message_t
{
    uint32_t magic_number;
    uint32_t interval_s;
} config_message_t;

#ifdef __cplusplus

#include <vector>
#include <string>
#include <ThingsBoard.h>
#include <Espressif_MQTT_Client.h>

class TelemetryReport
{
public:
    char name[16];
    double value;
    time_t timestamp;
    TelemetryReport(const char *name, double value);
    ~TelemetryReport();
};

class Device
{
public:
    Device(const char *name, const char *mac);
    ~Device();
    bool sendJsonTelemetry(char *json);
    bool sendTelemetry(char *key, double value);

    bool isConnected() const { return connected; }
    const uint8_t *getMac() const { return mac; }

    const std::string &getName() const { return name; }

    std::string getMacString() const;
    std::vector<TelemetryReport> &getTelemetryList() { return telemetry_list; }
    void addTelemetry(TelemetryReport telemetry);
    std::string computeTelemetryJson();
    void clearTelemetryJson();
    void moveBackTelemetry();
    bool saveTelemetryToSD(); // New method for saving telemetry

private:
    std::string name;
    uint8_t mac[6];
    bool connected = false;
    bool isGateway = false;

    TickType_t last_telemetry_saved = 0;
    std::vector<TelemetryReport> telemetry_list;
    std::vector<TelemetryReport> telemetry_list_json;
    SemaphoreHandle_t telemetry_mutex = xSemaphoreCreateMutex();
};

class DeviceList
{
public:
    void add(Device *device);
    Device *find_by_str_mac(const char *mac);
    Device *find_by_mac(const uint8_t *mac);

    std::vector<Device *> &get_all();
    Device *get_gateway();

private:
    std::vector<Device *> devices;
};

DeviceList &get_devices_list();

extern "C"
{
#endif

    void device_list();
    void device_add(const char *name, const char *mac);
    void device_receive(char *mac, telemetry_message data);
    void device_report_telemetry(char *mac, char *key, double value);

    bool device_gateway_send_json_telemetry(char *json);
    void device_start_telemetry_save_task(void); // New function to start telemetry save task

#ifdef __cplusplus
}
#endif

#endif // DEVICE_HPP