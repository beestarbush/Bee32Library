#ifndef _SERVICES_LED_H
#define _SERVICES_LED_H

#include <stdint.h>
#include <WString.h>

namespace HAL
{
    namespace PWM
    {
        class Driver;
    }
}

namespace SSL::LED
{
    enum State
    {
        OFF,
        ON,
        PULSE
    };

    class Service
    {
        public:
            Service(HAL::PWM::Driver& modeLed);
            ~Service() = default;

            void begin();
            void end();
            void run();

            void set(State state);

        private:
            HAL::PWM::Driver& _modeLed;
            State _state;
    };
}

#endif