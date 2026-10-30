#ifndef _SERVICES_NOW_H
#define _SERVICES_NOW_H

#include "Bee32Config.h"

#if CONFIG_SERVICE_NOW

#include <stdint.h>
#include <stddef.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

namespace SSL::NOW
{
    constexpr size_t MAC_LENGTH = 6U;
    constexpr size_t MAX_PAYLOAD = 250U;

    class Listener
    {
        public:
            virtual void OnNowDataReceivedCallback(const uint8_t mac[MAC_LENGTH], const uint8_t* data, size_t length) = 0;
    };

    // ESP-NOW transport: peers, unicast/broadcast send and receive. It has no knowledge of message content.
    // Wi-Fi must be in STA (or AP+STA) mode; peers use the current Wi-Fi channel, so both ends must share it.
    class Service
    {
        public:
            Service();
            ~Service() = default;

            bool begin();
            void end();

            // Delivers queued messages to the listener on the caller's task.
            void run();

            void registerINow(NOW::Listener* listener);
            void unregisterINow();

            bool addPeer(const uint8_t mac[MAC_LENGTH]);
            bool removePeer(const uint8_t mac[MAC_LENGTH]);
            bool send(const uint8_t mac[MAC_LENGTH], const void* data, size_t length);
            bool broadcast(const void* data, size_t length);

        private:
            struct Message
            {
                uint8_t mac[MAC_LENGTH];
                uint8_t length;
                uint8_t data[MAX_PAYLOAD];
            };

            static constexpr UBaseType_t QUEUE_LENGTH = 8U;

            // ESP-NOW callbacks run on the Wi-Fi task and carry no user context.
            static Service* _instance;
            static void onReceive(const uint8_t* mac, const uint8_t* data, int length);

            QueueHandle_t _queue;
            NOW::Listener* _inow;
            bool _started;
    };
}

#endif

#endif
