#include "NET.h"

#if CONFIG_SERVICE_NET

#include <Arduino.h>
#include <hal/WIFI/WIFI.h>
#include <services/NVS/NVS.h>
#include <services/TIM/TIM.h>
#include <services/OTA/OTA.h>

namespace SSL::NET
{
    Service::Service(HAL::WIFI::Driver& wifi, NVS::Service& configuration, TIM::Service& time, OTA::Service& ota) :
        _wifi(wifi),
        _configuration(configuration),
        _time(time),
        _ota(ota),
        _mode(Mode::OFFLINE),
        _channel(DEFAULT_CHANNEL),
        _connecting(false),
        _connected(false)
    {
    }

    void Service::begin()
    {
        _mode = Mode::OFFLINE;
        _ota.setEnabled(false);

        char ssid[30] = { 0 };
        char password[30] = { 0 };
        if (readCredentials(ssid, sizeof(ssid), password, sizeof(password)) && _wifi.connect(ssid, password))
        {
            onConnected();
            _wifi.disconnect();
        }
        else
        {
            uint32_t stored = 0U;
            if (_configuration.read("channel", &stored) && stored >= 1U && stored <= 13U)
            {
                _channel = static_cast<uint8_t>(stored);
            }
            Serial.printf("Router not reached, using channel %u.\n", _channel);
        }

        _connected = false;
        _wifi.setChannel(_channel);
    }

    void Service::run()
    {
        const bool connected = _wifi.getConnected();

        if (_mode == Mode::OFFLINE)
        {
            if (connected || _connecting)
            {
                _wifi.disconnect();
                _wifi.setChannel(_channel);
                _connecting = false;
            }
        }
        else if (connected && !_connected)
        {
            _connecting = false;
            onConnected();
        }
        else if (!connected && !_connecting)
        {
            char ssid[30] = { 0 };
            char password[30] = { 0 };
            // Marked as connecting even without credentials so a missing config is reported once.
            _connecting = true;
            if (readCredentials(ssid, sizeof(ssid), password, sizeof(password)))
            {
                _wifi.startConnect(ssid, password);
            }
            else
            {
                Serial.println("No Wi-Fi config; set ssid and password with cfg.");
            }
        }

        _connected = connected && _mode != Mode::OFFLINE;
        _ota.setEnabled(_mode == Mode::UPDATE);
    }

    void Service::end()
    {
        _mode = Mode::OFFLINE;
        _ota.setEnabled(false);
        _wifi.disconnect();
        _connecting = false;
        _connected = false;
    }

    void Service::setMode(Mode mode)
    {
        _mode = mode;
    }

    Mode Service::getMode() const
    {
        return _mode;
    }

    bool Service::isConnected() const
    {
        return _connected;
    }

    uint8_t Service::getChannel() const
    {
        return _channel;
    }

    bool Service::setChannel(uint8_t channel)
    {
        if (_mode != Mode::OFFLINE || !_wifi.setChannel(channel))
        {
            return false;
        }
        _channel = channel;
        return true;
    }

    bool Service::readCredentials(char* ssid, size_t ssidSize, char* password, size_t passwordSize)
    {
        return _configuration.read("ssid", ssid, ssidSize) &&
               _configuration.read("password", password, passwordSize);
    }

    // The router decides the channel; remember it so offline ESP-NOW stays on the shared channel.
    void Service::onConnected()
    {
        _time.sync();
        _channel = _wifi.getChannel();
        uint32_t stored = 0U;
        if (!_configuration.read("channel", &stored) || stored != _channel)
        {
            _configuration.write("channel", static_cast<uint32_t>(_channel));
        }
    }
}

#endif
