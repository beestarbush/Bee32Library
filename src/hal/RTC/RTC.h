#ifdef CONFIG_HAL_DRIVER_DS3231
#ifndef _HAL_RTC_H
#define _HAL_RTC_H

#include "RTClib.h"

namespace HAL::I2C
{
    class Driver;
}

namespace HAL::RTC
{
    class Driver
    {
        public:
            Driver(I2C::Driver& i2c);

            void begin();
            void end();

            void adjust(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);
            void reset();
            uint64_t getEpochTime();
            bool getLostPower();
            uint32_t getTemperature();
            bool isValidDateTime();

        private:
            I2C::Driver& _i2c;
            RTC_DS3231 _ds3231;
    };
}

#endif
#endif