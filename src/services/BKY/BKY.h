#ifndef _SERVICES_BKY_H
#define _SERVICES_BKY_H

#include <BleKeyboard.h>

namespace HAL
{
    namespace BLT
    {
        class Driver;
    }
}

namespace SSL::BKY
{
    class Service
    {
        public:
            Service(HAL::BLT::Driver& bluetooth);
            ~Service() = default;

            void begin();
            void end();

            bool isConnected();

            void press(uint8_t key);
            void write(uint8_t text);
            void write(String text);
            void release(uint8_t key);

        private:
            HAL::BLT::Driver& _bluetooth;
            BleKeyboard _keyboard;
    };
}

#endif