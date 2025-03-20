#ifndef _SERVICES_HTTP_H
#define _SERVICES_HTTP_H

#include <stdint.h>

#include "WString.h"
#include "WiFi.h"

#include <services/CLI/ICLI.h>

#define HTTP_WRITE_BUFFER_SIZE       (512)

namespace HAL
{
    namespace WIFI
    {
        class Driver;
    }
}

namespace SSL::HTTP
{
    class Listener
    {
        public:
            virtual void OnHttpDataReadyCallback(String data) = 0;
    };

    class Service : public ICLI::Listener
    {
        public:
            Service(HAL::WIFI::Driver& wifi);
            ~Service() = default;

            void begin();
            void end();

            void run();

            void registerIHttp(HTTP::Listener* http);
            void unregisterIHttp();

            void write(const char data[], bool newline = true) override;
            void write(const String &data, bool newline = true) override;
            void write(char data, bool newline = true) override;
            void write(bool data, bool newline = true) override;
            void write(uint32_t data, bool newline = true) override;
            void write(uint64_t data, bool newline = true) override;
            void write(void) override;

        private:
            bool parseGetRequest();

            String _input;
            char _writeBuffer[HTTP_WRITE_BUFFER_SIZE];
            HAL::WIFI::Driver& _wifi;
            WiFiServer _server;
            WiFiClient _client;
            HTTP::Listener* _ihttp;
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