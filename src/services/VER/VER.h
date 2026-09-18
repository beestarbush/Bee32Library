#ifndef _SERVICES_VER_H
#define _SERVICES_VER_H

#include <stdint.h>
#include <WString.h>

namespace SSL::VER
{
    class Service
    {
        public:
            Service();
            ~Service() = default;

            void begin();
            void end();

            String getTag();
            String getVersion();
            String getDate();
    };
}

#endif