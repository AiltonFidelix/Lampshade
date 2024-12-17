#ifndef __LEDCONTROL_H
#define __LEDCONTROL_H

#include <cinttypes>

class LedControl 
{
public:
    enum PinMode { Default = 0, Output, PWM };

    virtual void setPin(uint8_t pin) = 0;
    virtual void setMode(PinMode mode) = 0;

    virtual void setDuty(uint8_t duty) = 0;

    virtual bool configure() = 0;

    virtual uint8_t pin() = 0;
    virtual PinMode mode() = 0;

    virtual void turnOn() = 0;
    virtual void turnOff() = 0;
    virtual void toggle() = 0;
};

#endif