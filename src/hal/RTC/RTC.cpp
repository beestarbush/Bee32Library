#ifdef CONFIG_HAL_DRIVER_DS3231
#include "RTC.h"
#include <hal/I2C/I2C.h>

namespace HAL::RTC
{
    Driver::Driver(I2C::Driver& i2c) :
        _i2c(i2c),
        _ds3231()
    {
    }

    void Driver::begin()
    {
        _ds3231.begin(_i2c.getWire());
    }

    void Driver::end()
    {
    }

    void Driver::adjust(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
    {
        Serial.println(year);
        Serial.println(month);
        Serial.println(day);
        // Input is years since 1900, DateTime expects years since 2000.
        // Input is months starting at 0, DateTime expects months from 1.
        DateTime time(year - 100, month + 1, day, hour, min, sec);
        Serial.println(time.year());
        Serial.println(time.month());
        Serial.println(time.day());
        _ds3231.adjust(time);
    }

    void Driver::reset()
    {
        _ds3231.adjust(DateTime(2024, 3, 1, 1, 1, 1));
    }

    uint64_t Driver::getEpochTime()
    {
        DateTime now = _ds3231.now();
        return static_cast<uint64_t>(now.unixtime());
    }

    bool Driver::getLostPower()
    {
        return _ds3231.lostPower();
    }

    uint32_t Driver::getTemperature()
    {
        return _ds3231.getTemperature() * 100;
    }

    bool Driver::isValidDateTime()
    {
        DateTime now = _ds3231.now();
        Serial.println(now.year());
        return now.year() >= 2020;
    }
}
#endif