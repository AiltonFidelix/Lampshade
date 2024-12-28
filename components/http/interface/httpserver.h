#ifndef __HTTPSERVER_H
#define __HTTPSERVER_H

class HttpServer
{
public:
    virtual bool start() = 0;
    virtual void stop() = 0;
};

#endif