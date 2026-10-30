#include "WIFI.h"

#if CONFIG_HAL_DRIVER_WIFI

#include "WiFi.h"
#include <esp_wifi.h>

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
        startConnect(ssid, password);
        WiFi.waitForConnectResult(_configuration._connectionTimeout);

        return WiFi.status() == WL_CONNECTED;
    }

    void Driver::startConnect(const char* ssid, const char* password)
    {
        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(true);
        WiFi.begin(ssid, password);
    }

    // A station disconnect keeps the radio in STA mode so ESP-NOW keeps working on the current channel.
    void Driver::disconnect()
    {
        if (isAccessPoint())
        {
            WiFi.softAPdisconnect();
            WiFi.mode(WIFI_OFF);
            return;
        }

        WiFi.setAutoReconnect(false);
        WiFi.disconnect(false, false);
    }

    uint8_t Driver::getChannel()
    {
        return static_cast<uint8_t>(WiFi.channel());
    }

    // Only possible while the station is not connected; the access point then dictates the channel.
    bool Driver::setChannel(uint8_t channel)
    {
        if (getConnected() || channel < 1U || channel > 13U)
        {
            return false;
        }
        if (WiFi.getMode() == WIFI_MODE_NULL)
        {
            WiFi.mode(WIFI_STA);
        }
        esp_wifi_set_promiscuous(true);
        const bool result = esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE) == ESP_OK;
        esp_wifi_set_promiscuous(false);
        return result;
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

#endif