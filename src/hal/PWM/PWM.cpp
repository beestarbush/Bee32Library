#include "PWM.h"
#include <Arduino.h>

namespace HAL::PWM
{
    Configuration::Configuration(uint8_t pin, uint8_t mode) :
        _pin(pin),
        _mode(mode)
    {

    }

    Driver::Driver(const Configuration& configuration) :
        _configuration(configuration),
        _value(0)
    {
    }

    void Driver::begin()
    {
        pinMode(_configuration._pin, _configuration._mode);
    }

    void Driver::end()
    {
    }

    void Driver::setValue(uint8_t value)
    {
        _value = value;
        analogWrite(_configuration._pin, _value);
    }
}