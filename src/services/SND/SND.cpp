#include "SND.h"
#include "Arduino.h"
using namespace HAL;

namespace SSL::SND
{
    Service::Service(BUZ::Driver& buzzerDriver) :
        _buzzerDriver(buzzerDriver),
        _melodies{
            {
                2,
                {BUZ::NOTE_C6, BUZ::NOTE_G6},
                {8,8}
            },
            {
                8,
                {BUZ::NOTE_C4, BUZ::NOTE_G3, BUZ::NOTE_G3, BUZ::NOTE_A3, BUZ::NOTE_G3, BUZ::NO_NOTE, BUZ::NOTE_B3, BUZ::NOTE_C4},
                {4, 8, 8, 4, 4, 4, 4, 4}
            }}
    {
    }

    void Service::begin()
    {
    }

    void Service::end()
    {
    }

    void Service::play(MelodyId id)
    {
        Melody melody = _melodies[id];
        for (uint32_t i = 0; i < melody.noteCount; i++)
        {
            _buzzerDriver.play(melody.notes[i], melody.duration[i]);

            int noteDuration = 1000 / melody.duration[i];
            int pauseBetweenNotes = noteDuration * 1.30;
            delay(pauseBetweenNotes);
            _buzzerDriver.stop();
        }
    }
}