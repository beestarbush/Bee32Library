#include "WIFI.h"
#include "WiFi.h"

namespace HAL::WIFI
{
    Configuration::Configuration(uint32_t connectionTimeout) :
        _connectionTimeout(connectionTimeout)
    {
    }

    Driver::Driver(const Configuration& configuration) :
        _configuration(configuration)
    {
    }

    void Driver::begin()
    {
    }

    void Driver::end()
    {
    }

    bool Driver::connect(const char* ssid, const char* password)
    {
        WiFi.mode(WIFI_AP);
        WiFi.setAutoReconnect(true);
        WiFi.begin(ssid, password);
        WiFi.waitForConnectResult(_configuration._connectionTimeout);

        return WiFi.status() == WL_CONNECTED;
    }

    void Driver::disconnect()
    {
        if (isAccessPoint())
        {
            WiFi.softAPdisconnect();
        }
        else
        {
            WiFi.disconnect();
            WiFi.waitForConnectResult();
        }
        
        // Disable the WIFI radios.
        WiFi.mode(WIFI_OFF);
    }

    void Driver::setupAccessPoint()
    {
        WiFi.setAutoReconnect(false);
        WiFi.mode(WIFI_AP);
        WiFi.softAP("TimeSinceClock");
        Serial.print("Not connected to wifi, setting up AP with IP:");
        Serial.println(WiFi.softAPIP());
    }

    IPAddress Driver::getAccessPointIpAddress()
    {
        return WiFi.softAPIP();
    }

    bool Driver::getConnected()
    {
        return WiFi.status() == WL_CONNECTED;
    }

    bool Driver::isAccessPoint()
    {
        return WiFi.getMode() == WIFI_MODE_AP;
    }

    IPAddress Driver::getIpAddress()
    {
        return WiFi.localIP();
    }

    String Driver::getSubnetMask()
    {
        return WiFi.subnetMask().toString();
    }

    String Driver::getGatewayAddress()
    {
        return WiFi.gatewayIP().toString();
    }

    String Driver::getMacAddress()
    {
        return WiFi.macAddress();
    }
}