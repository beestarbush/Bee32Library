#ifndef _SERVICES_TIM_H
#define _SERVICES_TIM_H

#include <stdint.h>

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
            Service(HAL::WIFI::Driver& wifi);
            ~Service() = default;

            void begin();
            void end();
            void run();
            void requestSync();
            bool isSyncPending() const;

            uint64_t getEpochTime();
            bool getValid();

        private:
            enum class SyncState : uint8_t
            {
                IDLE,
                WAITING,
                ACTIVE
            };

            void retryOrFinish(uint32_t now);

            HAL::WIFI::Driver& _wifi;
            bool _valid;
            SyncState _syncState;
            uint8_t _attemptCount;
            uint32_t _updateInterval;
            uint32_t _requestStart;
            uint32_t _syncStart;
            uint32_t _retryAt;
    };
}

#endif