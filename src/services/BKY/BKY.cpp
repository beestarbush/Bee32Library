#include "BKY.h"
#include "Arduino.h"

#define DEVICE_NAME "Bee-pad"
#define DEVICE_MANUFACTURER "Bijsterbosch"

namespace SSL::BKY
{
    Service::Service(HAL::BLT::Driver& bluetooth) :
        _bluetooth(bluetooth),
        _keyboard(DEVICE_NAME, DEVICE_MANUFACTURER)
    {
    }

    void Service::begin()
    {
        _keyboard.begin();
    }

    void Service::end()
    {
        _keyboard.end();
    }

    bool Service::isConnected()
    {
        return _keyboard.isConnected();
    }

    void Service::press(uint8_t key)
    {
        _keyboard.press(key);
    }

    void Service::write(uint8_t key)
    {
        _keyboard.write(key);
    }

    void Service::write(String text)
    {
        for (size_t i = 0; i < text.length(); ++i)
        {
            _keyboard.write(text[i]);
        }
    }

    void Service::release(uint8_t key)
    {
        _keyboard.release(key);
    }
}