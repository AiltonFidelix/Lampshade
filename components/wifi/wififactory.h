#ifndef __WIFIFACTORY_H
#define __WIFIFACTORY_H

#include "interface/wifi.h"

class WiFiFactory
{
public:
    enum Mode { AP = 0, STA };

    WiFiFactory() = default;
    ~WiFiFactory() = default;

    static WiFi *getMode(Mode mode, const std::string &ssid, const std::string &pass);
};

#endif