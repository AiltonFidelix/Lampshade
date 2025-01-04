#include "ledesp32.h"
#include <cmath>

LedEsp32::LedEsp32()
{
    m_taskHandle = nullptr;

    m_pinMode = Output;

    // PWM default settings
    m_ledMode = LEDC_LOW_SPEED_MODE;
    m_ledChannel = LEDC_CHANNEL_0;
    m_ledTimer = LEDC_TIMER_0;
    m_ledTimerResolution = LEDC_TIMER_13_BIT;
    m_ledTimerFreq = 5000;                      // 5Khz
}

void LedEsp32::setPin(uint8_t pin)
{
    m_pin = static_cast<gpio_num_t>(pin);
}

void LedEsp32::setMode(PinMode mode)
{
    m_pinMode = mode;
}

void LedEsp32::setDuty(uint8_t duty)
{
    ledc_set_duty(m_ledMode, m_ledChannel, calcDuty(duty));
    ledc_update_duty(m_ledMode, m_ledChannel);

    m_duty = duty;
}

uint8_t LedEsp32::duty()
{
    return m_duty;
}

bool LedEsp32::configure()
{
    switch (m_pinMode)
    {
        case Output:
            return configureOutputMode();
        case PWM:
            return configurePWMMode();
        default:
            return configureOutputMode();
    }
}

uint8_t LedEsp32::pin()
{
    return m_pin;
}

Led::PinMode LedEsp32::mode()
{
    return m_pinMode;
}

void LedEsp32::turnOn()
{
    if (m_pinMode == PWM)
    {
        setDuty(100);
    }
    else
    {
        gpio_set_level(m_pin, 0);
    }
}

void LedEsp32::turnOff()
{
    if (m_pinMode == PWM)
    {
        setDuty(0);
    }
    else
    {
        gpio_set_level(m_pin, 1);
    }
}

void LedEsp32::toggle()
{
    if (m_pinMode == PWM)
    {
        uint8_t duty = m_duty == 0 ? 100 : 0;
        setDuty(duty);
    }
    else
    {
        uint8_t level = gpio_get_level(m_pin);
        level ^= 1;
        gpio_set_level(m_pin, level);
    }
}

void LedEsp32::startBlink(uint32_t period)
{
    m_blink.led = this;
    m_blink.period = period;

    xTaskCreate(LedEsp32::blinkTask, "blinkTask", configMINIMAL_STACK_SIZE, &m_blink, 1, &m_taskHandle);
}

void LedEsp32::stopBlink()
{
    if (m_taskHandle)
    {
        vTaskDelete(m_taskHandle);

        m_taskHandle = nullptr;
    }
}

bool LedEsp32::configureOutputMode()
{
    if (gpio_set_direction(m_pin, GPIO_MODE_INPUT_OUTPUT) != ESP_OK)
    {
        return false;
    } 

    turnOff();

    return true;
}

bool LedEsp32::configurePWMMode()
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode      = m_ledMode,
        .duty_resolution = m_ledTimerResolution,
        .timer_num       = m_ledTimer,            
        .freq_hz         = m_ledTimerFreq,
        .clk_cfg         = LEDC_AUTO_CLK,
        .deconfigure     = false
    };

    if (ledc_timer_config(&ledc_timer) != ESP_OK)
    {   
        return false;
    }

    ledc_channel_config_t ledc_channel = {
        .gpio_num   = m_pin,
        .speed_mode = m_ledMode,
        .channel    = m_ledChannel,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = m_ledTimer,
        .duty       = 0, 
        .hpoint     = 0,
        .flags      = { 0 }
    };

    if (ledc_channel_config(&ledc_channel) != ESP_OK)
    {
        return false;
    }

    setDuty(0);

    return true;
}

uint32_t LedEsp32::calcDuty(uint8_t duty)
{
    uint8_t tmp = duty > 100 ? 100 : duty;
    uint8_t final = 100 - tmp;

    uint32_t range = std::pow(2.0, m_ledTimerResolution);
    float multiplier = static_cast<float>(final / 100.0);

    return static_cast<uint32_t>(range * multiplier);
}

void LedEsp32::blinkTask(void *pvParameters)
{
    BlinkSettings *blink = static_cast<BlinkSettings*>(pvParameters);

    if (blink == nullptr)
    {
        vTaskDelete(nullptr);
    }

    while (true)
    {
        blink->led->toggle();

        vTaskDelay(pdMS_TO_TICKS(blink->period));
    }
}
