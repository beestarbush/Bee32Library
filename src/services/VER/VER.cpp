#include "VER.h"
#include "Arduino.h"

namespace SSL::VER
{
    Service::Service()
    {
    }

    void Service::begin()
    {
        Serial.print("Version: ");
        Serial.println(getTag());
    }

    void Service::end()
    {
    }

    String Service::getTag()
    {
        return REPO_TAG;
    }

    String Service::getVersion()
    {
        return REPO_VERSION;
    }

    String Service::getDate()
    {
        return REPO_DATE;
    }
}