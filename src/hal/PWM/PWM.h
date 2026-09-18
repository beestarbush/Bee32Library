#ifndef _HAL_PWM_H
#define _HAL_PWM_H

#include <stdint.h>

namespace HAL::PWM
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
            Driver(const Configuration& config);

            void begin();
            void end();

            void setValue(uint8_t value);

        private:
            const Configuration _configuration;
            uint8_t _value;
    };
}

#endif