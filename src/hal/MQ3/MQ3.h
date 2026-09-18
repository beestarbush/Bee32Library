#ifndef _HAL_MQ3_H
#define _HAL_MQ3_H

#include <stdint.h>

namespace HAL::MQ3
{
    class Configuration
    {
        public:
            Configuration(uint8_t pin, uint8_t mode);

            friend class Driver;

        private:
            uint8_t _pin;
            uint8_t _mode;
    };

    class Driver
    {
        public:
            Driver(const Configuration& configuration);

            void begin();
            void end();

            uint16_t readAdc();
            double readVoltage();

        private:
            const Configuration _configuration;
    };
}

#endif