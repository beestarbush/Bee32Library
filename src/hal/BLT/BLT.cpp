#include "BLT.h"

#if CONFIG_HAL_DRIVER_BLUETOOTH

#include "esp32-hal-bt.h"

namespace HAL::BLT
{
    Driver::Driver()
    {
    }

    void Driver::begin()
    {
        btStart();
    }

    void Driver::end()
    {
        btStop();
    }

    bool Driver::started()
    {
        return btStarted();
    }
}

#endif