#ifndef _HAL_BTN_H
#define _HAL_BTN_H

#include <stdint.h>

namespace HAL::BTN
{
    typedef struct
    {
        uint8_t pin;
        uint8_t mode;
    } button_config_t;

    class Driver
    {
        public:
            Driver(button_config_t cfg);

            void begin();
            void end();
            void run();

            bool getPressed();

        private:
            button_config_t _cfg;
            bool _pressed;
    };
}

#endif