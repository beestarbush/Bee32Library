#ifndef _HAL_SSD1306_H
#define _HAL_SSD1306_H

#ifdef CONFIG_HAL_DRIVER_SSD1306
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#endif

#include "hal/I2C/I2C.h"

namespace HAL::SSD1306
{
    class Configuration
    {
        public:
            Configuration(uint8_t width, uint8_t height, uint8_t address);

            friend class Driver;

        private:
            uint8_t _width;
            uint8_t _height;
            uint8_t _address;
    };
#ifdef CONFIG_HAL_DRIVER_SSD1306
    class Driver : public Adafruit_SSD1306
    {
        using Base = Adafruit_SSD1306;
        public:
            Driver(const Configuration& configuration, I2C::Driver& i2c);

            bool begin();

        private:
            const Configuration _configuration;
    };
    #else
    class Driver
    {
        public:
            Driver(const Configuration& configuration, I2C::Driver& i2c);

            bool begin();

        private:
            const Configuration _configuration;
    };
    #endif
}

#endif