#include "wifiap.h"

WiFiAP::WiFiAP() : m_monitor(new Monitor())
{
    m_monitor->connected = false;
    m_monitor->ssidHidden = false;

    m_monitor->channel = 1;
    m_monitor->maxConnections = 1;
    m_monitor->beaconInterval = 100;

    m_monitor->tag = "WiFiAP";
    m_monitor->ssid = "Lampshade";
    m_monitor->password = "12345678";
}

WiFiAP::~WiFiAP()
{
    stop();

    if (m_monitor)
    {
        delete m_monitor;
    }
}

bool WiFiAP::start()
{
    if (m_monitor->taskHandle)
    {
        ESP_LOGI(m_monitor->tag.c_str(), "Task has been created, just resuming...");
        vTaskResume(m_monitor->taskHandle);
    }
    else
    {
        esp_log_level_set("wifi", ESP_LOG_NONE);

        ESP_LOGI(m_monitor->tag.c_str(), "Task hasn't been created, creating...");

        xTaskCreate(WiFiAP::monitorTask, "monitorTask", 4096, m_monitor, 5, &m_monitor->taskHandle);
    }

    return true;
}

void WiFiAP::stop()
{
    if (m_monitor->taskHandle)
    {
        esp_wifi_stop();
        vTaskSuspend(m_monitor->taskHandle);
        ESP_LOGI(m_monitor->tag.c_str(), "Stoping WiFi task...");
    }
}

void WiFiAP::setCredentials(const std::string &ssid, const std::string &password)
{
    m_monitor->ssid = ssid;
    m_monitor->password = password;
}

void WiFiAP::setChannel(uint8_t channel)
{
    m_monitor->channel = channel;
}

void WiFiAP::setMaxConnections(uint8_t max)
{
    m_monitor->maxConnections = max;
}

void WiFiAP::setBeaconInterval(uint16_t interval)
{
    m_monitor->beaconInterval = interval;
}

void WiFiAP::setSSIDHidden(bool hidden)
{
    m_monitor->ssidHidden = hidden;
}

bool WiFiAP::isconnected()
{
    return m_monitor->connected;
}

WiFi::Mode WiFiAP::mode()
{
    return Mode::AP;
}

void WiFiAP::wifiEventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    Monitor *monitor = static_cast<Monitor *>(arg);

    if (monitor == nullptr)
    {
        ESP_LOGE("WiFiAPEventHandler", "Failed to catch monitor struct...");
        return;
    }

    uint32_t started_bit = BIT0;

    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            case WIFI_EVENT_AP_START:
                ESP_LOGI(monitor->tag.c_str(), "Iniciou Wifi AP");
                xEventGroupSetBits(monitor->eventGroup, started_bit);
                break;

            case WIFI_EVENT_AP_STACONNECTED:
            {
                wifi_event_ap_staconnected_t *event_conn = static_cast<wifi_event_ap_staconnected_t *>(event_data);
                ESP_LOGI(monitor->tag.c_str(), "Station " MACSTR " connected, AID=%d", MAC2STR(event_conn->mac), event_conn->aid);
            }
            break;

            case WIFI_EVENT_AP_STADISCONNECTED:
            {
                wifi_event_ap_stadisconnected_t *event_disc = static_cast<wifi_event_ap_stadisconnected_t *>(event_data);
                ESP_LOGI(monitor->tag.c_str(), "Station " MACSTR " disconnected, AID=%d", MAC2STR(event_disc->mac), event_disc->aid);
            }
            break;

            default:
                break;
        }
    }
}

void WiFiAP::initAP(Monitor *monitor)
{
    ESP_LOGI(monitor->tag.c_str(), "Initializing WiFi AP");

    ESP_ERROR_CHECK(esp_netif_init());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    esp_netif_create_default_wifi_ap();

    wifi_config_t wifi_ap_config = {
        .ap = {
            .ssid_len = static_cast<uint8_t>(monitor->ssid.size()),
            .channel = monitor->channel,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .ssid_hidden = static_cast<uint8_t>(monitor->ssidHidden),
            .max_connection = monitor->maxConnections,
            .beacon_interval = monitor->beaconInterval,
            .pmf_cfg = {
                .required = false,
            },
        },
    };

    for (size_t i = 0; i < monitor->ssid.size(); i++)
    {
        wifi_ap_config.ap.ssid[i] = monitor->ssid[i];
    }

    for (size_t i = 0; i < monitor->password.size(); i++)
    {
        wifi_ap_config.ap.password[i] = monitor->password[i];
    }

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));

    ESP_LOGI(monitor->tag.c_str(), "Finished WiFi AP. SSID: %s password: %s channel: %d",
             monitor->ssid.c_str(),
             monitor->password.c_str(),
             monitor->channel);
}

void WiFiAP::initEventHandler(Monitor *monitor)
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WiFiAP::wifiEventHandler, static_cast<void *>(monitor), nullptr));
}

void WiFiAP::monitorTask(void *pvParameters)
{
    Monitor *monitor = static_cast<Monitor *>(pvParameters);

    if (monitor == nullptr)
    {
        ESP_LOGE("WiFiAPMonitorTask", "Failed to catch monitor struct...");
        vTaskDelete(nullptr);
    }

    monitor->eventGroup = xEventGroupCreate();

    initEventHandler(monitor);
    initAP(monitor);

    if (esp_wifi_start() != ESP_OK)
    {
        ESP_LOGE(monitor->tag.c_str(), "WiFi start failed...");
        return;
    }

    uint32_t started_bit = BIT0;
    uint32_t connected_bit = BIT1;
    uint32_t failed_bit = BIT2;

    while (true)
    {
        EventBits_t bits = xEventGroupWaitBits(monitor->eventGroup,
                                               started_bit | connected_bit | failed_bit,
                                               pdFALSE,
                                               pdFALSE,
                                               portMAX_DELAY);

        monitor->connected = (bits & connected_bit);

        if (bits & started_bit)
        {
            ESP_LOGI(monitor->tag.c_str(), "WiFi started sucessfully...");
            xEventGroupClearBits(monitor->eventGroup, started_bit);
        }
        else if (bits & failed_bit)
        {
            ESP_LOGE(monitor->tag.c_str(), "WiFi start failed...");
            xEventGroupClearBits(monitor->eventGroup, failed_bit);
        }

        vTaskDelay(100);
    }
}
