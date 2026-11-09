#include "services/NTF/NTF.h"

#include <Arduino.h>
#include <cstring>

namespace SSL::NTF
{
    uint32_t Service::registerNotificationListener(Listener listener)
    {
        const uint32_t id = ++_nextListener;
        _listeners.push_back({id, std::move(listener)});
        return id;
    }

    void Service::unregisterNotificationListener(uint32_t listenerId)
    {
        for (auto registration = _listeners.begin(); registration != _listeners.end(); ++registration)
        {
            if (registration->id == listenerId)
            {
                _listeners.erase(registration);
                return;
            }
        }
    }

    void Service::select(uint32_t activation)
    {
        if (_currentActivation == activation)
        {
            return;
        }
        _currentActivation = activation;
        const Notification* current = getCurrent();
        _events.push_back({EventType::SelectionChanged, current != nullptr, current == nullptr ? Notification{} : *current});
    }

    void Service::dispatchEvents()
    {
        std::vector<Event> events;
        events.swap(_events);
        const auto listeners = _listeners;
        for (const Event& event : events)
        {
            for (const Registration& registration : listeners)
            {
                for (const Registration& active : _listeners)
                {
                    if (active.id == registration.id)
                    {
                        registration.listener(event);
                        break;
                    }
                }
            }
        }
    }

    void Service::begin()
    {
        for (const Entry& entry : _queue)
        {
            _events.push_back({EventType::Deactivated, true, entry.notification});
        }
        select(0U);
        _queue.clear();
        _nextActivation = 0U;
        _currentActivation = 0U;
        _initialDisplay = false;
    }

    void Service::end()
    {
        begin();
    }

    bool Service::activate(uint32_t id, Severity severity, const char* text)
    {
        if (isActive(id))
        {
            return true;
        }
        if (text == nullptr || std::strlen(text) >= TEXT_SIZE)
        {
            return false;
        }
        Entry entry{};
        entry.notification.id = id;
        entry.notification.activation = ++_nextActivation;
        if (entry.notification.activation == 0U)
        {
            entry.notification.activation = ++_nextActivation;
        }
        entry.notification.severity = severity;
        std::strcpy(entry.notification.text, text);
        entry.pending = true;
        _queue.push_back(entry);
        _events.push_back({EventType::Activated, true, entry.notification});
        return true;
    }

    void Service::deactivate(uint32_t id)
    {
        for (auto entry = _queue.begin(); entry != _queue.end(); ++entry)
        {
            if (entry->notification.id == id)
            {
                if (_currentActivation == entry->notification.activation)
                {
                    select(0U);
                    _initialDisplay = false;
                }
                _events.push_back({EventType::Deactivated, true, entry->notification});
                _queue.erase(entry);
                return;
            }
        }
    }

    bool Service::isActive(uint32_t id) const
    {
        for (const Entry& entry : _queue)
        {
            if (entry.notification.id == id)
            {
                return true;
            }
        }
        return false;
    }

    uint32_t Service::getCount() const
    {
        return static_cast<uint32_t>(_queue.size());
    }

    const Service::Notification* Service::getAt(uint32_t index) const
    {
        return index < _queue.size() ? &_queue[index].notification : nullptr;
    }

    const Service::Notification* Service::getCurrent() const
    {
        for (const Entry& entry : _queue)
        {
            if (entry.notification.activation == _currentActivation)
            {
                return &entry.notification;
            }
        }
        return nullptr;
    }

    void Service::run(bool presenting)
    {
        if (!presenting)
        {
            dispatchEvents();
            return;
        }
        const uint32_t now = static_cast<uint32_t>(millis());
        const Notification* current = getCurrent();
        if (_initialDisplay && current != nullptr)
        {
            if (now - _shownAt < DISPLAY_MS)
            {
                dispatchEvents();
                return;
            }
            const uint32_t id = current->id;
            const bool info = current->severity == Severity::Info;
            _initialDisplay = false;
            if (info)
            {
                deactivate(id);
            }
        }
        for (Entry& entry : _queue)
        {
            if (entry.pending)
            {
                entry.pending = false;
                select(entry.notification.activation);
                _shownAt = now;
                _initialDisplay = true;
                dispatchEvents();
                return;
            }
        }
        const Notification* selected = nullptr;
        for (const Entry& entry : _queue)
        {
            if (selected == nullptr || entry.notification.severity < selected->severity)
            {
                selected = &entry.notification;
            }
        }
        select(selected == nullptr ? 0U : selected->activation);
        dispatchEvents();
    }
}