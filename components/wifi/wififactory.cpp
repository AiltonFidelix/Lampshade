#include "wififactory.h"
#include "wifiap.h"
#include "wifista.h"

WiFi *WiFiFactory::getMode(WiFi::Mode mode, std::string ssid, std::string pass)
{
    switch (mode)
    {
        case WiFi::Mode::STA:
            return new WiFiSTA(ssid, pass);
            break;
        
        case WiFi::Mode::AP:
            return new WiFiAP();
            break;
    }

    return nullptr;
}
