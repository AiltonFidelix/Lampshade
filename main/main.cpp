#include "main.h"

#define WAIT_FOREVER while (true) { vTaskDelay(pdMS_TO_TICKS(10)); }

void app_main(void)
{
    std::string tag = "main";

    auto restart = [&tag](std::string message) -> void
    {
        message += '\0';

        ESP_LOGE(tag.c_str(), "%s", message.c_str());
        esp_restart();
    };

    auto checkNVS = []() -> void
    {
        esp_err_t ret = nvs_flash_init();

        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
        {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }

        ESP_ERROR_CHECK(ret);
    };

    checkNVS();
    
    WiFiStorage storage;

    WiFiFactory::Mode mode = static_cast<WiFiFactory::Mode>(storage.getMode());
    
    std::string ssid = storage.getSSID();
    std::string pass = storage.getPassword();

    LedEsp32 led;
    led.setPin(GPIO_NUM_8);
    led.setMode(Led::PWM);
    led.configure();
    led.startBlink(80);

    WiFi *wifi = WiFiFactory::getMode(mode, ssid, pass);

    if (wifi == nullptr)
    {
        restart("Failed to create wifi object, restarting...");
    }

    if (!wifi->start())
    {
        storage.setMode(static_cast<uint8_t>(WiFiFactory::AP));
        restart("Failed to start wifi, restarting...");
    }

    led.stopBlink();

    HttpServer *server = ServerFactory::getServer(mode);

    if (server == nullptr)
    {
        restart("Failed to create http server object, restarting...");
    }

    server->setLed(&led);

    if (!server->start())
    {
        restart("Failed to start http server, restarting...");
    }

    WAIT_FOREVER;
}
