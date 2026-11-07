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
        _configuration(configuration),
        _connectionState(ConnectionState::DISCONNECTED),
        _disconnectReason(0U)
    {
    }

    void Driver::begin()
    {
        WiFi.onEvent([this](arduino_event_id_t event, arduino_event_info_t info)
        {
            switch (event)
            {
                case ARDUINO_EVENT_WIFI_STA_CONNECTED:
                    _connectionState.store(ConnectionState::ASSOCIATED, std::memory_order_relaxed);
                    break;
                case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                    _connectionState.store(ConnectionState::CONNECTED, std::memory_order_relaxed);
                    break;
                case ARDUINO_EVENT_WIFI_STA_LOST_IP:
                    _connectionState.store(ConnectionState::ASSOCIATED, std::memory_order_relaxed);
                    break;
                case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
                    _disconnectReason.store(info.wifi_sta_disconnected.reason, std::memory_order_relaxed);
                    _connectionState.store(ConnectionState::DISCONNECTED, std::memory_order_relaxed);
                    break;
                default:
                    break;
            }
        });
    }

    void Driver::end()
    {
    }

    bool Driver::connect(const char* ssid, const char* password)
    {
        startConnect(ssid, password);
        WiFi.waitForConnectResult(_configuration._connectionTimeout);

        const bool connected = WiFi.status() == WL_CONNECTED;
        if (connected)
        {
            _connectionState.store(ConnectionState::CONNECTED, std::memory_order_relaxed);
        }
        return connected;
    }

    void Driver::startConnect(const char* ssid, const char* password, uint8_t channel)
    {
        _connectionState.store(ConnectionState::DISCONNECTED, std::memory_order_relaxed);
        _disconnectReason.store(0U, std::memory_order_relaxed);
        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(true);
        WiFi.begin(ssid, password, channel);
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
        if (getConnectionState() != ConnectionState::DISCONNECTED || getConnected() || channel < 1U || channel > 13U)
        {
            return false;
        }
        if (WiFi.getMode() == WIFI_MODE_NULL)
        {
            WiFi.mode(WIFI_STA);
        }
        uint8_t current = 0U;
        wifi_second_chan_t second = WIFI_SECOND_CHAN_NONE;
        if (esp_wifi_get_channel(&current, &second) == ESP_OK && current == channel)
        {
            return true;
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

    ConnectionState Driver::getConnectionState() const
    {
        return _connectionState.load(std::memory_order_relaxed);
    }

    uint8_t Driver::getDisconnectReason() const
    {
        return _disconnectReason.load(std::memory_order_relaxed);
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