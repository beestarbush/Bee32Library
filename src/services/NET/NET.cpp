#include "NET.h"

#if CONFIG_SERVICE_NET

#include <Arduino.h>
#include <hal/WIFI/WIFI.h>
#include <services/NVS/NVS.h>
#include <services/TIM/TIM.h>
#include <services/OTA/OTA.h>
#include <stdint.h>
#include <utility>

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
        _associated(false),
        _connected(false),
        _retryCount(0U),
        _connectStart(0U),
        _associationStart(0U),
        _retryAt(0U),
        _connectionRequirements{[this]() { return _time.isSyncPending(); }}
    {
    }

    void Service::begin()
    {
        _mode = Mode::OFFLINE;
        _ota.setEnabled(false);

        uint32_t stored = 0U;
        if (_configuration.read("channel", &stored) && stored >= 1U && stored <= 13U)
        {
            _channel = static_cast<uint8_t>(stored);
        }

    #if CONFIG_SERVICE_NET_BOOT_CONNECT
        char ssid[SSID_SIZE] = { 0 };
        char password[PASSWORD_SIZE] = { 0 };
        if (readCredentials(ssid, sizeof(ssid), password, sizeof(password)))
        {
            _time.requestSync();
            _retryCount = 0U;
            startConnection(millis());
        }
        else
        {
            Serial.printf("No Wi-Fi config; using channel %u.\n", _channel);
            _wifi.setChannel(_channel);
        }
    #else
        Serial.printf("NET boot connect disabled; using channel %u.\n", _channel);
        _wifi.setChannel(_channel);
    #endif
        _connected = false;
    }

    void Service::run()
    {
        const uint32_t now = millis();
        const HAL::WIFI::ConnectionState state = _wifi.getConnectionState();
        const bool connected = state == HAL::WIFI::ConnectionState::CONNECTED;

        if (_mode == Mode::OFFLINE && !hasConnectionRequirement())
        {
            if (connected || _connecting || _connected)
            {
                _wifi.disconnect();
            }
            if (!_wifi.getConnected())
            {
                _wifi.setChannel(_channel);
            }
            _connecting = false;
            _associated = false;
            _retryCount = 0U;
            _retryAt = 0U;
            _connected = false;
        }
        else if (connected)
        {
            const bool newlyConnected = !_connected;
            _connecting = false;
            _associated = false;
            _retryCount = 0U;
            _retryAt = 0U;
            _connected = true;
            if (newlyConnected)
            {
                onConnected();
            }
        }
        else
        {
            if (_connected)
            {
                _connected = false;
                _connecting = false;
                _associated = false;
                _retryAt = now;
            }

            advanceConnection(now, state);
        }

        if (_mode == Mode::UPDATE)
        {
            _wifi.startAccessPoint();
        }
        else
        {
            _wifi.stopAccessPoint();
        }
        _ota.setEnabled(_mode == Mode::UPDATE);
    }

    void Service::end()
    {
        _mode = Mode::OFFLINE;
        _ota.setEnabled(false);
        _wifi.stopAccessPoint();
        _wifi.disconnect();
        _connecting = false;
        _associated = false;
        _connected = false;
    }

    void Service::setMode(Mode mode)
    {
        if (_mode != mode && mode != Mode::OFFLINE)
        {
            _retryAt = 0U;
            if (!_time.getValid())
            {
                _time.requestSync();
            }
        }
        _mode = mode;
    }

    void Service::addConnectionRequirement(std::function<bool()> requirement)
    {
        _connectionRequirements.push_back(std::move(requirement));
    }

    bool Service::hasConnectionRequirement() const
    {
        for (const auto& requirement : _connectionRequirements)
        {
            if (requirement())
            {
                return true;
            }
        }
        return false;
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
        if (_mode != Mode::OFFLINE || _connecting || hasConnectionRequirement() || !_wifi.setChannel(channel))
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

    void Service::advanceConnection(uint32_t now, HAL::WIFI::ConnectionState state)
    {
        if (_connecting)
        {
            if (state == HAL::WIFI::ConnectionState::ASSOCIATED)
            {
                if (!_associated)
                {
                    _associated = true;
                    _associationStart = now;
                }
                if (now - _associationStart >= IP_TIMEOUT_MS)
                {
                    failConnection(now);
                }
            }
            else if (_associated || now - _connectStart >= ASSOCIATION_TIMEOUT_MS)
            {
                failConnection(now);
            }
        }

        if (!_connecting && static_cast<int32_t>(now - _retryAt) >= 0)
        {
            startConnection(now);
        }
    }

    void Service::startConnection(uint32_t now)
    {
        char ssid[SSID_SIZE] = { 0 };
        char password[PASSWORD_SIZE] = { 0 };
        if (!readCredentials(ssid, sizeof(ssid), password, sizeof(password)))
        {
            Serial.println("No Wi-Fi config; set ssid and password with cfg.");
            _retryAt = now + BACKGROUND_RETRY_DELAY_MS;
            return;
        }

        _connecting = true;
        _associated = false;
        _connectStart = now;
        _associationStart = 0U;
        _retryAt = 0U;
        const uint8_t channel = _retryCount == 0U ? _channel : 0U;
        _wifi.startConnect(ssid, password, channel);
    }

    void Service::failConnection(uint32_t now)
    {
        const uint8_t reason = _wifi.getDisconnectReason();
        _wifi.disconnect();
        _connecting = false;
        _associated = false;
        if (_retryCount < UINT8_MAX)
        {
            ++_retryCount;
        }
        const uint32_t delay = _retryCount <= FAST_RETRY_LIMIT ? FAST_RETRY_DELAY_MS : BACKGROUND_RETRY_DELAY_MS;
        _retryAt = now + delay;
        Serial.printf("Wi-Fi attempt failed (reason %u); retrying in %lu ms.\n",
                      reason, static_cast<unsigned long>(delay));
    }

    // The router decides the channel; remember it so offline ESP-NOW stays on the shared channel.
    void Service::onConnected()
    {
        _channel = _wifi.getChannel();
        uint32_t stored = 0U;
        if (!_configuration.read("channel", &stored) || stored != _channel)
        {
            _configuration.write("channel", static_cast<uint32_t>(_channel));
        }
    }
}

#endif
