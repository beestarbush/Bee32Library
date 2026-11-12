#ifndef _SERVICES_TIM_H
#define _SERVICES_TIM_H

#include <stdint.h>
#include <time.h>
#include <functional>

namespace HAL
{
    namespace WIFI
    {
        class Driver;
    }
}

namespace SSL::TIM
{
    class Service
    {
        public:
            // Daily events a callback can be registered on. Sunrise and sunset are calculated
            // from the configured location, so no internet is needed after the clock is valid.
            enum class Event : uint8_t
            {
                SUNRISE,
                SUNSET,
                MIDNIGHT // Local midnight, the start of the local day.
            };

            using Callback = std::function<void()>;

            Service(HAL::WIFI::Driver& wifi);
            ~Service() = default;

            void begin();
            void end();
            void run();
            void requestSync();
            bool isSyncPending() const;

            // Latitude and longitude in decimal degrees (east positive).
            void setLocation(double latitude, double longitude);
            // POSIX TZ string. Local midnight and the local day are derived from it.
            void setTimezone(const char* posixTimezone);

            // The callback fires once per local day when the event time plus the offset has passed.
            // After the first valid time only the most recent due event fires, so the state of
            // the callers matches the time of day after a reboot. Returns false when full.
            bool registerCallback(Event event, int32_t offsetSeconds, Callback callback);

            // Epoch time of the event on the local day containing reference. Returns false when
            // the event does not occur (polar day or night) or the location is not set.
            bool getEventTime(Event event, time_t reference, time_t* result);

            uint64_t getEpochTime();
            bool getValid();

        private:
            enum class SyncState : uint8_t
            {
                IDLE,
                WAITING,
                ACTIVE
            };

            struct Registration
            {
                Event event;
                int32_t offset;
                Callback callback;
                int32_t lastFiredDay;
            };

            static constexpr uint8_t MAX_REGISTRATIONS = 8U;

            void retryOrFinish(uint32_t now);
            void runSchedule();
            void applyTimezone();

            HAL::WIFI::Driver& _wifi;
            bool _valid;
            SyncState _syncState;
            uint8_t _attemptCount;
            uint32_t _updateInterval;
            uint32_t _requestStart;
            uint32_t _syncStart;
            uint32_t _retryAt;
            double _latitude;
            double _longitude;
            bool _locationSet;
            char _timezone[64];
            Registration _registrations[MAX_REGISTRATIONS];
            uint8_t _registrationCount;
            bool _scheduleSynced;
    };
}

#endif