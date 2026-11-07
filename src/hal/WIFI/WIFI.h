#ifndef _HAL_WIFI_H
#define _HAL_WIFI_H

#include "Bee32Config.h"

#if CONFIG_HAL_DRIVER_WIFI

#include <atomic>
#include <stdint.h>
#include <WString.h>
#include <IPAddress.h>

namespace HAL::WIFI
{
    enum class ConnectionState : uint8_t
    {
        DISCONNECTED,
        ASSOCIATED,
        CONNECTED
    };

    class Configuration
    {
        public:
            Configuration(uint32_t connectionTimeout);

            friend class Driver;

        private:
            uint32_t _connectionTimeout;
    };

    class Driver
    {
        public:
            Driver(const Configuration& configuration);
            ~Driver() = default;

            void begin();
            void end();

            bool connect(const char* ssid, const char* password);
            // channel 0 scans all channels; a known channel connects without scanning.
            void startConnect(const char* ssid, const char* password, uint8_t channel = 0U);
            void disconnect();
            uint8_t getChannel();
            bool setChannel(uint8_t channel);
            void setupAccessPoint();
            IPAddress getAccessPointIpAddress();

            bool getConnected();
            ConnectionState getConnectionState() const;
            uint8_t getDisconnectReason() const;
            bool isAccessPoint();
            IPAddress getIpAddress();
            String getSubnetMask();
            String getGatewayAddress();
            String getMacAddress();

        private:
            const Configuration _configuration;
            std::atomic<ConnectionState> _connectionState;
            std::atomic<uint8_t> _disconnectReason;
    };
}

#endif

#endif