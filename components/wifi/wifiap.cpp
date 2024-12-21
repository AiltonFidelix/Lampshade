#include "wifiap.h"

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"

WiFiAP::WiFiAP() :
    m_connected(false),
    m_ssidHidden(false),
    m_channel(1),
    m_maxConnections(1),
    m_beaconInterval(100),
    m_tag("WiFiAP"),
    m_ssid("Lampshade"),
    m_password("12345678"),
    m_monitor(nullptr)
{
    ESP_LOGI(m_tag.c_str(), "Creating...");
}

WiFiAP::~WiFiAP()
{
    stop();

    if (m_monitor)
    {
        delete m_monitor;
    }
}

void WiFiAP::start()
{
    if (m_monitorHandle)
    {
        ESP_LOGI(m_tag.c_str(), "Task ja foi criada, apenas resumindo...");
        vTaskResume(m_monitorHandle);
    }
    else
    {
        esp_log_level_set("wifi", ESP_LOG_NONE);

        ESP_LOGI(m_tag.c_str(), "Task nao foi criada ainda, criando...");

        xTaskCreate(WiFiAP::monitorTask, "monitorTask", 4096, NULL, 5, &m_monitorHandle);
    }
}

void WiFiAP::stop()
{
    if (m_monitorHandle)
    {
        esp_wifi_stop();
        vTaskSuspend(m_monitorHandle);
        ESP_LOGI(m_tag.c_str(), "Stoping WiFi task...");
    }
}

void WiFiAP::setSSID(const std::string &ssid)
{
    m_ssid = ssid;
}

void WiFiAP::setPassword(const std::string &password)
{
    m_password = password;
}

void WiFiAP::setChannel(uint8_t channel)
{
    m_channel = channel;
}

void WiFiAP::setMaxConnections(uint8_t max)
{
    m_maxConnections = max;
}

void WiFiAP::setBeaconInterval(uint16_t interval)
{
    m_beaconInterval = interval;
}

void WiFiAP::setSSIDHidden(bool hidden)
{
    m_ssidHidden = hidden;
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

    // Inicializa TCP stack
    ESP_ERROR_CHECK(esp_netif_init());

    // Inicializa WiFi em modo AP
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    esp_netif_t *esp_netif_ap = esp_netif_create_default_wifi_ap();
    (void)esp_netif_ap;

    wifi_config_t wifi_ap_config = {
        .ap = {
            .ssid_len = static_cast<uint8_t>(m_ssid.size()),
            .channel = m_channel,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .ssid_hidden = static_cast<uint8_t>(m_ssidHidden),
            .max_connection = m_maxConnections,
            .beacon_interval = m_beaconInterval,
            .pmf_cfg = {
                .required = false,
            },
        },
    };

    for (size_t i = 0; i < m_ssid.size(); i++)
    {
        wifi_ap_config.ap.ssid[i] = m_ssid[i];
    }

    for (size_t i = 0; i < m_password.size(); i++)
    {
        wifi_ap_config.ap.password[i] = m_password[i];
    }

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));
    ESP_LOGI(m_tag.c_str(), "Finished WiFi AP. SSID: %s password: %s channel: %d", m_ssid.c_str(), m_password.c_str(), m_channel);
}

void WiFiAP::wifiAPTask(void *pvParameters)
{
    // Handle *handle = static_cast<Handle*>(pvParameters);

    // ESP_LOGI(handle->tag.c_str(), "Initializing WiFi AP task...");

    // m_eventGroup = xEventGroupCreate();

    // wifi_event_handler_init();
    // initSoftAP();

    if (esp_wifi_start() != ESP_OK)
    {
        // ESP_LOGE(handle->tag.c_str(), "WiFi start failed...");
        vTaskDelete(NULL);
    }
}

void WiFiAP::monitorTask(void *pvParameters)
{
    bool *connected = static_cast<bool*>(pvParameters);

    EventGroupHandle_t eventGroup = xEventGroupCreate();

    uint32_t started_bit = BIT0;
    uint32_t connected_bit = BIT1;
    uint32_t failed_bit = BIT2;

    while (true)
    {
        EventBits_t bits = xEventGroupWaitBits(eventGroup,
                                               started_bit | connected_bit | failed_bit,
                                               pdFALSE,
                                               pdFALSE,
                                               portMAX_DELAY);

        if (connected)
        {
            *connected = (bits & connected_bit);
        }

        if (bits & started_bit) 
        {
            ESP_LOGI("WiFiAPMonitor", "WiFi started sucessfully...");
            xEventGroupClearBits(eventGroup, started_bit);
        }
        else if (bits & failed_bit)
        {
            ESP_LOGE("WiFiAPMonitor", "WiFi start failed...");
            xEventGroupClearBits(eventGroup, failed_bit);
        }

        vTaskDelay(100);
    }
}
