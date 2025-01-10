#ifndef __STORAGE_H
#define __STORAGE_H

#include <string>

#include "esp_log.h"
#include "nvs_flash.h"

class WiFiStorage
{
public:
    WiFiStorage();
    ~WiFiStorage();

    void setMode(uint8_t mode);
    uint8_t getMode();

    void setSSID(const std::string &ssid);
    std::string getSSID();

    void setPassword(const std::string &password);
    std::string getPassword();

    bool commit();
    void close();

private:
    bool saveString(const std::string &key, const std::string &data);
    bool loadString(const std::string &key, std::string &data);

    bool saveU8(const std::string &key, uint8_t value);
    uint8_t loadU8(const std::string &key);

private:
    std::string m_tag;
    std::string m_namespace;
    nvs_handle_t m_handle;
};

#endif