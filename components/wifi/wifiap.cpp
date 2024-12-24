#include "wifiap.h"

std::string WiFiAP::m_tag = "WiFiAP";
bool WiFiAP::m_connected = false;

uint32_t WiFiAP::m_startedBit = BIT0;
uint32_t WiFiAP::m_connectedBit = BIT1;
uint32_t WiFiAP::m_failedBit = BIT2;

TaskHandle_t WiFiAP::m_taskHandle = nullptr;
EventGroupHandle_t WiFiAP::m_eventGroup = nullptr;

WiFiAP::WiFiAP()
{
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

void WiFiAP::start()
{
    if (m_taskHandle)
    {
        ESP_LOGI(m_tag.c_str(), "Task has been created, just resuming...");
        vTaskResume(m_taskHandle);
    }
    else
    {
        esp_log_level_set("wifi", ESP_LOG_NONE);

        ESP_LOGI(m_tag.c_str(), "Task hasn't been created, creating...");

        xTaskCreate(WiFiAP::monitorTask, "monitorTask", WIFI_TASK_STACK, &m_wifiConfig, 5, &m_taskHandle);
    }
}

void WiFiAP::stop()
{
    if (m_taskHandle)
    {
        ESP_LOGI(m_tag.c_str(), "Stoping WiFi task...");
        esp_wifi_stop();
        vTaskSuspend(m_taskHandle);

        m_taskHandle = nullptr;
    }

    if (m_eventGroup)
    {
        vEventGroupDelete(m_eventGroup);

        m_eventGroup = nullptr;
    }
}

void WiFiAP::setCredentials(const std::string &ssid, const std::string &password)
{
    m_wifiConfig.ap.ssid_len = static_cast<uint8_t>(ssid.size());

    for (size_t i = 0; i < ssid.size(); i++)
    {
        m_wifiConfig.ap.ssid[i] = ssid[i];
    }

    for (size_t i = 0; i < password.size(); i++)
    {
        m_wifiConfig.ap.password[i] = password[i];
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

void WiFiAP::wifiEventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            case WIFI_EVENT_AP_START:
            {
                ESP_LOGI(m_tag.c_str(), "Iniciou Wifi AP");
                xEventGroupSetBits(m_eventGroup, m_startedBit);
            }
                break;

            case WIFI_EVENT_AP_STACONNECTED:
            {
                wifi_event_ap_staconnected_t *event_conn = static_cast<wifi_event_ap_staconnected_t *>(event_data);
                ESP_LOGI(m_tag.c_str(), "Station " MACSTR " connected, AID=%d", MAC2STR(event_conn->mac), event_conn->aid);
            }
                break;

            case WIFI_EVENT_AP_STADISCONNECTED:
            {
                wifi_event_ap_stadisconnected_t *event_disc = static_cast<wifi_event_ap_stadisconnected_t *>(event_data);
                ESP_LOGI(m_tag.c_str(), "Station " MACSTR " disconnected, AID=%d", MAC2STR(event_disc->mac), event_disc->aid);
            }
                break;

            default:
                break;
        }
    }
}

void WiFiAP::initAP(wifi_config_t *wifiConfig)
{
    ESP_LOGI(m_tag.c_str(), "Initializing WiFi AP");

    ESP_ERROR_CHECK(esp_netif_init());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    esp_netif_create_default_wifi_ap();

    wifiConfig->ap.authmode = WIFI_AUTH_WPA2_PSK;
    
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, wifiConfig));

    ESP_LOGI(m_tag.c_str(), "Finished WiFi AP. SSID: %s password: %s channel: %d",
             wifiConfig->ap.ssid, wifiConfig->ap.password, wifiConfig->ap.channel);
}

void WiFiAP::initEventHandler()
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WiFiAP::wifiEventHandler, nullptr, nullptr));
}

void WiFiAP::monitorTask(void *pvParameters)
{
    wifi_config_t *wifiConfig = static_cast<wifi_config_t*>(pvParameters);

    if (wifiConfig == nullptr)
    {
        ESP_LOGE(m_tag.c_str(), "Failed to get wifi settings...");
        vTaskDelete(nullptr);
    }

    m_eventGroup = xEventGroupCreate();

    initEventHandler();
    initAP(wifiConfig);

    if (esp_wifi_start() != ESP_OK)
    {
        ESP_LOGE(m_tag.c_str(), "WiFi start failed...");
        vTaskDelete(nullptr);
    }

    while (true)
    {
        EventBits_t bits = xEventGroupWaitBits(m_eventGroup,
                                               m_startedBit | m_connectedBit | m_failedBit,
                                               pdFALSE,
                                               pdFALSE,
                                               portMAX_DELAY);

        m_connected = (bits & m_connectedBit);

        if (bits & m_startedBit)
        {
            ESP_LOGI(m_tag.c_str(), "WiFi started sucessfully...");
            xEventGroupClearBits(m_eventGroup, m_startedBit);
        }
        else if (bits & m_failedBit)
        {
            ESP_LOGE(m_tag.c_str(), "WiFi start failed...");
            xEventGroupClearBits(m_eventGroup, m_failedBit);
        }

        vTaskDelay(100);
    }
}
