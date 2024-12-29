#ifndef __HTTPSERVER_H
#define __HTTPSERVER_H

#include "../led/interface/led.h"

class HttpServer
{
public:
    virtual bool start() = 0;
    virtual void stop() = 0;

    virtual void setLed(Led *led) = 0;
};

#endif