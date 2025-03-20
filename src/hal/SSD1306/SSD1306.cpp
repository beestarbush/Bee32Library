#include "SSD1306.h"
#include <Arduino.h>

namespace HAL::SSD1306
{
    Configuration::Configuration(uint8_t width, uint8_t height, uint8_t address) :
        _width(width),
        _height(height),
        _address(address)
    {
    }

    Driver::Driver(const Configuration& configuration, I2C::Driver& i2c) :
        Base(configuration._width, configuration._height, i2c.getWire(), -1),
        _configuration(configuration)
    {
    }

    bool Driver::begin()
    {
        return Base::begin(SSD1306_SWITCHCAPVCC, _configuration._address);
    }
}