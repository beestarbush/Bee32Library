#include "TIM.h"
#include "time.h"
#include <hal/WIFI/WIFI.h>
#include <Arduino.h>

#define GMT_OFFSET  (0) // No offset so UTC is used which will be the easiest.
#define DST_OFFSET  (0) // No offset so no DST is used which will be the easiest.
#define NTP_SERVER  "pool.ntp.org"

#define I2C_CONNECTED
#define UPDATE_INTERVAL     (1000 * 60 * 60 * 12) // every 12 hour sync with NTP.
#define SYNC_RETRY_LIMIT    (3)

namespace SSL::TIM
{
    Service::Service(HAL::WIFI::Driver& wifi) :
        _wifi(wifi),
        _valid(false),
        _updateInterval(0),
        _syncRetryCounter(0)
    {
    }

    void Service::begin()
    {
        // if (_rtc.getLostPower())
        // {
            // Activate WiFi when we do not have a valid time due to a loss of power at the RTC.
            if (_wifi.getConnected())
            {
                // The RTC lost its power due to an empty battery or removal of battery, when a WiFi connection is
                // available we will try to fetch the time from NTP and synchronize this time to the RTC. When no
                // WiFi connection is available or the fetch failed, we will reset the RTC to a well-known default
                // time which will give the user the ability to adjust the RTC time from the menu or via CLI.
                if (!sync())
                {
                    _valid = false;
                }
                else
                {
                    _valid = true;
                }
            }
            else
            {
                // Also invalid when no WiFi is available.
                _valid = false;
            }
        // }
        // else
        // {
        //     _valid = true;
        // }
    }

    void Service::end()
    {
    }

    void Service::run()
    {
        // Periodically sync the time via NTP.
        uint64_t now = millis();
        if (now - _updateInterval < UPDATE_INTERVAL)
        {
            return;
        }

        // Perform a try to sync the current time from a NTP server to the RTC.
        if (sync())
        {
            // When syncing is successful we want to synchronize after the next UPDATE_INTERVAL time.
            _syncRetryCounter = 0;
            _updateInterval = now;
            _valid = true;
            return;
        }

        // When syncing fails we increase the retry counter.
        _syncRetryCounter++;

        // When syncing failed for 3 times, we assume that the connection is not stable
        // and thus we will skip the synchronisation and wait for the next UPDATE_INTERVAL.
        if (_syncRetryCounter >= SYNC_RETRY_LIMIT)
        {
            _syncRetryCounter = 0;
            _updateInterval = now;
        }
    }

    uint64_t Service::getEpochTime()
    {
    return 0;//_rtc.getEpochTime();
    }

    bool Service::getValid()
    {
        return _valid;
    }

    bool Service::sync()
    {
        if (!_wifi.getConnected() ||
            _wifi.isAccessPoint())
        {
            Serial.println("Wifi not connected.");
            return false;
        }

        struct tm now;
        configTime(GMT_OFFSET, DST_OFFSET, NTP_SERVER);
        if (!getLocalTime(&now))
        {
            // Failed to retrieve local time from ntp server.
            Serial.println("NTP failed.");
            return false;
        }

        Serial.println("Adjusting RTC.");
        Serial.println(&now, "%A, %B %d %Y %H:%M:%S");

        return true;
    }
}