#include "main.h"

#include "esp_log.h"
#include "nvs_flash.h"

#include "wififactory.h"
#include "ledesp32.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    WiFi *wifi = WiFiFactory::getMode(WiFi::AP);
    // WiFi *wifi = WiFiFactory::getMode(WiFi::STA, "", "");

    if (wifi)
    {
        wifi->start();
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
