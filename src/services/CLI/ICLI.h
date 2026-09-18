#ifndef _SERVICES_ICLI_H
#define _SERVICES_ICLI_H

#include <WString.h>

namespace SSL::ICLI
{
    class Listener
    {
        public:
            virtual void write(const char data[], bool newline = true) = 0;
            virtual void write(const String &data, bool newline = true) = 0;
            virtual void write(char data, bool newline = true) = 0;
            virtual void write(bool data, bool newline = true) = 0;
            virtual void write(uint32_t data, bool newline = true) = 0;
            virtual void write(uint64_t data, bool newline = true) = 0;
            virtual void write(void) = 0;
    };
}

#endif
