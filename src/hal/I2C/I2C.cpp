#include "I2C.h"
#include <Wire.h>

namespace HAL::I2C
{
    Configuration::Configuration(uint8_t bus, uint8_t clockPin, uint8_t dataPin) :
        _bus(bus),
        _clockPin(clockPin),
        _dataPin(dataPin)
    {    
    }

    Driver::Driver(const Configuration& configuration) :
        _configuration(configuration),
        _wire(_configuration._bus)
    {
    }

    void Driver::begin()
    {
        _wire.setPins(_configuration._dataPin, _configuration._clockPin);
        _wire.begin();
    }

    void Driver::end()
    {
        _wire.endTransmission();
        _wire.end();
    }

    TwoWire* Driver::getWire()
    {
        return &_wire;
    }
}