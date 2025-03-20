#ifndef _SERVICES_TNT_H
#define _SERVICES_TNT_H

#include <stdint.h>

#include "WString.h"
#include "WiFi.h"

#include <services/CLI/ICLI.h>

#define TNT_WRITE_BUFFER_SIZE       (256)

namespace HAL
{
    namespace WIFI
    {
        class Driver;
    }
}

namespace SSL::TNT
{
    class Listener
    {
        public:
        virtual void OnTntDataReadyCallback(String data) = 0;
    };

    class Service : public ICLI::Listener
    {
        public:
            Service(HAL::WIFI::Driver& wifi);
            ~Service() = default;

            void begin();
            void end();

            void run();

            void registerITnt(TNT::Listener* TNT);
            void unregisterITnt();

            void write(const char data[], bool newline = true) override;
            void write(const String &data, bool newline = true) override;
            void write(char data, bool newline = true) override;
            void write(bool data, bool newline = true) override;
            void write(uint32_t data, bool newline = true) override;
            void write(uint64_t data, bool newline = true) override;
            void write(void) override;

        private:
            String _input;
            char _writeBuffer[TNT_WRITE_BUFFER_SIZE];
            HAL::WIFI::Driver& _wifi;
            WiFiServer _server;
            WiFiClient _client;
            TNT::Listener* _itnt;
            typedef enum : uint8_t
            {
                STATE_MIN = 0,
                STATE_WAIT_FOR_WIFI = STATE_MIN,
                STATE_LISTEN,
                STATE_ESTABLISHED,
                STATE_CLOSING,
                STATE_MAX
            } state_t;
            state_t _state;
    };
}

#endif