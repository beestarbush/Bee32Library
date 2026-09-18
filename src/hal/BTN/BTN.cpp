#include "BTN.h"
#include <Arduino.h>

namespace HAL::BTN
{
    Driver::Driver(button_config_t cfg) :
        _cfg(cfg),
        _pressed(false)
    {
    }

    void Driver::begin()
    {
        pinMode(_cfg.pin, _cfg.mode);
    }

    void Driver::run()
    {
        _pressed = !digitalRead(_cfg.pin);
    }

    bool Driver::getPressed()
    {
        return _pressed;
    }
}