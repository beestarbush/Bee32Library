#ifdef CONFIG_HAL_DRIVER_MAX72XX
#ifndef _HAL_MAX_H
#define _HAL_MAX_H

#include "LedController.hpp"

namespace HAL::MAX72XX
{
    class Configuration
    {
        public:
            Configuration(uint8_t mosi, uint8_t clk, uint8_t cs, uint8_t cascadedDevices, uint8_t intensity, uint8_t connectedDigits);

            friend class Driver;

        private:
            uint8_t _mosiPin;
            uint8_t _clkPin;
            uint8_t _csPin;
            uint8_t _cascadedDevices;
            uint8_t _intensity;
            uint8_t _connectedDigits;
    };

    class Driver
    {
        public:
            Driver(const Configuration& configuration);

            void begin();
            void end();
            void run();

            void setDigit(uint8_t digit, uint8_t value, bool decimalPoint);
            void setChar(uint8_t digit, char character, bool decimalPoint);
            uint8_t getConnectedDigits();
            void setBrightness(uint8_t brightness);
            void clearAll();

        private:
            LedController _max7219;
            const Configuration _configuration;
            uint64_t _updateInterval;
    };
}

#endif
#endif