#include "lampshadeserver.h"

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
}

bool LampshadeServer::start()
{
    ESP_LOGI(m_tag.c_str(), "Initializing lampshade server...");

    if (m_serverHandle == nullptr)
    {
        m_serverHandle = serverConfigure();
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
}

httpd_handle_t LampshadeServer::serverConfigure()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.task_priority = 3;
    config.stack_size = 4096;

    // Increase uri handlers
    config.max_uri_handlers = 20;

    // Increase the timeout limits
    config.recv_wait_timeout = 10;
    config.send_wait_timeout = 10;

    if (httpd_start(&m_serverHandle, &config) == ESP_OK)
    {
        ESP_LOGI(m_tag.c_str(), "Registering URI handlers...");

        httpd_uri_t index = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = LampshadeServer::indexHandler,
            .user_ctx = nullptr};
        httpd_register_uri_handler(m_serverHandle, &index);

        httpd_uri_t app_js = {
            .uri = "/js/brightness.min.js",
            .method = HTTP_GET,
            .handler = LampshadeServer::jsHandler,
            .user_ctx = nullptr};
        httpd_register_uri_handler(m_serverHandle, &app_js);

        httpd_uri_t jquery = {
            .uri = "/js/jquery.min.js",
            .method = HTTP_GET,
            .handler = LampshadeServer::jqueryHandler,
            .user_ctx = nullptr};
        httpd_register_uri_handler(m_serverHandle, &jquery);

        httpd_uri_t app_css = {
            .uri = "/css/app.min.css",
            .method = HTTP_GET,
            .handler = LampshadeServer::cssHandler,
            .user_ctx = nullptr};
        httpd_register_uri_handler(m_serverHandle, &app_css);

        httpd_uri_t favicon = {
            .uri = "/assets/favicon.ico",
            .method = HTTP_GET,
            .handler = LampshadeServer::faviconHandler,
            .user_ctx = nullptr};
        httpd_register_uri_handler(m_serverHandle, &favicon);

        return m_serverHandle;
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
    ESP_LOGI(m_tag.c_str(), "jquery-3.7.1.min.js requested");

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
