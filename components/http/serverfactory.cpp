#include "serverfactory.h"
#include "managerserver.h"
#include "lampshadeserver.h"

HttpServer *ServerFactory::getServer(uint8_t mode)
{   
    switch (mode)
    {
        case 1:
            static LampshadeServer lampshade;
            return &lampshade;
        
        default:
            static ManagerServer manager;
            return &manager;
    }

    return nullptr;
}
