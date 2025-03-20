#ifndef _SERVICES_DSP_H
#define _SERVICES_DSP_H

#include <stdint.h>
#include <WString.h>
#include <IPAddress.h>

namespace SSL::DSP
{
    enum Error
    {
        NO_ERROR = 0,
        INVALID_TIME,
        TRIGGER_TIME_READ_FAILED,
        TRIGGER_TIME_WRITE_FAILED,
        TRIGGER_TIME_REMOVE_FAILED,
        APP_MODE_READ_FAILED,
        APP_MODE_WRITE_FAILED,
        UNKNOWN_APP_MODE,
    };

    template <typename T>
    class DisplayItem
    {
    public:
        DisplayItem(T value) :
            _value(value),
            _updated(false)
        {
        };

        bool update(T value)
        {
            if (_value != value)
            {
                _value = value;
                _updated = true;
            }
            return _updated;
        };

        bool isChanged()
        {
            return _updated;
        }

        T get()
        {
            _updated = false;
            return _value;
        };

        void reset()
        {
            _updated = false;
        }

    private:
        T _value;
        bool _updated;
    };

    typedef struct
    {
        uint8_t digit;
    } segment_config_t;

    typedef struct
    {
        segment_config_t seconds[2];
        segment_config_t minutes[2];
        segment_config_t hours[2];
        segment_config_t days[5];
    } display_config_t;

    class Service
    {
        public:
            Service(display_config_t configuration);
            ~Service() = default;

            void begin();
            void end();
            void run();

            void showTimeSince(uint64_t timeSince);
            void showInvalidTimeSince();
            void showError(Error error);
            void showNumber(uint64_t number);
            void showReset();
            void showIpAddress(IPAddress address);
            void showVersion(String version);
            void showConnected(bool connected);
            void showTemperature(uint32_t temperature);
            void showTimeSynced(bool synced);
            void showDate(uint64_t date);
            void showTime(uint64_t time);

            void setBrightness(uint8_t brightness);

        private:
            enum Mode
            {
                OFF,
                TIMESINCE,
                INVALID,
                ERROR,
                NUMBER,
                RESET,
                IP_ADDRESS,
                VERSION,
                CONNECTED,
                TEMPERATURE,
                DATETIME_SYNCED,
                DATE,
                TIME
            };
            void splitDigits(uint64_t number, uint8_t* digits, uint8_t digitCount);

            void setDays(uint16_t days);
            void setHours(uint8_t hours);
            void setMinutes(uint8_t minutes);
            void setSeconds(uint8_t seconds);
            void setDigit(uint64_t number);
            void setBoolean(bool boolean);
            void setVersion(String version);
            void setIpAddress(IPAddress ipAddress);
            void setDate(uint32_t years, uint32_t months, uint32_t days);
            void setTime(uint32_t hours, uint32_t minutes);

        private:
            display_config_t _cfg;
            Mode _mode;
            DisplayItem<uint64_t> _timeSinceItem;
            DisplayItem<bool> _invalidItem;
            DisplayItem<Error> _errorItem;
            DisplayItem<uint64_t> _numberItem;
            DisplayItem<bool> _resetItem;
            DisplayItem<IPAddress> _ipAddressItem;
            DisplayItem<String> _versionItem;
            DisplayItem<bool> _connectedItem;
            DisplayItem<uint32_t> _temperatureItem;
            DisplayItem<bool> _dateTimeSyncedItem;
            DisplayItem<uint64_t> _dateItem;
            DisplayItem<uint64_t> _timeItem;
    };
}

#endif