#include "wifistorage.h"

WiFiStorage::WiFiStorage()
{
    m_tag = "WiFiStorage";
    m_namespace = "wifi";

    esp_err_t ret = nvs_open(m_namespace.c_str(), NVS_READWRITE, &m_handle);

    if (ret != ESP_OK)
    {
        ESP_LOGE(m_tag.c_str(), "Failed to open nvs");
    }
}

WiFiStorage::~WiFiStorage()
{
    commit();
    close();
}

void WiFiStorage::setMode(uint8_t mode)
{
    std::string key = "mode";
    saveU8(key, mode);
}

uint8_t WiFiStorage::getMode()
{
    std::string key = "mode";
    return loadU8(key);
}

void WiFiStorage::setSSID(const std::string &ssid)
{
    std::string key = "ssid";
    saveString(key, ssid);
}

std::string WiFiStorage::getSSID()
{
    std::string key = "ssid";
    std::string ssid;

    if (!loadString(key, ssid))
    {
        return "unknown";
    }

    return ssid;
}

void WiFiStorage::setPassword(const std::string &password)
{
    std::string key = "pass";
    saveString(key, password);
}

std::string WiFiStorage::getPassword()
{
    std::string key = "pass";
    std::string pass;

    if (!loadString(key, pass))
    {
        return "unknown";
    }

    return pass;
}

bool WiFiStorage::commit()
{
    esp_err_t ret = nvs_commit(m_handle);

    if (ret != ESP_OK)
    {
        ESP_LOGE(m_tag.c_str(), "Commit failed");
        return false;
    }

    return true;
}

void WiFiStorage::close()
{
    nvs_close(m_handle);
}

bool WiFiStorage::saveString(const std::string &key, const std::string &data)
{
    if (key.empty() || data.empty())
    {
        ESP_LOGE(m_tag.c_str(), "Failed to save %s: %s", key.c_str(), data.c_str());
        return false; 
    }

    std::string lenKey = key + "_size";

    if (!saveU8(lenKey, data.length() + 1))
    {
        ESP_LOGE(m_tag.c_str(), "Failed to save %s len", key.c_str());
        return false;
    }

    esp_err_t ret = nvs_set_str(m_handle, key.c_str(), data.c_str());

    if (ret != ESP_OK)
    {
        ESP_LOGE(m_tag.c_str(), "Failed to save %s: %s", key.c_str(), esp_err_to_name(ret));
        return false;
    }

    return true;
}

bool WiFiStorage::loadString(const std::string &key, std::string &data)
{
    if (key.empty())
    {
        return false; 
    }

    std::string lenKey = key + "_size";

    size_t required_size = loadU8(lenKey);
    char value[required_size];

    nvs_type_t type = NVS_TYPE_STR;

    esp_err_t ret = nvs_find_key(m_handle, key.c_str(), &type);

    if (ret != ESP_OK)
    {
        ESP_LOGE(m_tag.c_str(), "Key '%s' not found in '%s' namespace", key.c_str(), m_namespace.c_str());
        return false;
    }

    ret = nvs_get_str(m_handle, key.c_str(), value, &required_size);

    if (ret != ESP_OK)
    {
        ESP_LOGE(m_tag.c_str(), "Failed to load %s in '%s' namespace: %s", key.c_str(), m_namespace.c_str(), esp_err_to_name(ret));
        return false;
    }

    data = value;

    return true;
}

bool WiFiStorage::saveU8(const std::string &key, uint8_t value)
{
    esp_err_t ret = nvs_set_u8(m_handle, key.c_str(), value);

    if (ret != ESP_OK)
    {
        ESP_LOGE(m_tag.c_str(), "Failed to save %s", key.c_str());
    }

    return (ret == ESP_OK);
}

uint8_t WiFiStorage::loadU8(const std::string &key)
{
    uint8_t value = 0;
  
    esp_err_t ret = nvs_get_u8(m_handle, key.c_str(), &value);

    if (ret != ESP_OK)
    {
        ESP_LOGE(m_tag.c_str(), "Failed to load %s", key.c_str());
    }

    return value;
}
