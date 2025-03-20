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

#ifdef CONFIG_HAL_DRIVER_SSD1306
    Driver::Driver(const Configuration& configuration, I2C::Driver& i2c) :
        Base(configuration._width, configuration._height, i2c.getWire(), -1),
        _configuration(configuration)
    {
    }

    bool Driver::begin()
    {
        return Base::begin(SSD1306_SWITCHCAPVCC, _configuration._address);
    }
#else
    Driver::Driver(const Configuration& configuration, I2C::Driver& i2c) :
        _configuration(configuration)
    {
    }

    bool Driver::begin()
    {
        Serial.println("! SSD1306 driver not enabled.");
        return true;
    }
    #endif
}