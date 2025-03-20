#include "UART.h"
#include "Arduino.h"

namespace HAL::UART
{
    Configuration::Configuration(uint16_t baudRate) :
        _baudRate(baudRate)
    {
    }

    Driver::Driver(const Configuration& configuration) :
        _configuration(configuration)
    {
    }

    void Driver::begin()
    {
        Serial.begin(_configuration._baudRate);
        delay(100);
        Serial.flush();
    }

    void Driver::end()
    {
        Serial.end();
    }
}