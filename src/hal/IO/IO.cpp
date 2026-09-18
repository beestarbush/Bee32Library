#include "IO.h"
#include <Arduino.h>

#define DEBOUNCE_DELAY (50)

namespace HAL::IO
{
    Configuration::Configuration(uint8_t pin, uint8_t mode, bool activeHigh) :
        _pin(pin),
        _mode(mode),
        _activeHigh(activeHigh)
    {    
    }

    Driver::Driver(const Configuration& configuration) :
        _configuration(configuration),
        _value(false),
        _lastReadValue(false),
        _lastDebounceTime(0),
        _callback(nullptr)
    {
    }

    void Driver::begin()
    {
        pinMode(_configuration._pin, _configuration._mode);
        _lastReadValue = digitalRead(_configuration._pin);
        _lastDebounceTime = 0;
    }

    void Driver::end()
    {
    }

    void Driver::run()
    {
        bool currentValue = digitalRead(_configuration._pin);
        unsigned long now = millis();

        // Check for a change in pin state
        if (currentValue != _lastReadValue)
        {
            // Reset the debounce timer if the state has changed
            _lastDebounceTime = now;
        }

        // Only consider the pin state if the debounce delay has passed
        if (((now - _lastDebounceTime) > DEBOUNCE_DELAY) && (currentValue != _value))
        {
            // Update the value based on active high or low configuration
            if (_configuration._activeHigh)
            {
                _value = currentValue;
            }
            else
            {
                _value = !currentValue;
            }
            if (_callback)
            {
                _callback(_value);
            }
        }

        // Store the last read value for comparison
        _lastReadValue = currentValue;
    }

    void Driver::registerCallback(Callback callback)
    {
        _callback = callback;
    }

    bool Driver::getValue()
    {
        return _value;
    }
}