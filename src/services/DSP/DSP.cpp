#include "DSP.h"

#define YEARS_SINCE_1900_OFFSET (70)
#define YEARS_BASE_OFFSET (1900)
#define MONTH_BASE_OFFSET (1)
//#define DIGIT_DEBUG

namespace SSL::DSP
{
    Service::Service(display_config_t configuration) :
        _cfg(configuration),
        _mode(OFF),
        _timeSinceItem(0),
        _invalidItem(false),
        _errorItem(NO_ERROR),
        _numberItem(0),
        _resetItem(false),
        _ipAddressItem(IPAddress(0,0,0,0)),
        _versionItem(""),
        _connectedItem(false),
        _temperatureItem(0),
        _dateTimeSyncedItem(false),
        _dateItem(0),
        _timeItem(0)
    {
    }

    void Service::begin()
    {
        _timeSinceItem.reset();
        _invalidItem.reset();
        _errorItem.reset();
        _numberItem.reset();
        _resetItem.reset();
        _ipAddressItem.reset();
        _versionItem.reset();
        _connectedItem.reset();
        _temperatureItem.reset();
        _dateTimeSyncedItem.reset();
        _dateItem.reset();
        _timeItem.reset();
    }

    void Service::end()
    {
    }

    void Service::run()
    {
    }

    void Service::showTimeSince(uint64_t timeSince)
    {
        // _timeSinceItem.update(timeSince);

        // if (_timeSinceItem.isChanged() || _mode != TIMESINCE)
        // {
        //     // Convert from epoch digits to usable elements for the display.
        //     time_t seconds = _timeSinceItem.get();
        //     struct tm time;
        //     localtime_r(&seconds, &time);

        //     // tm_year contains years since 1900, epoch always contains 1970 so substract 70 of the amount of years.
        //     // The firest year is a one-off since if we would multiply the days with zero, it will become zero days.
        //     uint16_t yearMultiplier = 1;
        //     if (time.tm_year != YEARS_SINCE_1900_OFFSET)
        //     {
        //         yearMultiplier = (time.tm_year - YEARS_SINCE_1900_OFFSET);
        //     }

        //     setDays(time.tm_yday * yearMultiplier);
        //     setHours(time.tm_hour);
        //     setMinutes(time.tm_min);
        //     setSeconds(time.tm_sec);

        //     // Change mode.
        //     _mode = TIMESINCE;
        // }
    }

    void Service::showInvalidTimeSince()
    {
        // _invalidItem.update(true);

        // if (_invalidItem.isChanged() || _mode != INVALID)
        // {
        //     // Reset to indicate that we consumed the value.
        //     _invalidItem.reset();

        //     for (uint8_t i = 0; i < _maxA.getConnectedDigits(); i++)
        //     {
        //         _maxA.setChar(i, '-', false);
        //     }
        //     for (uint8_t i = 0; i < _maxB.getConnectedDigits(); i++)
        //     {
        //         _maxB.setChar(i, '-', false);
        //     }

        //     // Change mode.
        //     _mode = INVALID;
        // }
    }

    void Service::showError(Error error)
    {
        // _errorItem.update(error);

        // if (_errorItem.isChanged() || _mode != ERROR)
        // {
        //     uint8_t errorCode[2] = { 0, 0 };
        //     splitDigits(_errorItem.get(), errorCode, 2);

        //     for (uint8_t i = 4; i < _maxA.getConnectedDigits(); i++)
        //     {
        //         _maxA.setChar(i, ' ', false);
        //     }
        //     for (uint8_t i = 0; i < _maxB.getConnectedDigits(); i++)
        //     {
        //         _maxB.setChar(i, ' ', false);
        //     }

        //     _maxA.setChar(_cfg.minutes[0].digit, '-', false);
        //     _maxA.setChar(_cfg.minutes[1].digit, 'E', false);
        //     _maxA.setChar(_cfg.seconds[0].digit, errorCode[0], false);
        //     _maxA.setChar(_cfg.seconds[1].digit, errorCode[1], false);

        //     // Change mode.
        //     _mode = ERROR;
        // }
    }

    void Service::showNumber(uint64_t number)
    {
        _numberItem.update(number);

        if (_numberItem.isChanged() || _mode != NUMBER)
        {
            setDigit(_numberItem.get());
            _mode = NUMBER;
        }
    }

    void Service::showReset()
    {
        _resetItem.update(true);

        if (_resetItem.isChanged() || _mode != RESET)
        {
            // Reset to indicate that we consumed the value.
            _resetItem.reset();

            // for (uint8_t i = 2; i < _maxA.getConnectedDigits(); i++)
            // {
            //     _maxA.setChar(i, ' ', false);
            // }
            // for (uint8_t i = 0; i < _maxB.getConnectedDigits(); i++)
            // {
            //     _maxB.setChar(i, ' ', false);
            // }

            // _maxA.setChar(_cfg.seconds[1].digit, 'A', false); // A pretending an R on seven segment display.
            // _maxA.setDigit(_cfg.seconds[0].digit, 5, false); // 5 pretending an S on seven segment display.

            // Change mode.
            _mode = RESET;
        }
    }

    void Service::showIpAddress(IPAddress address)
    {
        _ipAddressItem.update(address);

        if (_ipAddressItem.isChanged() || _mode != IP_ADDRESS)
        {
            setIpAddress(_ipAddressItem.get());
            _mode = IP_ADDRESS;
        }
    }

    void Service::showVersion(String version)
    {
        _versionItem.update(version);

        if (_versionItem.isChanged() || _mode != VERSION)
        {
            setVersion(_versionItem.get());
            _mode = VERSION;
        }
    }

    void Service::showConnected(bool connected)
    {
        _connectedItem.update(connected);

        if (_connectedItem.isChanged() || _mode != CONNECTED)
        {
            setBoolean(_connectedItem.get());
            _mode = CONNECTED;
        }
    }

    void Service::showTemperature(uint32_t temperature)
    {
        _temperatureItem.update(temperature);

        if (_temperatureItem.isChanged() || _mode != TEMPERATURE)
        {
            setDigit(_temperatureItem.get());
            _mode = TEMPERATURE;
        }
    }

    void Service::showTimeSynced(bool synced)
    {
        _dateTimeSyncedItem.update(synced);

        if (_dateTimeSyncedItem.isChanged() || _mode != DATETIME_SYNCED)
        {
            setBoolean(_dateTimeSyncedItem.get());
            _mode = DATETIME_SYNCED;
        }
    }

    void Service::showDate(uint64_t date)
    {
        _dateItem.update(date);

        // if (_dateItem.isChanged() || _mode != DATE)
        // {
        //     time_t seconds = _dateItem.get();
        //     struct tm time;
        //     localtime_r(&seconds, &time);

        //     setDate(time.tm_year + YEARS_BASE_OFFSET, time.tm_mon + MONTH_BASE_OFFSET, time.tm_mday);
        //     _mode = DATE;
        // }
    }

    void Service::showTime(uint64_t time)
    {
        _timeItem.update(time);

        // if (_timeItem.isChanged() || _mode != TIME)
        // {
        //     time_t seconds = _timeItem.get();
        //     struct tm time;
        //     localtime_r(&seconds, &time);

        //     setTime(time.tm_hour, time.tm_min);
        //     _mode = TIME;
        // }
    }

    void Service::setBrightness(uint8_t brightness)
    {
        // _maxA.setBrightness(brightness);
        // _maxB.setBrightness(brightness);
    }

    void Service::splitDigits(uint64_t number, uint8_t* digits, uint8_t digitCount)
    {
        uint8_t i = 0;
        while (number > 0 && i < digitCount)
        {
            digits[i++] = number % 10;
            number /= 10;
        }
    }

    void Service::setDays(uint16_t days)
    {
        uint8_t digits[5] = { 0, 0, 0, 0, 0};
        splitDigits(days, digits, 5);
        // _maxA.setDigit(_cfg.days[0].digit, digits[0], false);
        // _maxA.setDigit(_cfg.days[1].digit, digits[1], false);
        // _maxB.setDigit(_cfg.days[2].digit, digits[2], false);
        // _maxB.setDigit(_cfg.days[3].digit, digits[3], false);
        // _maxB.setDigit(_cfg.days[4].digit, digits[4], false);

    #ifdef DIGIT_DEBUG
        Serial.print("Days: ");
        Serial.print(digits[4]);
        Serial.print(digits[3]);
        Serial.print(digits[2]);
        Serial.print(digits[1]);
        Serial.println(digits[0]);
    #endif
    }

    void Service::setHours(uint8_t hours)
    {
        uint8_t digits[2] = { 0, 0 };
        splitDigits(hours, digits, 2);
        // _maxA.setDigit(_cfg.hours[0].digit, digits[0], false);
        // _maxA.setDigit(_cfg.hours[1].digit, digits[1], false);

    #ifdef DIGIT_DEBUG
        Serial.print("Hours: ");
        Serial.print(digits[1]);
        Serial.println(digits[0]);
    #endif
    }

    void Service::setMinutes(uint8_t minutes)
    {
        uint8_t digits[2] = { 0, 0 };
        splitDigits(minutes, digits, 2);
        // _maxA.setDigit(_cfg.minutes[0].digit, digits[0], false);
        // _maxA.setDigit(_cfg.minutes[1].digit, digits[1], false);

    #ifdef DIGIT_DEBUG
        Serial.print("Minutes: ");
        Serial.print(digits[1]);
        Serial.println(digits[0]);
    #endif
    }

    void Service::setSeconds(uint8_t seconds)
    {
        uint8_t digits[2] = { 0, 0 };
        splitDigits(seconds, digits, 2);
        // _maxA.setDigit(_cfg.seconds[0].digit, digits[0], false);
        // _maxA.setDigit(_cfg.seconds[1].digit, digits[1], false);

    #ifdef DIGIT_DEBUG
        Serial.print("Seconds: ");
        Serial.print(digits[1]);
        Serial.println(digits[0]);
    #endif
    }

    void Service::setDigit(uint64_t number)
    {
        uint8_t digits[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        splitDigits(number, digits, 11);
        // _maxA.setDigit(0, digits[0], false);
        // _maxA.setDigit(1, digits[1], false);
        // _maxA.setDigit(2, digits[2], false);
        // _maxA.setDigit(3, digits[3], false);
        // _maxA.setDigit(4, digits[4], false);
        // _maxA.setDigit(5, digits[5], false);
        // _maxA.setDigit(6, digits[6], false);
        // _maxA.setDigit(7, digits[7], false);
        // _maxB.setDigit(0, digits[8], false);
        // _maxB.setDigit(1, digits[9], false);
        // _maxB.setDigit(2, digits[10], false);

    #ifdef DIGIT_DEBUG
        Serial.print("Number: ");
        Serial.print(digits[10]);
        Serial.print(digits[9]);
        Serial.print(digits[8]);
        Serial.print(digits[7]);
        Serial.print(digits[6]);
        Serial.print(digits[5]);
        Serial.print(digits[4]);
        Serial.print(digits[3]);
        Serial.print(digits[2]);
        Serial.print(digits[1]);
        Serial.println(digits[0]);
    #endif
    }

    void Service::setBoolean(bool boolean)
    {
        // for (uint8_t i = 1; i < _maxA.getConnectedDigits(); i++)
        // {
        //     _maxA.setChar(i, ' ', false);
        // }
        // for (uint8_t i = 0; i < _maxB.getConnectedDigits(); i++)
        // {
        //     _maxB.setChar(i, ' ', false);
        // }

        // _maxA.setDigit(_cfg.seconds[0].digit, boolean ? 1 : 0, false);
    }

    void Service::setVersion(String version)
    {
        // for (uint8_t i = 3; i < _maxA.getConnectedDigits(); i++)
        // {
        //     _maxA.setChar(i, ' ', false);
        // }
        // for (uint8_t i = 0; i < _maxB.getConnectedDigits(); i++)
        // {
        //     _maxB.setChar(i, ' ', false);
        // }

        // Find first dot so we know where to look for the tag IDs.
        int index = version.indexOf('.');
        uint8_t patch = version.charAt(index + 3) - '0';
        uint8_t minor = version.charAt(index + 1) - '0';
        uint8_t major = version.charAt(index - 1) - '0';

        // _maxA.setDigit(_cfg.minutes[0].digit, major, true);
        // _maxA.setDigit(_cfg.seconds[1].digit, minor, true);
        // _maxA.setDigit(_cfg.seconds[0].digit, patch, false);
    }

    void Service::setIpAddress(IPAddress ipAddress)
    {
        uint8_t fourthOctet[3] = { 0, 0, 0};
        splitDigits(ipAddress[3], fourthOctet, 3);
        // _maxA.setDigit(0, fourthOctet[0], false);
        // _maxA.setDigit(1, fourthOctet[1], false);
        // _maxA.setDigit(2, fourthOctet[2], false);

        uint8_t thirdOctet[3] = { 0, 0, 0};
        splitDigits(ipAddress[2], thirdOctet, 3);
        // _maxA.setDigit(3, thirdOctet[0], true);
        // _maxA.setDigit(4, thirdOctet[1], false);
        // Skip the last digit from the third octet as we miss one digit to display the full IP.

        uint8_t secondOctet[3] = { 0, 0, 0};
        splitDigits(ipAddress[1], secondOctet, 3);
        // _maxA.setDigit(5, secondOctet[0], true);
        // _maxA.setDigit(6, secondOctet[1], false);
        // _maxA.setDigit(7, secondOctet[2], false);

        uint8_t firstOctet[3] = { 0, 0, 0};
        splitDigits(ipAddress[0], firstOctet, 3);
        // _maxB.setDigit(0, firstOctet[0], true);
        // _maxB.setDigit(1, firstOctet[1], false);
        // _maxB.setDigit(2, firstOctet[2], false);
    }

    void Service::setDate(uint32_t years, uint32_t months, uint32_t days)
    {
        // _maxA.setChar(_cfg.hours[1].digit, ' ', false);
        // _maxA.setChar(_cfg.hours[0].digit, ' ', false);
        // _maxB.setChar(2, ' ', false);

        uint8_t yearsDigits[4] = { 0, 0, 0, 0};
        splitDigits(years, yearsDigits, 4);
        // _maxB.setDigit(_cfg.days[3].digit, yearsDigits[3], false);
        // _maxB.setDigit(_cfg.days[2].digit, yearsDigits[2], false);
        // _maxA.setDigit(_cfg.days[1].digit, yearsDigits[1], false);
        // _maxA.setDigit(_cfg.days[0].digit, yearsDigits[0], true);

        uint8_t monthsDigits[2] = { 0, 0};
        splitDigits(months, monthsDigits, 2);
        // _maxA.setDigit(_cfg.minutes[1].digit, monthsDigits[1], false);
        // _maxA.setDigit(_cfg.minutes[0].digit, monthsDigits[0], true);

        uint8_t daysDigits[2] = { 0, 0};
        splitDigits(days, daysDigits, 2);
        // _maxA.setDigit(_cfg.seconds[1].digit, daysDigits[1], false);
        // _maxA.setDigit(_cfg.seconds[0].digit, daysDigits[0], false);
    }

    void Service::setTime(uint32_t hours, uint32_t minutes)
    {
        // for (uint8_t i = 5; i < _maxA.getConnectedDigits(); i++)
        // {
        //     _maxA.setChar(i, ' ', false);
        // }
        // for (uint8_t i = 0; i < _maxB.getConnectedDigits(); i++)
        // {
        //     _maxB.setChar(i, ' ', false);
        // }

        // uint8_t hourDigits[2] = { 0, 0};
        // splitDigits(hours, hourDigits, 2);
        // _maxA.setDigit(_cfg.minutes[1].digit, hourDigits[1], false);
        // _maxA.setDigit(_cfg.minutes[0].digit, hourDigits[0], true);

        // uint8_t minutesDigits[2] = { 0, 0};
        // splitDigits(minutes, minutesDigits, 2);
        // _maxA.setDigit(_cfg.seconds[1].digit, minutesDigits[1], false);
        // _maxA.setDigit(_cfg.seconds[0].digit, minutesDigits[0], false);
    }
}