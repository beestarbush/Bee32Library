#ifndef _SERVICES_NTF_H
#define _SERVICES_NTF_H

#include <stdint.h>
#include <vector>
#include <functional>

namespace SSL::NTF
{
    class Service
    {
        public:
            enum class Severity : uint8_t
            {
                High,
                Medium,
                Low,
                Info
            };

            static constexpr uint32_t DISPLAY_MS = 3000U;
            static constexpr uint32_t TEXT_SIZE = 64U;

            struct Notification
            {
                uint32_t id;
                uint32_t activation;
                Severity severity;
                char text[TEXT_SIZE];
            };

            enum class EventType : uint8_t
            {
                Activated,
                Deactivated,
                SelectionChanged
            };

            struct Event
            {
                EventType type;
                bool hasNotification;
                Notification notification;
            };

            using Listener = std::function<void(const Event&)>;
            uint32_t registerNotificationListener(Listener listener);
            void unregisterNotificationListener(uint32_t listenerId);

            void begin();
            void run(bool presenting = true);
            void end();
            bool activate(uint32_t id, Severity severity, const char* text);
            void deactivate(uint32_t id);
            bool isActive(uint32_t id) const;
            uint32_t getCount() const;
            const Notification* getAt(uint32_t index) const;
            const Notification* getCurrent() const;

        private:
            struct Entry
            {
                Notification notification;
                bool pending;
            };

            struct Registration
            {
                uint32_t id;
                Listener listener;
            };

            void select(uint32_t activation);
            void dispatchEvents();

            std::vector<Entry> _queue;
            std::vector<Registration> _listeners;
            std::vector<Event> _events;
            uint32_t _nextListener = 0U;
            uint32_t _nextActivation = 0U;
            uint32_t _currentActivation = 0U;
            uint32_t _shownAt = 0U;
            bool _initialDisplay = false;
    };
}

#endif