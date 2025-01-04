#ifndef __STORAGE_H
#define __STORAGE_H

#include <string>
#include "wififactory.h"

class Storage
{
public:
    Storage();
    ~Storage();

    void setWiFiMode(WiFiFactory::Mode mode);
    void setWiFiSSID(const std::string &ssid);
    void setWiFiPassword(const std::string & password);
};

#endif