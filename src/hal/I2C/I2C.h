#ifndef _HAL_I2C_H
#define _HAL_I2C_H

#include <stdint.h>
#include "Wire.h"

namespace HAL::I2C
{
    class Configuration
    {
        public:
            Configuration(uint8_t bus, uint8_t clockPin, uint8_t dataPin);

            friend class Driver;

        private:
            uint8_t _bus;
            uint8_t _clockPin;
            uint8_t _dataPin;
    };

    class Driver
    {
        public:
            Driver(const Configuration& configuration);
            ~Driver() = default;

            void begin();
            void end();

            TwoWire* getWire();

        private:
            const Configuration _configuration;
            TwoWire _wire;
    };
}

#endif