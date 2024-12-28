#ifndef __LEDESP32_H
#define __LEDESP32_H

#include "interface/led.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include <iostream>

class LedEsp32 : public Led
{
public:
    LedEsp32();
    
    void setPin(uint8_t pin) override;
    void setMode(PinMode mode) override;

    void setDuty(uint8_t duty) override;

    bool configure() override;

    uint8_t pin() override;
    PinMode mode() override;

    void turnOn() override;
    void turnOff() override;
    void toggle() override;

private:
    bool configureOutputMode();
    bool configurePWMMode();

    uint32_t calcDuty(uint8_t duty);

private:
    gpio_num_t m_pin;
    PinMode m_pinMode;
    ledc_mode_t m_ledMode;
    ledc_channel_t m_ledChannel;
    ledc_timer_t m_ledTimer;
    ledc_timer_bit_t m_ledTimerResolution;
    uint32_t m_ledTimerFreq;
};

#endif