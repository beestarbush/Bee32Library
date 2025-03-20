#include "EINK.h"
#include <Arduino.h>

namespace HAL::EINK
{
    Configuration::Configuration(uint16_t width, uint16_t height, uint8_t address, uint8_t cs, uint8_t dc, uint8_t rst, uint8_t busy) :
        _width(width),
        _height(height),
        _address(address),
        _cs(cs),
        _dc(dc),
        _rst(rst),
        _busy(busy)
    {
    }

    Driver::Driver(const Configuration& configuration, SPIClass& spi) :
        Base(GxEPD2_290_C90c(configuration._cs, configuration._dc, configuration._rst, configuration._busy)),
        _configuration(configuration),
        _hspi(spi)
    {
    }

    bool Driver::begin()
    {
        _hspi.begin(13, 12, 14, 15); // remap hspi for EPD (swap pins)
        Base::epd2.selectSPI(_hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
        Base::init(115200);
        return true;
    }
}