#ifndef __WIFI_H
#define __WIFI_H

#include <string>

class WiFi 
{
public:
    enum Mode { AP = 0, ST };

    virtual void start() = 0;
    virtual void stop() = 0;

    virtual void setSSID(const std::string &ssid) = 0;
    virtual void setPassword(const std::string &password) = 0;

    virtual bool isconnected() = 0;

    virtual Mode mode() = 0;
};

#endif