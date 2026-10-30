#ifndef _SERVICES_NET_H
#define _SERVICES_NET_H

#include "Bee32Config.h"

#if CONFIG_SERVICE_NET

#include <stdint.h>
#include <stddef.h>

namespace HAL::WIFI
{
    class Driver;
}
namespace SSL::NVS
{
    class Service;
}
namespace SSL::TIM
{
    class Service;
}
namespace SSL::OTA
{
    class Service;
}

namespace SSL::NET
{
    enum class Mode : uint8_t
    {
        OFFLINE = 0,
        ONLINE = 1,
        UPDATE = 2
    };

    // Network policy: Wi-Fi is off by default and only joins the router at boot (time sync and
    // learning the router channel) and while ONLINE or UPDATE is requested. Offline, the radio stays
    // on the learned channel so ESP-NOW peers that joined the same router share it.
    // Credentials come from the configuration keys "ssid" and "password"; the learned channel is
    // stored as "channel" and used when the router is unreachable at boot.
    class Service
    {
        public:
            Service(HAL::WIFI::Driver& wifi, NVS::Service& configuration, TIM::Service& time, OTA::Service& ota);
            ~Service() = default;

            // Blocks for up to the Wi-Fi connection timeout plus the NTP timeout.
            void begin();
            void run();
            void end();

            void setMode(Mode mode);
            Mode getMode() const;
            bool isConnected() const;
            uint8_t getChannel() const;
            // Moves the offline radio to another channel, for example to search for an ESP-NOW peer.
            bool setChannel(uint8_t channel);

        private:
            static constexpr uint8_t DEFAULT_CHANNEL = 1U;

            bool readCredentials(char* ssid, size_t ssidSize, char* password, size_t passwordSize);
            void onConnected();

            HAL::WIFI::Driver& _wifi;
            NVS::Service& _configuration;
            TIM::Service& _time;
            OTA::Service& _ota;
            Mode _mode;
            uint8_t _channel;
            bool _connecting;
            bool _connected;
    };
}

#endif

#endif
