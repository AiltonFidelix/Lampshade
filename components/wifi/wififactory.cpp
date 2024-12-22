#include "wififactory.h"
#include "wifiap.h"

WiFi *WiFiFactory::getMode(WiFi::Mode mode, std::string ssid, std::string pass)
{
    switch (mode)
    {
        case WiFi::Mode::ST:
            /* code */
            break;
        
        default:
            return new WiFiAP();
            break;
    }

    return nullptr;
}
