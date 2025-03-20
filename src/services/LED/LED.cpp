#include "LED.h"
#include <hal/PWM/PWM.h>
#include "Arduino.h"

uint32_t brightness = 0;
int32_t fadeAmount = 5;
uint64_t _updateInterval;
constexpr uint64_t UPDATE_INTERVAL = 30;

namespace SSL::LED
{
    Service::Service(HAL::PWM::Driver& modeLed) :
        _modeLed(modeLed),
        _state(OFF)
    {
    }

    void Service::begin()
    {
        _state = State::OFF;
    }

    void Service::end()
    {
    }

    void Service::run()
    {
        // Periodically update the LEDs.
        uint64_t now = millis();
        if (now - _updateInterval < UPDATE_INTERVAL)
        {
            return;
        }

        _updateInterval = now;

        switch (_state)
        {
            case OFF:
            {
                _modeLed.setValue(0);
            }
            break;

            case ON:
            {
                _modeLed.setValue(255);
            }
            break;

            case PULSE:
            {
                _modeLed.setValue(brightness);

                // Increase brightness
                brightness += fadeAmount;

                if (brightness <= 0 || brightness >= 255)
                {
                    fadeAmount = -fadeAmount;
                }
            }
            break;

            default:
            break;
        }
    }

    void Service::set(State state)
    {
        if (_state != state)
        {
            _state = state;
        }
    }
}