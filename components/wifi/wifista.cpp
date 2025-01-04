#include "wifista.h"

std::string WiFiSTA::m_tag = "wifista";

uint32_t WiFiSTA::m_connectedBit = BIT0;
uint32_t WiFiSTA::m_failedBit = BIT1;

uint8_t WiFiSTA::m_retryNum = 0;

EventGroupHandle_t WiFiSTA::m_eventGroup = nullptr;

WiFiSTA::WiFiSTA()
{
    m_netifSta = nullptr;
    m_connected = false;
    setRetry(WIFI_MAX_RETRY);
}

WiFiSTA::WiFiSTA(const std::string &ssid, const std::string &password) 
{
    m_netifSta = nullptr;
    m_connected = false;
    setRetry(WIFI_MAX_RETRY);
    setCredentials(ssid, password);
}

WiFiSTA::~WiFiSTA()
{
    stop();
}

bool WiFiSTA::start()
{
    ESP_LOGI(m_tag.c_str(), "Starting WiFi STA...");

    esp_log_level_set("wifi", ESP_LOG_NONE);

    m_eventGroup = xEventGroupCreate();

    initNetif();
    initSTA();

    int8_t max_tx_power = 80; // 20 dBm
    esp_wifi_set_max_tx_power(max_tx_power);

    if (esp_wifi_start() != ESP_OK)
    {
        ESP_LOGE(m_tag.c_str(), "WiFi start failed...");
        return false;
    }

    EventBits_t bits = xEventGroupWaitBits(m_eventGroup,
                                           m_connectedBit | m_failedBit,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    if (bits & m_connectedBit) 
    {
        m_connected = true;
        ESP_LOGI(m_tag.c_str(), "Connected to ap SSID: %s password: %s", m_wifiConfig.sta.ssid, m_wifiConfig.sta.password);

        if (m_netifSta)
        {
            esp_netif_set_default_netif(m_netifSta);
        }

        initMDNS();

        return true;
    } 
    else if (bits & m_failedBit) 
    {
        ESP_LOGE(m_tag.c_str(), "Failed to connect to SSID: %s, password: %s",  m_wifiConfig.sta.ssid, m_wifiConfig.sta.password);
    }
    else
    {
        ESP_LOGE(m_tag.c_str(), "Unexpected event");
    } 
   
    return false;
}

void WiFiSTA::stop()
{
    ESP_LOGI(m_tag.c_str(), "Stoping WiFi STA...");
    
    esp_wifi_stop();

    if (m_netifSta)
    {
        esp_netif_destroy_default_wifi(m_netifSta);

        m_netifSta = nullptr;
    }

    if (m_eventGroup)
    {
        vEventGroupDelete(m_eventGroup);

        m_eventGroup = nullptr;
    }

    m_connected = false;
}

void WiFiSTA::setCredentials(const std::string &ssid, const std::string &password)
{   
    if ((ssid.size() > WIFI_SSID_LEN) || (password.size() > WIFI_PASS_LEN))
    {
        ESP_LOGE(m_tag.c_str(), "ssid or password out of range");
        return;
    }

    for (size_t i = 0; i < ssid.size(); i++)
    {
        m_wifiConfig.sta.ssid[i] = ssid[i];
    }

    if (ssid.size() < WIFI_SSID_LEN)
    {
        m_wifiConfig.sta.ssid[ssid.size()] = '\0';
    }

    for (size_t i = 0; i < password.size(); i++)
    {
        m_wifiConfig.sta.password[i] = password[i];
    }

    if (password.size() < WIFI_PASS_LEN)
    {
        m_wifiConfig.sta.password[password.size()] = '\0';
    }
}

void WiFiSTA::setRetry(uint8_t max)
{
    m_wifiConfig.sta.failure_retry_cnt = max;
}

bool WiFiSTA::isconnected()
{
    return m_connected;
}

void WiFiSTA::initSTA()
{
    ESP_LOGI(m_tag.c_str(), "Initializing WiFi STA");

    ESP_ERROR_CHECK(esp_netif_init());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    m_wifiConfig.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &m_wifiConfig));

    ESP_LOGI(m_tag.c_str(), "Finished WiFi STA");
}

void WiFiSTA::initNetif()
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    m_netifSta = esp_netif_create_default_wifi_sta();

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WiFiSTA::eventHandler, nullptr, nullptr));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &WiFiSTA::eventHandler, nullptr, nullptr));
}

void WiFiSTA::initMDNS()
{
    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set(WIFI_HOSTNAME));
    ESP_ERROR_CHECK(mdns_service_add(WIFI_HOSTNAME, "_http", "_tcp", 80, NULL, 0));
    ESP_LOGI(m_tag.c_str(), "mDNS initialized with hostname: %s.local", WIFI_HOSTNAME);
}

void WiFiSTA::eventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_START)) 
    {
        esp_wifi_connect();

        ESP_LOGI(m_tag.c_str(), "Station started!");
    } 
    else if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_DISCONNECTED)) 
    {
        if (m_retryNum < WIFI_MAX_RETRY)
        {
            m_retryNum++;
            esp_wifi_connect(); 

            ESP_LOGI(m_tag.c_str(), "Station trying to connect...");
        }
        else
        {
            ESP_LOGE(m_tag.c_str(), "Station connection failed!");
            xEventGroupSetBits(m_eventGroup, m_failedBit);
        }
    } 
    else if ((event_base == IP_EVENT) && (event_id == IP_EVENT_STA_GOT_IP)) 
    {
        ip_event_got_ip_t *event = static_cast<ip_event_got_ip_t*>(event_data);

        ESP_LOGI(m_tag.c_str(), "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));

        m_retryNum = 0;

        xEventGroupSetBits(m_eventGroup, m_connectedBit);
    }
}
