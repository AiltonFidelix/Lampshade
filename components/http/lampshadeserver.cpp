#include "lampshadeserver.h"
#include "httpconfig.h"

#include "cJSON.h"

extern const uint8_t index_min_html_start[] asm("_binary_lampshade_min_html_start");
extern const uint8_t index_min_html_end[] asm("_binary_lampshade_min_html_end");

extern const uint8_t app_min_js_start[] asm("_binary_lampshade_min_js_start");
extern const uint8_t app_min_js_end[] asm("_binary_lampshade_min_js_end");

extern const uint8_t jquery_min_js_start[] asm("_binary_jquery_min_js_start");
extern const uint8_t jquery_min_js_end[] asm("_binary_jquery_min_js_end");

extern const uint8_t app_min_css_start[] asm("_binary_app_min_css_start");
extern const uint8_t app_min_css_end[] asm("_binary_app_min_css_end");

extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start");
extern const uint8_t favicon_ico_end[] asm("_binary_favicon_ico_end");

std::string LampshadeServer::m_tag = "lampshadeserver";

Led *LampshadeServer::m_led = nullptr;

LampshadeServer::LampshadeServer()
{
    m_serverHandle = nullptr;
}

LampshadeServer::~LampshadeServer()
{
    stop();
}

bool LampshadeServer::start()
{
    ESP_LOGI(m_tag.c_str(), "Initializing lampshade server...");

    m_serverHandle = serverConfigure();

    if (m_serverHandle == nullptr)
    {
        ESP_LOGE(m_tag.c_str(), "Lampshade server initialization failed...");

        return false;
    }

    if (m_led)
    {
        m_led->setDuty(100);
    }

    return true;
}

void LampshadeServer::stop()
{
}

void LampshadeServer::setLed(Led *led)
{
    if (led == nullptr)
    {
        return;
    }

    m_led = led;
    m_led->turnOn();
}

httpd_handle_t LampshadeServer::serverConfigure()
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
            .handler = LampshadeServer::indexHandler,
            .user_ctx = nullptr
        };

        httpd_register_uri_handler(handle, &index);

        httpd_uri_t app_js = {
            .uri = "/js/lampshade.min.js",
            .method = HTTP_GET,
            .handler = LampshadeServer::jsHandler,
            .user_ctx = nullptr
        };

        httpd_register_uri_handler(handle, &app_js);

        httpd_uri_t jquery = {
            .uri = "/js/jquery.min.js",
            .method = HTTP_GET,
            .handler = LampshadeServer::jqueryHandler,
            .user_ctx = nullptr
        };

        httpd_register_uri_handler(handle, &jquery);

        httpd_uri_t app_css = {
            .uri = "/css/app.min.css",
            .method = HTTP_GET,
            .handler = LampshadeServer::cssHandler,
            .user_ctx = nullptr
        };

        httpd_register_uri_handler(handle, &app_css);

        httpd_uri_t favicon = {
            .uri = "/assets/favicon.ico",
            .method = HTTP_GET,
            .handler = LampshadeServer::faviconHandler,
            .user_ctx = nullptr
        };

        httpd_register_uri_handler(handle, &favicon);

        httpd_uri_t setduty = {
            .uri = "/setduty.json",
            .method = HTTP_POST,
            .handler = LampshadeServer::setDutyHandler,
            .user_ctx = nullptr
        };

        httpd_register_uri_handler(handle, &setduty);

        httpd_uri_t getduty = {
            .uri = "/getduty.json",
            .method = HTTP_GET,
            .handler = LampshadeServer::getDutyHandler,
            .user_ctx = nullptr
        };

        httpd_register_uri_handler(handle, &getduty);

        return handle;
    }

    return nullptr;
}

esp_err_t LampshadeServer::indexHandler(httpd_req_t *req)
{
    ESP_LOGI(m_tag.c_str(), "lampshade.min.html requested");

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_min_html_start, index_min_html_end - index_min_html_start);

    return ESP_OK;
}

esp_err_t LampshadeServer::jsHandler(httpd_req_t *req)
{
    ESP_LOGI(m_tag.c_str(), "lampshade.min.js requested");

    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)app_min_js_start, app_min_js_end - app_min_js_start);

    return ESP_OK;
}

esp_err_t LampshadeServer::jqueryHandler(httpd_req_t *req)
{
    ESP_LOGI(m_tag.c_str(), "jquery.min.js requested");

    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)jquery_min_js_start, jquery_min_js_end - jquery_min_js_start);

    return ESP_OK;
}

esp_err_t LampshadeServer::cssHandler(httpd_req_t *req)
{
    ESP_LOGI(m_tag.c_str(), "app.min.css requested");

    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)app_min_css_start, app_min_css_end - app_min_css_start);

    return ESP_OK;
}

esp_err_t LampshadeServer::faviconHandler(httpd_req_t *req)
{
    ESP_LOGI(m_tag.c_str(), "favicon.ico requested");

    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_end - favicon_ico_start);

    return ESP_OK;
}

esp_err_t LampshadeServer::setDutyHandler(httpd_req_t *req)
{
    ESP_LOGI(m_tag.c_str(), "setduty.json requested");

    std::string json_str;

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

    cJSON *duty = cJSON_GetObjectItem(json, "duty");

    if (cJSON_IsNumber(duty)) 
    {
        m_led->setDuty(duty->valueint);
        ESP_LOGI(m_tag.c_str(), "New duty %d", duty->valueint);
    }

    cJSON_Delete(json);

    return ESP_OK;
}

esp_err_t LampshadeServer::getDutyHandler(httpd_req_t *req)
{
    ESP_LOGI(m_tag.c_str(), "getduty.json requested");

    cJSON *root = cJSON_CreateObject();

    if (root == NULL)
    {
        ESP_LOGE(m_tag.c_str(), "Failed to allocate memory for the JSON object");
        return ESP_FAIL;
    }

    cJSON_AddNumberToObject(root, "duty", m_led->duty());

    char *json_string = cJSON_PrintUnformatted(root);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_string, HTTPD_RESP_USE_STRLEN);

    free(json_string);
    cJSON_Delete(root);

    return ESP_OK;
}
