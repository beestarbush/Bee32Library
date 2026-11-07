#include "TIM.h"
#include "time.h"
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
        _retryAt(0U)
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
    }

    void Service::end()
    {
        _syncState = SyncState::IDLE;
    }

    void Service::run()
    {
        const uint32_t now = millis();
        if (isSyncPending() && _attemptCount != 0U)
        {
            time_t epoch;
            time(&epoch);
            if (epoch >= 1704067200)
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