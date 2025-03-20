#include "TTY.h"
#include <hal/UART/UART.h>

#define CLI_CHARACTER       ("# ")

namespace SSL::TTY
{
    Service::Service(HAL::UART::Driver& uart) :
        ICLI::Listener(),
        _itty(nullptr),
        _uart(uart)
    {
    }

    void Service::begin()
    {
    }

    void Service::end()
    {
    }

    void Service::run()
    {
        if (Serial.available())
        {
            char data = Serial.read();
            if (_itty == nullptr)
            {
                // When no registered TTY is available, no need to handle the TTY but we now at least
                // have a clear UART buffer.
                return;
            }
            if (data != '\n')
            {
                Serial.print(data);
                _input.concat(data);
            }

            if (data == '\n')
            {
                Serial.print(data);

                if (_itty != nullptr)
                {
                    _itty->OnTtyDataReadyCallback(_input);
                }

                _input.clear();
                Serial.print(CLI_CHARACTER);
            }
        }
    }

    void Service::registerITty(TTY::Listener* tty)
    {
        _itty = tty;
    }

    void Service::unregisterITty()
    {
        _itty = nullptr;
    }

    void Service::write(const char data[], bool newline)
    {
        if (newline)
        {
            Serial.println(data);
        }
        else
        {
            Serial.print(data);
        }
    }

    void Service::write(const String &data, bool newline)
    {
        if (newline)
        {
            Serial.println(data);
        }
        else
        {
            Serial.print(data);
        } 
    }

    void Service::write(char data, bool newline)
    {
        if (newline)
        {
            Serial.println(data);
        }
        else
        {
            Serial.print(data);
        }
    }

    void Service::write(bool data, bool newline)
    {
        if (newline)
        {
            Serial.println(data);
        }
        else
        {
            Serial.print(data);
        }
    }

    void Service::write(uint32_t data, bool newline)
    {
        if (newline)
        {
            Serial.println(data);
        }
        else
        {
            Serial.print(data);
        }
    }


    void Service::write(uint64_t data, bool newline)
    {
        if (newline)
        {
            Serial.println(data);
        }
        else
        {
            Serial.print(data);
        }
    }

    void Service::write(void)
    {
        Serial.println();
    }
}