#ifndef _SERVICES_NVS_H
#define _SERVICES_NVS_H

#include "Preferences.h"

namespace SSL::NVS
{
    class Service
    {
        public:
            Service();

            void begin(const char* name);
            void end();

            bool write(const char* key, uint32_t value);
            bool write(const char* key, uint64_t value);
            bool write(const char* key, const char* value);
            bool read(const char* key, uint32_t* value);
            bool read(const char* key, uint64_t* value);
            bool read(const char* key, char* value, uint32_t bufferSize);
            bool remove(const char* key);
            bool contains(const char* key);

        private:
            Preferences _prf;
    };
}
#endif