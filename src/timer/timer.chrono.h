#ifndef GUARD_TIMER_CHRONO_H
#define GUARD_TIMER_CHRONO_H

#include "timer.h"

#include <chrono>

using std::chrono::steady_clock;

class TimerChrono : public Timer {
    public:
        TimerChrono();
        virtual ~TimerChrono() override;
        virtual void start() override;
        virtual float get_delta() override;
        virtual void sleep_until(float) override;
    private:
        steady_clock::time_point start_time;
};

#endif
