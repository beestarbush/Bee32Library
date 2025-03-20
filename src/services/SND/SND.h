#ifndef _SERVICES_SND_H
#define _SERVICES_SND_H

#include "stdint.h"
#include <hal/BUZ/BUZ.h>

namespace SSL::SND
{
    const uint8_t MAX_NOTES = 10;

    enum MelodyId
    {
        SHORT_BEEP,
        LONGER,
        MELODY_COUNT
    };

    struct Melody
    {
        uint32_t noteCount;
        HAL::BUZ::Note notes[MAX_NOTES];
        uint32_t duration[MAX_NOTES];
    };

    class Service
    {
        public:
            Service(HAL::BUZ::Driver& buzzerDriver);
            ~Service() = default;

            void begin();
            void end();

            void play(MelodyId id);

        private:
            HAL::BUZ::Driver& _buzzerDriver;
            Melody _melodies[MELODY_COUNT];
    };
}

#endif