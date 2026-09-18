#ifndef _SERVICES_TIM_H
#define _SERVICES_TIM_H

#include <stdint.h>
#include <WString.h>

namespace HAL
{
    namespace WIFI
    {
        class Driver;
    }
}

namespace SSL::TIM
{
    class Service
    {
        public:
            Service(HAL::WIFI::Driver& wifi);
            ~Service() = default;

            void begin();
            void end();
            void run();

            uint64_t getEpochTime();
            bool getValid();

            bool sync();

        private:
            HAL::WIFI::Driver& _wifi;
            bool _valid;
            uint64_t _updateInterval;
            uint8_t _syncRetryCounter;
    };
}

#endif