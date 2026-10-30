#include "NOW.h"

#if CONFIG_SERVICE_NOW

#include <string.h>

#include <WiFi.h>
#include <esp_now.h>
#include <esp_idf_version.h>

namespace SSL::NOW
{
    namespace
    {
        constexpr uint8_t BROADCAST_MAC[MAC_LENGTH] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    }

    Service* Service::_instance = nullptr;

    Service::Service() :
        _queue(nullptr),
        _inow(nullptr),
        _started(false)
    {
    }

    bool Service::begin()
    {
        if (_started)
        {
            return true;
        }

        if (WiFi.getMode() == WIFI_MODE_NULL)
        {
            WiFi.mode(WIFI_STA);
        }

        if (_queue == nullptr)
        {
            _queue = xQueueCreate(QUEUE_LENGTH, sizeof(Message));
            if (_queue == nullptr)
            {
                return false;
            }
        }

        if (esp_now_init() != ESP_OK)
        {
            return false;
        }

        _instance = this;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
        esp_now_register_recv_cb([](const esp_now_recv_info_t* info, const uint8_t* data, int length)
        {
            onReceive(info->src_addr, data, length);
        });
#else
        esp_now_register_recv_cb(onReceive);
#endif

        _started = addPeer(BROADCAST_MAC);
        if (!_started)
        {
            end();
        }
        return _started;
    }

    void Service::end()
    {
        esp_now_unregister_recv_cb();
        esp_now_deinit();
        _instance = nullptr;
        _started = false;
        if (_queue != nullptr)
        {
            xQueueReset(_queue);
        }
    }

    void Service::run()
    {
        if (_queue == nullptr)
        {
            return;
        }

        Message message;
        while (xQueueReceive(_queue, &message, 0) == pdTRUE)
        {
            if (_inow != nullptr)
            {
                _inow->OnNowDataReceivedCallback(message.mac, message.data, message.length);
            }
        }
    }

    void Service::registerINow(NOW::Listener* listener)
    {
        _inow = listener;
    }

    void Service::unregisterINow()
    {
        _inow = nullptr;
    }

    bool Service::addPeer(const uint8_t mac[MAC_LENGTH])
    {
        if (esp_now_is_peer_exist(mac))
        {
            return true;
        }

        esp_now_peer_info_t peer = {};
        memcpy(peer.peer_addr, mac, MAC_LENGTH);
        peer.channel = 0;
        peer.ifidx = WIFI_IF_STA;
        peer.encrypt = false;
        return esp_now_add_peer(&peer) == ESP_OK;
    }

    bool Service::removePeer(const uint8_t mac[MAC_LENGTH])
    {
        if (!esp_now_is_peer_exist(mac))
        {
            return true;
        }
        return esp_now_del_peer(mac) == ESP_OK;
    }

    bool Service::send(const uint8_t mac[MAC_LENGTH], const void* data, size_t length)
    {
        if (!_started || length > MAX_PAYLOAD)
        {
            return false;
        }
        return esp_now_send(mac, static_cast<const uint8_t*>(data), length) == ESP_OK;
    }

    bool Service::broadcast(const void* data, size_t length)
    {
        return send(BROADCAST_MAC, data, length);
    }

    void Service::onReceive(const uint8_t* mac, const uint8_t* data, int length)
    {
        Service* self = _instance;
        if (self == nullptr || length <= 0 || static_cast<size_t>(length) > MAX_PAYLOAD)
        {
            return;
        }

        Message message;
        memcpy(message.mac, mac, MAC_LENGTH);
        message.length = static_cast<uint8_t>(length);
        memcpy(message.data, data, static_cast<size_t>(length));
        // Drop when full: the receiver is too slow and newer status will follow.
        xQueueSend(self->_queue, &message, 0);
    }
}

#endif
