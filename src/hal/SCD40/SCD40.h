#ifdef CONFIG_HAL_DRIVER_SCD40
#ifndef _HAL_SCD40_H
#define _HAL_SCD40_H

#include "hal/I2C/I2C.h"
#include <SensirionI2cScd4x.h>

namespace HAL::SCD40
{
    class Configuration
    {
        public:
            Configuration(uint8_t address);

            friend class Driver;

        private:
            uint8_t _address;
    };

    class Driver
    {
        public:
            Driver(const Configuration& configuration, I2C::Driver& i2c);

            bool begin();
            void run();
            void end();

            uint64_t getSerialNumber();
            uint16_t getCO2Concentration();
            float getTemperature();
            float getRelativeHumidity();

        private:
            const Configuration _configuration;
            I2C::Driver& _i2c;
            SensirionI2cScd4x _sensor;
            uint64_t _serialNumber;
            uint16_t _co2Concentration;
            float _temperature;
            float _relativeHumidity;
            uint64_t _updateInterval;
    };
}

#endif
#endif