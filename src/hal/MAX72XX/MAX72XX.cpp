#ifdef CONFIG_HAL_DRIVER_MAX72XX
#include "MAX72XX.h"

constexpr uint8_t MAX7219_DEFAULT_ADDRESS = 0;
constexpr uint64_t UPDATE_INTERVAL = 500;

namespace HAL::MAX72XX
{
    Configuration::Configuration(uint8_t mosi, uint8_t clk, uint8_t cs, uint8_t cascadedDevices, uint8_t intensity, uint8_t connectedDigits) :
        _mosiPin(mosi),
        _clkPin(clk),
        _csPin(cs),
        _cascadedDevices(cascadedDevices),
        _intensity(intensity),
        _connectedDigits(connectedDigits)
    {    
    }
    
    Driver::Driver(const Configuration& configuration) :
        _max7219(),
        _configuration(configuration),
        _updateInterval(0)
    {
    }

    void Driver::begin()
    {
        _max7219.init(_configuration._mosiPin, _configuration._clkPin, _configuration._csPin, _configuration._cascadedDevices);
        _max7219.activateAllSegments();
        _max7219.setIntensity(_configuration._intensity);

        // Do not set scan limit as this gives a different current, and thus different intensity of the LEDs.
        //_max7219.setScanLimit(MAX7219_DEFAULT_ADDRESS, _configuration.connectedDigits - 1);
    }

    void Driver::end()
    {
    }

    void Driver::run()
    {
        // Periodically update the segments on this chip.
        uint64_t now = millis();
        if (now - _updateInterval < UPDATE_INTERVAL)
        {
            return;
        }

        _updateInterval = now;

        _max7219.refreshSegments();
    }

    void Driver::setDigit(uint8_t digit, uint8_t value, bool decimalPoint)
    {
        _max7219.setDigit(MAX7219_DEFAULT_ADDRESS, digit, value, decimalPoint);
    }

    void Driver::setChar(uint8_t digit, char character, bool decimalPoint)
    {
        _max7219.setChar(MAX7219_DEFAULT_ADDRESS, digit, character, decimalPoint);
    }

    uint8_t Driver::getConnectedDigits()
    {
        return _configuration._connectedDigits;
    }

    void Driver::setBrightness(uint8_t brightness)
    {
        if (brightness > 15)
        {
            brightness = 15;
        }
        _max7219.setIntensity(brightness);
    }

    void Driver::clearAll()
    {
        for (uint8_t i = 0; i < getConnectedDigits(); i++)
        {
            _max7219.clearSegment(i);
        }
    }
}
#endif