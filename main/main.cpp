#include "main.h"
#include "managerserver.h"

#define WAIT_FOREVER while (true) { vTaskDelay(pdMS_TO_TICKS(10)); }

void app_main(void)
{
    auto restart = [](std::string message) -> void
    {
        message += '\0';

        ESP_LOGE("main", "%s", message.c_str());
        esp_restart();
    };

    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    LedEsp32 led;
    led.setPin(GPIO_NUM_8);
    led.setMode(Led::Output);
    led.configure();

    WiFi *wifi = WiFiFactory::getMode(WiFiFactory::AP);
    // WiFi *wifi = WiFiFactory::getMode(WiFiFactory::STA, "", "");

    if (wifi == nullptr)
    {
        restart("Failed to create wifi object, restarting...");
    }

    if (!wifi->start())
    {
        restart("Failed to start wifi, restarting...");
    }

    ManagerServer server;
    server.setLed(&led);
    server.start();

    WAIT_FOREVER;
}
