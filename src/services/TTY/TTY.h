#ifndef _SERVICES_TTY_H
#define _SERVICES_TTY_H

#include <stdint.h>
#include "Arduino.h"

#include "services/CLI/ICLI.h"

namespace HAL
{
    namespace UART
    {
        class Driver;
    }
}

namespace SSL::TTY
{
    class Listener
    {
        public:
            virtual void OnTtyDataReadyCallback(String data) = 0;
    };

    class Service : public ICLI::Listener
    {
        public:
            Service(HAL::UART::Driver& uart);
            ~Service() = default;

            void begin();
            void end();

            void run();

            void registerITty(TTY::Listener* tty);
            void unregisterITty();

            void write(const char data[], bool newline = true) override;
            void write(const String &data, bool newline = true) override;
            void write(char data, bool newline = true) override;
            void write(bool data, bool newline = true) override;
            void write(uint32_t data, bool newline = true) override;
            void write(uint64_t data, bool newline = true) override;
            void write(void) override;

        private:
            String _input;
            TTY::Listener* _itty;
            HAL::UART::Driver& _uart;
    };
}

#endif