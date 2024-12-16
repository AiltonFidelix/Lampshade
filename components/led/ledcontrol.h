#ifndef __LEDCONTROL_H
#define __LEDCONTROL_H

#include <cinttypes>

class LedControl 
{
public:
    enum Mode { Default = 0, Output, PWM };

    virtual void setPin(uint8_t pin) = 0;
    virtual void setMode(Mode mode) = 0;
    virtual void configure() = 0;

    virtual uint8_t pin() = 0;
    virtual Mode mode() = 0;

    virtual void turnOn() = 0;
    virtual void turnOff() = 0;
    virtual void toggle() = 0;
};

#endif