#ifndef __WIFIAP_H
#define __WIFIAP_H

#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "wifi.h"

class WiFiAP : public WiFi
{
    using Monitor = struct {
        bool *connected;
        TaskHandle_t taskHandle;
        EventGroupHandle_t eventGroup;
    };

public:
    WiFiAP();
    ~WiFiAP();

    void start() override;
    void stop() override;
    
    void setSSID(const std::string &ssid) override;
    void setPassword(const std::string &password) override;
    void setChannel(uint8_t channel);
    void setMaxConnections(uint8_t max);
    void setBeaconInterval(uint16_t interval);
    void setSSIDHidden(bool hidden);

    bool isconnected() override;

    Mode mode() override;

private:
    void initSoftAP();
    static void wifiAPTask(void *pvParameters);
    static void monitorTask(void *pvParameters);

private:
    static int m_typeId;
    bool m_connected;
    bool m_ssidHidden;

    uint8_t m_channel;
    uint8_t m_maxConnections;
    uint16_t m_beaconInterval;

    std::string m_tag;
    std::string m_ssid;
    std::string m_password;

    TaskHandle_t m_monitorHandle;
    EventGroupHandle_t m_monitorGroup;

    Monitor *m_monitor;
};

#endif