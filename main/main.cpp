#include "main.h"
#include "ledesp32.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    LedEsp32 led(GPIO_NUM_8, LedControl::Output);
    led.configure();

    ESP_LOGI("MAIN", "Hello from esp32c3 RISC-V");

    while (true)
    {
        led.toggle();

        vTaskDelay(pdMS_TO_TICKS(80));
    }
}
