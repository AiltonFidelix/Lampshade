#ifndef __SERVERFACTORY_H
#define __SERVERFACTORY_H

#include "interface/httpserver.h"

class ServerFactory
{
public:

    ServerFactory() = default;
    ~ServerFactory() = default;

    static HttpServer *getServer(uint8_t mode);
};

#endif