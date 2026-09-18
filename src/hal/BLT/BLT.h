#ifndef _HAL_BLT_H
#define _HAL_BLT_H

#include "Bee32Config.h"

#if CONFIG_HAL_DRIVER_BLUETOOTH

namespace HAL::BLT
{
    class Driver
    {
    public:
        Driver();
        ~Driver() = default;

        void begin();
        void end();
        bool started();
    };
}

#endif

#endif // _HAL_BLT_H