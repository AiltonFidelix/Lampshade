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
public:
    WiFiAP();
    ~WiFiAP();

    void start() override;
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
    static void initAP(wifi_config_t *wifiConfig);
    static void initEventHandler();
    static void monitorTask(void *pvParameters);

private:
    static std::string m_tag;
    static bool m_connected;

    static uint32_t m_startedBit;
    static uint32_t m_connectedBit;
    static uint32_t m_failedBit;

    static TaskHandle_t m_taskHandle;
    static EventGroupHandle_t m_eventGroup;
  
    wifi_config_t m_wifiConfig;
};

#endif