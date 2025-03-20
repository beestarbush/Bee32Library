#include "NVS.h"
#include "Arduino.h"
#include "Preferences.h"

namespace SSL::NVS
{
    Service::Service() :
        _prf()
    {
    }

    void Service::begin(const char* name)
    {
        _prf.begin(name, false);
    }

    void Service::end()
    {
        _prf.end();
    }

    bool Service::write(const char* key, uint32_t value)
    {
        return _prf.putUInt(key, value) == sizeof(uint32_t);
    }

    bool Service::write(const char* key, uint64_t value)
    {
        return _prf.putULong64(key, value) == sizeof(uint64_t);
    }

    bool Service::write(const char* key, const char* value)
    {
        return _prf.putString(key, value) == strlen(value);
    }

    bool Service::read(const char* key, uint32_t* value)
    {
        if (!contains(key))
        {
            return false;
        }
        *value = _prf.getUInt(key);
        return true;
    }

    bool Service::read(const char* key, uint64_t* value)
    {
        if (!contains(key))
        {
            return false;
        }
        *value = _prf.getULong64(key);
        return true;
    }

    bool Service::read(const char* key, char* value, uint32_t bufferSize)
    {
        if (!contains(key))
        {
            return false;
        }
        String storedValue = _prf.getString(key);
        storedValue.toCharArray(value, bufferSize);
        return true;
    }

    bool Service::remove(const char* key)
    {
        if (!contains(key))
        {
            return false;
        }
        return _prf.remove(key);
    }


    bool Service::contains(const char* key)
    {
        return _prf.isKey(key);
    }
}