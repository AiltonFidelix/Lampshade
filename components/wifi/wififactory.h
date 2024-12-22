#ifndef __WIFIFACTORY_H
#define __WIFIFACTORY_H

#include "wifi.h"

class WiFiFactory
{
public:
    WiFiFactory() = default;
    ~WiFiFactory() = default;

    static WiFi *getMode(WiFi::Mode mode, std::string ssid = "", std::string pass = "");
};

#endif