#include "wifiap.h"

std::string WiFiAP::m_tag = "WiFiAP";

WiFiAP::WiFiAP()
{
    m_netifAp = nullptr;
    m_connected = false;

    setChannel(1);
    setMaxConnections(1);
    setBeaconInterval(100);
    setSSIDHidden(false);

    std::string ssid = "Lampshade";
    std::string password = "12345678";

    setCredentials(ssid, password);
}

WiFiAP::~WiFiAP()
{
    stop();
}

bool WiFiAP::start()
{
    ESP_LOGI(m_tag.c_str(), "Starting WiFi AP...");

    esp_log_level_set("wifi", ESP_LOG_NONE);

    initEventHandler();
    initSoftAP();

    if (esp_wifi_start() != ESP_OK)
    {
        ESP_LOGE(m_tag.c_str(), "WiFi start failed...");
        return false;
    }

    m_connected = true;

    return true;
}

void WiFiAP::stop()
{
    ESP_LOGI(m_tag.c_str(), "Stoping WiFi AP...");

    esp_wifi_stop();

    if (m_netifAp)
    {
        esp_netif_destroy_default_wifi(m_netifAp);

        m_netifAp = nullptr;
    }

    m_connected = false;
}

void WiFiAP::setCredentials(const std::string &ssid, const std::string &password)
{
    m_wifiConfig.ap.ssid_len = static_cast<uint8_t>(ssid.size());

    for (size_t i = 0; i < ssid.size(); i++)
    {
        m_wifiConfig.ap.ssid[i] = ssid[i];
    }

    if (ssid.size() < WIFI_SSID_LEN)
    {
        m_wifiConfig.ap.ssid[ssid.size()] = '\0';
    }

    for (size_t i = 0; i < password.size(); i++)
    {
        m_wifiConfig.ap.password[i] = password[i];
    }

    if (password.size() < WIFI_PASS_LEN)
    {
        m_wifiConfig.ap.password[password.size()] = '\0';
    }
}

void WiFiAP::setChannel(uint8_t channel)
{
    m_wifiConfig.ap.channel = channel;
}

void WiFiAP::setMaxConnections(uint8_t max)
{
    m_wifiConfig.ap.max_connection  = max;
}

void WiFiAP::setBeaconInterval(uint16_t interval)
{
    m_wifiConfig.ap.beacon_interval = interval;
}

void WiFiAP::setSSIDHidden(bool hidden)
{
    m_wifiConfig.ap.ssid_hidden = static_cast<uint8_t>(hidden);
}

bool WiFiAP::isconnected()
{
    return m_connected;
}

WiFi::Mode WiFiAP::mode()
{
    return Mode::AP;
}

void WiFiAP::initSoftAP()
{
    ESP_LOGI(m_tag.c_str(), "Initializing WiFi AP");

    ESP_ERROR_CHECK(esp_netif_init());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    m_netifAp = esp_netif_create_default_wifi_ap();

    m_wifiConfig.ap.authmode = WIFI_AUTH_WPA2_PSK;
    
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &m_wifiConfig));

    ESP_LOGI(m_tag.c_str(), "Finished WiFi Soft AP. SSID: %s password: %s channel: %d",
             m_wifiConfig.ap.ssid, m_wifiConfig.ap.password, m_wifiConfig.ap.channel);
}

void WiFiAP::initEventHandler()
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WiFiAP::eventHandler, nullptr, nullptr));
}

void WiFiAP::eventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_AP_STACONNECTED))
    {
        wifi_event_ap_staconnected_t *event_conn = static_cast<wifi_event_ap_staconnected_t *>(event_data);
        ESP_LOGI(m_tag.c_str(), "Station " MACSTR " connected, AID=%d", MAC2STR(event_conn->mac), event_conn->aid);
    }
    else if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_AP_STADISCONNECTED))
    {
        wifi_event_ap_staconnected_t *event_conn = static_cast<wifi_event_ap_staconnected_t *>(event_data);
        ESP_LOGI(m_tag.c_str(), "Station " MACSTR " disconnected, AID=%d", MAC2STR(event_conn->mac), event_conn->aid);
    }
}
