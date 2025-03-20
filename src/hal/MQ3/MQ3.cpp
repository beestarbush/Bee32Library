#include "MQ3.h"
#include <Arduino.h>

namespace HAL::MQ3
{
    Configuration::Configuration(uint8_t pin, uint8_t mode) :
        _pin(pin),
        _mode(mode)
    {
    }

    Driver::Driver(const Configuration& configuration) :
        _configuration(configuration)
    {
    }

    void Driver::begin()
    {
        pinMode(_configuration._pin, _configuration._mode);
    }

    void Driver::end()
    {
    }

    uint16_t Driver::readAdc()
    {
        return analogRead(_configuration._pin);
    }

    double Driver::readVoltage()
    {
        uint16_t adcValue = readAdc();
        return (adcValue / 4095.0) * 3.3;
    }
}