#include "TIM.h"
#include "time.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <hal/WIFI/WIFI.h>
#include <Arduino.h>

#define GMT_OFFSET  (0) // No offset so UTC is used which will be the easiest.
#define DST_OFFSET  (0) // No offset so no DST is used which will be the easiest.
#define NTP_SERVER  "pool.ntp.org"

#define UPDATE_INTERVAL     (1000 * 60 * 60 * 12) // every 12 hour sync with NTP.
#define SYNC_TIMEOUT_MS     (10000U)
#define SYNC_RETRY_DELAY_MS (60000U)
#define SYNC_REQUEST_TIMEOUT_MS (180000U)
#define SYNC_ATTEMPT_LIMIT (3U)
#define VALID_TIME_AFTER    (1704067200) // Same threshold as used to detect a completed NTP sync.
#define SUN_ZENITH_DEGREES  (90.833) // Sun centre at the horizon including refraction and the solar disc.

namespace
{
    constexpr double DEG_TO_RAD = M_PI / 180.0;
    constexpr double RAD_TO_DEG = 180.0 / M_PI;

    // NOAA solar calculator. Returns minutes after 00:00 UTC of the UTC day containing noonUtc.
    // The result can fall outside 0..1440 for locations far from the prime meridian.
    bool sunMinutesUtc(double latitude, double longitude, time_t noonUtc, bool rising, double* minutes)
    {
        const double julianDay = static_cast<double>(noonUtc) / 86400.0 + 2440587.5;
        const double t = (julianDay - 2451545.0) / 36525.0;
        const double meanLongitude = fmod(280.46646 + t * (36000.76983 + t * 0.0003032), 360.0);
        const double meanAnomaly = (357.52911 + t * (35999.05029 - 0.0001537 * t)) * DEG_TO_RAD;
        const double eccentricity = 0.016708634 - t * (0.000042037 + 0.0000001267 * t);
        const double center = sin(meanAnomaly) * (1.914602 - t * (0.004817 + 0.000014 * t))
            + sin(2.0 * meanAnomaly) * (0.019993 - 0.000101 * t)
            + sin(3.0 * meanAnomaly) * 0.000289;
        const double omega = (125.04 - 1934.136 * t) * DEG_TO_RAD;
        const double apparentLongitude = (meanLongitude + center - 0.00569 - 0.00478 * sin(omega)) * DEG_TO_RAD;
        const double obliquity = (23.0 + (26.0 + (21.448 - t * (46.815 + t * (0.00059 - t * 0.001813))) / 60.0) / 60.0
            + 0.00256 * cos(omega)) * DEG_TO_RAD;
        const double declination = asin(sin(obliquity) * sin(apparentLongitude));

        const double y = tan(obliquity / 2.0) * tan(obliquity / 2.0);
        const double meanLongitudeRad = meanLongitude * DEG_TO_RAD;
        const double equationOfTime = 4.0 * RAD_TO_DEG * (y * sin(2.0 * meanLongitudeRad)
            - 2.0 * eccentricity * sin(meanAnomaly)
            + 4.0 * eccentricity * y * sin(meanAnomaly) * cos(2.0 * meanLongitudeRad)
            - 0.5 * y * y * sin(4.0 * meanLongitudeRad)
            - 1.25 * eccentricity * eccentricity * sin(2.0 * meanAnomaly));

        const double latitudeRad = latitude * DEG_TO_RAD;
        const double cosHourAngle = cos(SUN_ZENITH_DEGREES * DEG_TO_RAD) / (cos(latitudeRad) * cos(declination))
            - tan(latitudeRad) * tan(declination);
        if (cosHourAngle < -1.0 || cosHourAngle > 1.0)
        {
            return false;
        }

        const double hourAngle = acos(cosHourAngle) * RAD_TO_DEG;
        const double solarNoon = 720.0 - 4.0 * longitude - equationOfTime;
        *minutes = rising ? solarNoon - 4.0 * hourAngle : solarNoon + 4.0 * hourAngle;
        return true;
    }

    int32_t dayKey(const tm& local)
    {
        return (local.tm_year + 1900) * 10000 + (local.tm_mon + 1) * 100 + local.tm_mday;
    }
}

namespace SSL::TIM
{
    Service::Service(HAL::WIFI::Driver& wifi) :
        _wifi(wifi),
        _valid(false),
        _syncState(SyncState::IDLE),
        _attemptCount(0U),
        _updateInterval(0U),
        _requestStart(0U),
        _syncStart(0U),
        _retryAt(0U),
        _latitude(0.0),
        _longitude(0.0),
        _locationSet(false),
        _timezone("UTC0"),
        _registrations(),
        _registrationCount(0U),
        _scheduleSynced(false)
    {
    }

    void Service::begin()
    {
        _valid = false;
        _syncState = SyncState::IDLE;
        _attemptCount = 0U;
        _updateInterval = millis();
        _requestStart = 0U;
        _syncStart = 0U;
        _retryAt = 0U;
        _scheduleSynced = false;
        for (uint8_t index = 0U; index < _registrationCount; ++index)
        {
            _registrations[index].lastFiredDay = 0;
        }
    }

    void Service::end()
    {
        _syncState = SyncState::IDLE;
    }

    void Service::run()
    {
        const uint32_t now = millis();
        runSchedule();
        if (isSyncPending() && _attemptCount != 0U)
        {
            time_t epoch;
            time(&epoch);
            if (epoch >= VALID_TIME_AFTER)
            {
                Serial.println("NTP time valid.");
                _syncState = SyncState::IDLE;
                _valid = true;
                _updateInterval = now;
                return;
            }
        }
        if (isSyncPending() && now - _requestStart >= SYNC_REQUEST_TIMEOUT_MS)
        {
            Serial.println("NTP sync request expired.");
            _syncState = SyncState::IDLE;
            _updateInterval = now;
        }

        if (!_wifi.getConnected() || _wifi.isAccessPoint())
        {
            if (_syncState == SyncState::ACTIVE)
            {
                retryOrFinish(now);
            }
            return;
        }

        if (_syncState == SyncState::ACTIVE)
        {
            if (now - _syncStart >= SYNC_TIMEOUT_MS)
            {
                retryOrFinish(now);
            }
            return;
        }

        if (!isSyncPending() && _valid && now - _updateInterval >= UPDATE_INTERVAL)
        {
            requestSync();
        }
        if (_syncState == SyncState::WAITING && static_cast<int32_t>(now - _retryAt) >= 0)
        {
            configTime(GMT_OFFSET, DST_OFFSET, NTP_SERVER);
            ++_attemptCount;
            _syncState = SyncState::ACTIVE;
            _syncStart = now;
        }
    }

    void Service::retryOrFinish(uint32_t now)
    {
        if (_attemptCount >= SYNC_ATTEMPT_LIMIT)
        {
            Serial.println("NTP sync failed; attempts exhausted.");
            _syncState = SyncState::IDLE;
            _updateInterval = now;
            return;
        }
        Serial.println("NTP sync failed; retry scheduled.");
        _syncState = SyncState::WAITING;
        _retryAt = now + SYNC_RETRY_DELAY_MS;
    }

    uint64_t Service::getEpochTime()
    {
    return 0;//_rtc.getEpochTime();
    }

    void Service::setLocation(double latitude, double longitude)
    {
        _latitude = latitude;
        _longitude = longitude;
        _locationSet = true;
    }

    void Service::setTimezone(const char* posixTimezone)
    {
        strncpy(_timezone, posixTimezone, sizeof(_timezone) - 1U);
        _timezone[sizeof(_timezone) - 1U] = '\0';
    }

    bool Service::registerCallback(Event event, int32_t offsetSeconds, Callback callback)
    {
        if (_registrationCount >= MAX_REGISTRATIONS || !callback)
        {
            return false;
        }
        _registrations[_registrationCount++] = Registration{event, offsetSeconds, callback, 0};
        return true;
    }

    // The sync (configTime) resets TZ to GMT, so the timezone is re-applied whenever it differs.
    void Service::applyTimezone()
    {
        const char* current = getenv("TZ");
        if (current == nullptr || strcmp(current, _timezone) != 0)
        {
            setenv("TZ", _timezone, 1);
            tzset();
        }
    }

    bool Service::getEventTime(Event event, time_t reference, time_t* result)
    {
        applyTimezone();
        tm local;
        localtime_r(&reference, &local);

        if (event == Event::MIDNIGHT)
        {
            local.tm_hour = 0;
            local.tm_min = 0;
            local.tm_sec = 0;
            local.tm_isdst = -1;
            *result = mktime(&local);
            return true;
        }

        if (!_locationSet)
        {
            return false;
        }
        local.tm_hour = 12;
        local.tm_min = 0;
        local.tm_sec = 0;
        local.tm_isdst = -1;
        const time_t localNoon = mktime(&local);
        const time_t utcMidnight = localNoon - (localNoon % 86400);
        double minutes;
        if (!sunMinutesUtc(_latitude, _longitude, utcMidnight + 43200, event == Event::SUNRISE, &minutes))
        {
            return false;
        }
        *result = utcMidnight + static_cast<time_t>(lround(minutes * 60.0));
        return true;
    }

    void Service::runSchedule()
    {
        if (!_valid || _registrationCount == 0U)
        {
            return;
        }

        time_t now;
        time(&now);
        applyTimezone();
        tm local;
        localtime_r(&now, &local);
        const int32_t today = dayKey(local);

        uint8_t due[MAX_REGISTRATIONS];
        time_t dueAt[MAX_REGISTRATIONS];
        uint8_t dueCount = 0U;
        for (uint8_t index = 0U; index < _registrationCount; ++index)
        {
            time_t trigger;
            if (_registrations[index].lastFiredDay == today ||
                !getEventTime(_registrations[index].event, now, &trigger))
            {
                continue;
            }
            trigger += _registrations[index].offset;
            if (trigger > now)
            {
                continue;
            }
            // Insertion sort so callbacks fire in the order the events happened.
            uint8_t position = dueCount++;
            while (position > 0U && dueAt[position - 1U] > trigger)
            {
                due[position] = due[position - 1U];
                dueAt[position] = dueAt[position - 1U];
                --position;
            }
            due[position] = index;
            dueAt[position] = trigger;
        }

        // After boot the earlier events of today are stale; only the latest one describes the present.
        uint8_t first = 0U;
        if (!_scheduleSynced)
        {
            _scheduleSynced = true;
            first = dueCount > 0U ? dueCount - 1U : 0U;
            for (uint8_t index = 0U; index < first; ++index)
            {
                _registrations[due[index]].lastFiredDay = today;
            }
        }
        for (uint8_t index = first; index < dueCount; ++index)
        {
            _registrations[due[index]].lastFiredDay = today;
            _registrations[due[index]].callback();
        }
    }

    bool Service::getValid()
    {
        return _valid;
    }

    void Service::requestSync()
    {
        if (isSyncPending())
        {
            return;
        }
        _syncState = SyncState::WAITING;
        _attemptCount = 0U;
        _requestStart = millis();
        _retryAt = _requestStart;
    }

    bool Service::isSyncPending() const
    {
        return _syncState != SyncState::IDLE;
    }

}