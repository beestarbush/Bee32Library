#ifndef _HAL_UART_H
#define _HAL_UART_H

#include <stdint.h>

namespace HAL::UART
{
    class Configuration
    {
        public:
            Configuration(uint16_t baudRate);

            friend class Driver;

        private:
            uint16_t _baudRate;
    };

    class Driver
    {
        public:
            Driver(const Configuration& configuration);
            ~Driver() = default;

            void begin();
            void end();

        private:
            const Configuration _configuration;
    };
}

#endif