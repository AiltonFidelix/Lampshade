#include "ledesp32.h"

#include "esp_log.h"

LedEsp32::LedEsp32()
{
    m_tag = "LedEsp32";
}

LedEsp32::LedEsp32(uint8_t pin, Mode mode)
{
    m_tag = "LedEsp32";

    m_pin = static_cast<gpio_num_t>(pin);
    m_mode = mode;

    ESP_LOGI(m_tag.c_str(), "Setting pin %d, mode %d", pin, mode);
}

void LedEsp32::setPin(uint8_t pin)
{
    ESP_LOGI(m_tag.c_str(), "Setting pin %d", pin);
    m_pin = static_cast<gpio_num_t>(pin);
}

void LedEsp32::setMode(Mode mode)
{
    ESP_LOGI(m_tag.c_str(), "Setting mode %d", mode);
    m_mode = mode;
}

void LedEsp32::configure()
{
    ESP_LOGI(m_tag.c_str(), "Configuring...");

    switch (m_mode)
    {
        case Default:
        case Output:
            configureOutputMode();
            break;
        case PWM:
            configurePWMMode();
            break;
        
        default:
            configureOutputMode();
            break;
    }
}

uint8_t LedEsp32::pin()
{
    return m_pin;
}

LedControl::Mode LedEsp32::mode()
{
    return m_mode;
}

void LedEsp32::turnOn()
{
    gpio_set_level(m_pin, 1);
}

void LedEsp32::turnOff()
{
    gpio_set_level(m_pin, 0);
}

void LedEsp32::toggle()
{
    uint8_t level = gpio_get_level(m_pin);
    level ^= 1;
    gpio_set_level(m_pin, level);
}

void LedEsp32::configureOutputMode()
{
    gpio_set_direction(m_pin, GPIO_MODE_INPUT_OUTPUT);   
    turnOff();
}

void LedEsp32::configurePWMMode()
{
}
