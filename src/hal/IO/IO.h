#ifndef _IO_H
#define _IO_H

#include <stdint.h>
#include <functional>

namespace HAL::IO
{
    class Configuration
    {
        public:
            Configuration(uint8_t pin, uint8_t mode, bool activeHigh = true);

            friend class Driver;

        private:
            uint8_t _pin;
            uint8_t _mode;
            bool _activeHigh;
    };

    class Driver
    {
        public:
            using Callback = std::function<void(bool)>;
            Driver(const Configuration& configuration);

            void begin();
            void end();
            void run();

            void registerCallback(Callback callback);

            bool getValue();

        private:
            const Configuration _configuration;
            bool _value;
            bool _lastReadValue;
            uint64_t _lastDebounceTime;
            Callback _callback;
    };
}

#endif