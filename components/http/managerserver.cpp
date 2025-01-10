#include "managerserver.h"
#include "httpconfig.h"
#include "wifistorage.h"

#include "cJSON.h"
#include "esp_wifi.h"

extern const uint8_t index_min_html_start[] asm("_binary_manager_min_html_start");
extern const uint8_t index_min_html_end[] asm("_binary_manager_min_html_end");

extern const uint8_t app_min_js_start[] asm("_binary_manager_min_js_start");
extern const uint8_t app_min_js_end[] asm("_binary_manager_min_js_end");

extern const uint8_t jquery_min_js_start[] asm("_binary_jquery_min_js_start");
extern const uint8_t jquery_min_js_end[] asm("_binary_jquery_min_js_end");

extern const uint8_t app_min_css_start[] asm("_binary_app_min_css_start");
extern const uint8_t app_min_css_end[] asm("_binary_app_min_css_end");

extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start");
extern const uint8_t favicon_ico_end[] asm("_binary_favicon_ico_end");

std::string ManagerServer::m_tag = "managerserver";

Led *ManagerServer::m_led = nullptr;

ManagerServer::ManagerServer()
{
    m_serverHandle = nullptr;
}

ManagerServer::~ManagerServer()
{
    stop();
}

bool ManagerServer::start()
{
    ESP_LOGI(m_tag.c_str(), "Initializing manager server...");

    m_serverHandle = serverConfigure();

    if (m_serverHandle == nullptr)
    {
        ESP_LOGE(m_tag.c_str(), "Manager server initialization failed...");

        return false;
    }

    if (m_led)
    {
        m_led->startBlink(1000);
    }

    return true;
}

void ManagerServer::stop()
{
    if (m_led)
    {
        m_led->stopBlink();
    }
    
    if (m_serverHandle)
    {
        httpd_stop(m_serverHandle);
    }
}

void ManagerServer::setLed(Led *led)
{
    if (led == nullptr)
    {
        return;
    }

    m_led = led;
}

httpd_handle_t ManagerServer::serverConfigure()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.task_priority = HTTP_SERVER_TASK_PRIORITY;
    config.stack_size = HTTP_SERVER_TASK_STACK;

    // Increase uri handlers
    config.max_uri_handlers = 20;

    // Increase the timeout limits
    config.recv_wait_timeout = 10;
    config.send_wait_timeout = 10;

    httpd_handle_t handle;

    if (httpd_start(&handle, &config) == ESP_OK)
    {
        ESP_LOGI(m_tag.c_str(), "Registering URI handlers...");

        httpd_uri_t index = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = ManagerServer::indexHandler,
            .user_ctx = nullptr
        };

        httpd_register_uri_handler(handle, &index);

        httpd_uri_t app_js = {
            .uri = "/js/manager.min.js",
            .method = HTTP_GET,
            .handler = ManagerServer::jsHandler,
            .user_ctx = nullptr
        };

        httpd_register_uri_handler(handle, &app_js);

        httpd_uri_t jquery = {
            .uri = "/js/jquery.min.js",
            .method = HTTP_GET,
            .handler = ManagerServer::jqueryHandler,
            .user_ctx = nullptr
        };

        httpd_register_uri_handler(handle, &jquery);

        httpd_uri_t app_css = {
            .uri = "/css/app.min.css",
            .method = HTTP_GET,
            .handler = ManagerServer::cssHandler,
            .user_ctx = nullptr
        };

        httpd_register_uri_handler(handle, &app_css);

        httpd_uri_t favicon = {
            .uri = "/assets/favicon.ico",
            .method = HTTP_GET,
            .handler = ManagerServer::faviconHandler,
            .user_ctx = nullptr
        };

        httpd_register_uri_handler(handle, &favicon);

        httpd_uri_t networks = {
            .uri = "/networks.json",
            .method = HTTP_GET,
            .handler = ManagerServer::networksHandler,
            .user_ctx = nullptr
        };

        httpd_register_uri_handler(handle, &networks);

        httpd_uri_t connect = {
            .uri = "/connect.json",
            .method = HTTP_POST,
            .handler = ManagerServer::connectHandler,
            .user_ctx = nullptr
        };

        httpd_register_uri_handler(handle, &connect);

        return handle;
    }

    return nullptr;
}

esp_err_t ManagerServer::indexHandler(httpd_req_t *req)
{
    ESP_LOGI(m_tag.c_str(), "index.min.html requested");

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_min_html_start, index_min_html_end - index_min_html_start);

    return ESP_OK;
}

esp_err_t ManagerServer::jsHandler(httpd_req_t *req)
{
    ESP_LOGI(m_tag.c_str(), "manager.min.js requested");

    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)app_min_js_start, app_min_js_end - app_min_js_start);

    return ESP_OK;
}

esp_err_t ManagerServer::jqueryHandler(httpd_req_t *req)
{
    ESP_LOGI(m_tag.c_str(), "jquery.min.js requested");

    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)jquery_min_js_start, jquery_min_js_end - jquery_min_js_start);

    return ESP_OK;
}

esp_err_t ManagerServer::cssHandler(httpd_req_t *req)
{
    ESP_LOGI(m_tag.c_str(), "app.min.css requested");

    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)app_min_css_start, app_min_css_end - app_min_css_start);

    return ESP_OK;
}

esp_err_t ManagerServer::faviconHandler(httpd_req_t *req)
{
    ESP_LOGI(m_tag.c_str(), "favicon.ico requested");

    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_end - favicon_ico_start);

    return ESP_OK;
}

esp_err_t ManagerServer::networksHandler(httpd_req_t *req)
{
    ESP_LOGI(m_tag.c_str(), "networks.json requested");

    wifi_scan_config_t scan_config;

    scan_config.ssid = nullptr;
    scan_config.bssid = nullptr;
    scan_config.channel = 0;
    scan_config.show_hidden = true;

    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));

    wifi_ap_record_t wifi_records[HTTP_SERVER_SCAN_MAX_AP];

    uint16_t max_records = HTTP_SERVER_SCAN_MAX_AP;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&max_records, wifi_records));

    cJSON *root = cJSON_CreateObject();
    cJSON *networks = cJSON_CreateArray();

    if ((root == NULL) || (networks == NULL))
    {
        ESP_LOGE(m_tag.c_str(), "Failed to allocate memory for the JSON object");
        return ESP_FAIL;
    }

    for (int i = 0; i < max_records; i++)
    {
        char *ssidptr = reinterpret_cast<char*>(wifi_records[i].ssid);

        if (strlen(ssidptr) == 0)
        {
            continue;
        }

        cJSON *network = cJSON_CreateObject();

        if (network == NULL)
        {
            cJSON_Delete(root);
            ESP_LOGE(m_tag.c_str(), "Failed to allocate memory for the JSON object");
            return ESP_FAIL;
        }

        cJSON_AddStringToObject(network, "ssid", ssidptr);
        cJSON_AddNumberToObject(network, "rssi", wifi_records[i].rssi);
        cJSON_AddItemToArray(networks, network);
    }

    cJSON_AddItemToObject(root, "networks", networks);

    char *json_string = cJSON_PrintUnformatted(root);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_string, HTTPD_RESP_USE_STRLEN);

    free(json_string);
    cJSON_Delete(root);

    return ESP_OK;
}

esp_err_t ManagerServer::connectHandler(httpd_req_t *req)
{
    ESP_LOGI(m_tag.c_str(), "connect.json requested");

    std::string json_str, ssid, pass;

    size_t json_len = httpd_req_get_hdr_value_len(req, "data") + 1;

    if (json_len > 1)
    {
        json_str.reserve(json_len);

        if (httpd_req_get_hdr_value_str(req, "data", json_str.data(), json_len) != ESP_OK)
        {
            ESP_LOGE(m_tag.c_str(), "Failed to get JSON object");
            return ESP_FAIL;
        }
    }

    cJSON *json = cJSON_Parse(json_str.c_str());

    if (json == NULL) 
    {
        ESP_LOGE(m_tag.c_str(), "Error parsing JSON");
        return ESP_FAIL;
    }

    cJSON *ssidObj = cJSON_GetObjectItem(json, "ssid");

    if (cJSON_IsString(ssidObj) && (ssidObj->valuestring != NULL)) 
    {
        ssid = ssidObj->valuestring;
    }

    cJSON *passObj = cJSON_GetObjectItem(json, "pass");

    if (cJSON_IsString(passObj) && (passObj->valuestring != NULL)) 
    {
        pass = passObj->valuestring;
    }

    cJSON_Delete(json);

    if (ssid.empty() || pass.empty())
    {
        ESP_LOGE(m_tag.c_str(), "Failed to obtain SSID or password");
        return ESP_FAIL;
    }

    WiFiStorage storage;
    storage.setMode(1);
    storage.setSSID(ssid);
    storage.setPassword(pass);
    storage.commit();
    storage.close();

    esp_restart();

    return ESP_OK;
}