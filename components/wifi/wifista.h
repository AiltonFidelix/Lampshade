#ifndef __WIFISTA_H
#define __WIFISTA_H

#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "lwip/netdb.h"

#include "interface/wifi.h"
#include "wificonfig.h"

class WiFiSTA : public WiFi
{
public:
    WiFiSTA();
    WiFiSTA(const std::string &ssid, const std::string &password);
    
    ~WiFiSTA();

    bool start() override;
    void stop() override;
    
    void setCredentials(const std::string &ssid, const std::string &password) override;
    void setRetry(uint8_t max);

    bool isconnected() override;

private:
    void initSTA();
    void initEventHandler();
    static void eventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

private:
    static std::string m_tag;

    static uint32_t m_connectedBit;
    static uint32_t m_failedBit;

    static uint8_t m_retryNum;

    static EventGroupHandle_t m_eventGroup;

    bool m_connected;
    wifi_config_t m_wifiConfig;
    esp_netif_t *m_netifSta;
};

#endif