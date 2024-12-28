#ifndef __WIFIFACTORY_H
#define __WIFIFACTORY_H

#include "interface/wifi.h"

class WiFiFactory
{
public:
    enum Mode { AP = 0, STA };

    WiFiFactory() = default;
    ~WiFiFactory() = default;

    static WiFi *getMode(Mode mode, std::string ssid = "", std::string pass = "");
};

#endif