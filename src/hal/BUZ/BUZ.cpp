#include "BUZ.h"
#include <Arduino.h>

namespace HAL::BUZ
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

    void Driver::play(Note note, uint32_t duration)
    {
        uint32_t noteDuration = 1000 / duration;
        tone(_configuration._pin, note, noteDuration);
    }

    void Driver::stop()
    {
        noTone(_configuration._pin);
    }
}