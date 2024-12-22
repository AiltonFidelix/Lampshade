#ifndef __WIFIAP_H
#define __WIFIAP_H

#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "lwip/netdb.h"

#include "wifi.h"

class WiFiAP : public WiFi
{
    using Monitor = struct {
        bool connected;
        bool ssidHidden;

        uint8_t channel;
        uint8_t maxConnections;
        uint16_t beaconInterval;

        std::string tag;
        std::string ssid;
        std::string password;

        TaskHandle_t taskHandle;
        EventGroupHandle_t eventGroup;
    };

public:
    WiFiAP();
    ~WiFiAP();

    bool start() override;
    void stop() override;
    
    void setCredentials(const std::string &ssid, const std::string &password) override;
    void setChannel(uint8_t channel);
    void setMaxConnections(uint8_t max);
    void setBeaconInterval(uint16_t interval);
    void setSSIDHidden(bool hidden);

    bool isconnected() override;

    Mode mode() override;

private:
    static void wifiEventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
    static void initAP(Monitor *monitor);
    static void initEventHandler(Monitor *monitor);
    static void monitorTask(void *pvParameters);

private:
    // TODO: change to static variables
    // bool m_connected;
    // bool m_ssidHidden;

    // uint8_t m_channel;
    // uint8_t m_maxConnections;
    // uint16_t m_beaconInterval;

    // std::string m_tag;
    // std::string m_ssid;
    // std::string m_password;

    // TaskHandle_t m_monitorHandle;

    Monitor *m_monitor;
};

#endif