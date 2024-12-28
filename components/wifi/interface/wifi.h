#ifndef __WIFI_H
#define __WIFI_H

#include <string>

#define WIFI_TASK_STACK 4096
#define WIFI_MAX_RETRY 5
#define WIFI_SSID_LEN 34
#define WIFI_PASS_LEN 64

class WiFi 
{
public:
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual void setCredentials(const std::string &ssid, const std::string &password) = 0;
    virtual bool isconnected() = 0;
};

#endif