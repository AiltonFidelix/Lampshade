#ifndef __LEDCONTROL_H
#define __LEDCONTROL_H

#include <cinttypes>

class Led
{
public:
    enum PinMode { Output = 0, PWM };

    virtual void setPin(uint8_t pin) = 0;
    virtual void setMode(PinMode mode) = 0;

    virtual void setDuty(uint8_t duty) = 0;
    virtual uint8_t duty() = 0;

    virtual bool configure() = 0;

    virtual uint8_t pin() = 0;
    virtual PinMode mode() = 0;

    virtual void turnOn() = 0;
    virtual void turnOff() = 0;
    virtual void toggle() = 0;

    virtual void startBlink(uint32_t period) = 0;
    virtual void stopBlink() = 0;
};

#endif