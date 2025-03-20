#ifndef _HAL_BLT_H
#define _HAL_BLT_H

namespace HAL::BLT
{
    class Driver
    {
    public:
        Driver();
        ~Driver() = default;

        void begin();
        void end();
        bool started();
    };
}

#endif // _HAL_BLT_H