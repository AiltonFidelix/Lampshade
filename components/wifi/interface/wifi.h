#ifndef __WIFI_H
#define __WIFI_H

#include <string>

class WiFi 
{
public:
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual void setCredentials(const std::string &ssid, const std::string &password) = 0;
    virtual bool isconnected() = 0;
};

#endif