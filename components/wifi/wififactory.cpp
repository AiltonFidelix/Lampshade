#include "wififactory.h"
#include "wifiap.h"
#include "wifista.h"

WiFi *WiFiFactory::getMode(WiFiFactory::Mode mode, std::string ssid, std::string pass)
{
    switch (mode)
    {
        case STA:
            static WiFiSTA wifiSTA(ssid, pass);
            return &wifiSTA;
        
        case AP:
            static WiFiAP wifiAP;
            return &wifiAP;
    }

    return nullptr;
}
