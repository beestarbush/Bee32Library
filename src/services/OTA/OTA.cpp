#include "OTA.h"

#if CONFIG_SERVICE_OTA

#include <ElegantOTA.h>
#include <hal/WIFI/WIFI.h>

#define OTA_PORT    (81)

namespace SSL::OTA
{
    Service::Service(HAL::WIFI::Driver& wifi) :
        _wifi(wifi),
        _server(OTA_PORT),
        _routesRegistered(false),
        _serverRunning(false),
        _enabled(false)
    {
    }

    void Service::begin()
    {
        if (!_routesRegistered)
        {
            ElegantOTA.begin(&_server);
            _routesRegistered = true;
        }
    }

    void Service::end()
    {
        stopServer();
    }

    void Service::run()
    {
        if (!_enabled || !_wifi.getConnected())
        {
            stopServer();
            return;
        }

        if (!_serverRunning)
        {
            _server.begin();
            _serverRunning = true;
        }

        _server.handleClient();
        ElegantOTA.loop();
    }

    void Service::setEnabled(bool enabled)
    {
        _enabled = enabled;
    }

    bool Service::isEnabled() const
    {
        return _enabled;
    }

    bool Service::isActive() const
    {
        return _serverRunning;
    }

    void Service::stopServer()
    {
        if (_serverRunning)
        {
            _server.stop();
            _serverRunning = false;
        }
    }
}

#endif
