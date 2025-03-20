#ifndef _HAL_EINK_H
#define _HAL_EINK_H

#include <GxEPD2_3C.h>

#define MAX_DISPLAY_BUFFER_SIZE 65536ul // e.g.
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))

namespace HAL::EINK
{
    class Configuration
    {
        public:
            Configuration(uint16_t width, uint16_t height, uint8_t address, uint8_t cs, uint8_t dc, uint8_t rst, uint8_t busy);
            friend class Driver;

        private:
            uint16_t _width;
            uint16_t _height;
            uint8_t _address;
            uint8_t _cs;
            uint8_t _dc;
            uint8_t _rst;
            uint8_t _busy;
    };

    class Driver : public GxEPD2_3C<GxEPD2_290_C90c, MAX_HEIGHT(GxEPD2_290_C90c)>
    {
        using Base = GxEPD2_3C<GxEPD2_290_C90c, MAX_HEIGHT(GxEPD2_290_C90c)>;
        public:
            Driver(const Configuration& configuration, SPIClass& spi);

            bool begin();

        private:
            const Configuration _configuration;
            SPIClass _hspi;
    };
}

#endif