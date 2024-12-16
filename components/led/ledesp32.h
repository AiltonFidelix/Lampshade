#ifndef __LEDESP32_H
#define __LEDESP32_H

#include "ledcontrol.h"
#include "driver/gpio.h"
#include <iostream>

class LedEsp32 : public LedControl
{
public:
    LedEsp32();
    LedEsp32(uint8_t pin, Mode mode);
    
    void setPin(uint8_t pin) override;
    void setMode(Mode mode) override;

    void configure() override;

    uint8_t pin() override;
    Mode mode() override;

    void turnOn() override;
    void turnOff() override;
    void toggle() override;

private:
    void configureOutputMode();
    void configurePWMMode();

private:
    std::string m_tag;
    gpio_num_t m_pin;
    Mode m_mode;
};

#endif