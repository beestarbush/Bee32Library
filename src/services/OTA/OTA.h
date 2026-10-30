#ifndef _SERVICES_OTA_H
#define _SERVICES_OTA_H

#include "Bee32Config.h"

#if CONFIG_SERVICE_OTA

#include <WebServer.h>

namespace HAL::WIFI
{
    class Driver;
}

namespace SSL::OTA
{
    // ElegantOTA firmware upload page on port 81. Served only while enabled and the station is connected.
    // No authentication: anyone who can reach the device can replace its firmware.
    class Service
    {
        public:
            explicit Service(HAL::WIFI::Driver& wifi);

            void begin();
            void end();
            void run();

            void setEnabled(bool enabled);
            bool isEnabled() const;
            bool isActive() const;

        private:
            void stopServer();

            HAL::WIFI::Driver& _wifi;
            WebServer _server;
            bool _routesRegistered;
            bool _serverRunning;
            bool _enabled;
    };
}

#endif

#endif
