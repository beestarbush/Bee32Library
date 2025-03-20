#ifndef _HAL_WIFI_H
#define _HAL_WIFI_H

#include <stdint.h>
#include <WString.h>
#include <IPAddress.h>

namespace HAL::WIFI
{
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
            void disconnect();
            void setupAccessPoint();
            IPAddress getAccessPointIpAddress();

            bool getConnected();
            bool isAccessPoint();
            IPAddress getIpAddress();
            String getSubnetMask();
            String getGatewayAddress();
            String getMacAddress();

        private:
            const Configuration _configuration;
    };
}

#endif