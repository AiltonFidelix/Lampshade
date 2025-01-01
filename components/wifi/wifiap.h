#ifndef __WIFIAP_H
#define __WIFIAP_H

#include <string>

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "lwip/netdb.h"

#include "interface/wifi.h"
#include "wificonfig.h"

class WiFiAP : public WiFi
{
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

private:
    void initSoftAP();
    void initEventHandler();
    // void scan();
    static void eventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

private:
    static std::string m_tag;

    bool m_connected;
    wifi_config_t m_wifiConfig;
    esp_netif_t *m_netifAp;
};

#endif