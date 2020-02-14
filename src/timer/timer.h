#ifndef GUARD_TIMER_H
#define GUARD_TIMER_H

class Timer {
    public:
        Timer() {};
        virtual ~Timer() {};
        virtual void start() = 0;
        virtual float get_delta() = 0;
        virtual void sleep_until(float) = 0;
};

#ifdef LINUX
#include "timer.chrono.h"
#endif

#endif
