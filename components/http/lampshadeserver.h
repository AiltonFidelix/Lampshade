#ifndef __LAMPSHADESERVER_H
#define __LAMPSHADESERVER_H

#include "interface/httpserver.h"

#include "esp_log.h"
#include "esp_http_server.h"

#include <string>

class LampshadeServer : public HttpServer
{
public:
    LampshadeServer();
    ~LampshadeServer();

    bool start() override;
    void stop() override;

    void setLed(Led *led) override;

private:
    httpd_handle_t serverConfigure();

    static esp_err_t indexHandler(httpd_req_t *req);
    static esp_err_t jsHandler(httpd_req_t *req);
    static esp_err_t jqueryHandler(httpd_req_t *req);
    static esp_err_t cssHandler(httpd_req_t *req);
    static esp_err_t faviconHandler(httpd_req_t *req);

private:
    static std::string m_tag;
    static Led *m_led;

    httpd_handle_t m_serverHandle;
};

#endif