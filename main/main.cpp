#include "main.h"

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

    WiFi *wifi = WiFiFactory::getMode(WiFiFactory::AP);
    // WiFi *wifi = WiFiFactory::getMode(WiFi::STA, "", "");

    if (wifi == nullptr)
    {
        restart("Failed to create wifi object, restarting...");
    }

    if (!wifi->start())
    {
        restart("Failed to start wifi, restarting...");
    }

    LedEsp32 led;
    led.setPin(GPIO_NUM_8);
    led.setMode(Led::PWM);
    led.configure();

    uint8_t duty = 0;

    while (true)
    {
        duty++;
        led.setDuty(duty);
        duty = (duty > 100) ? 0 : duty;

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
