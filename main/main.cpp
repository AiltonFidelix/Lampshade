#include "main.h"
#include "ledesp32.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    LedEsp32 led;
    led.setPin(GPIO_NUM_8);
    led.setMode(LedControl::PWM);
    // led.setMode(LedControl::Output);
    led.configure();
    

    ESP_LOGI("MAIN", "Hello from esp32c3 RISC-V");
    uint8_t duty = 0;

    while (true)
    {
        duty++;
        led.setDuty(duty);
        duty = (duty > 100) ? 0 : duty;

        vTaskDelay(pdMS_TO_TICKS(80));
    }
}
