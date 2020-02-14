#include "timer.chrono.h"

#include <chrono>
#include <thread>

using std::chrono::steady_clock;
using std::chrono::milliseconds;
using std::this_thread::sleep_for;

TimerChrono::TimerChrono() : Timer() {
}

TimerChrono::~TimerChrono() {
}

void TimerChrono::start() {
    start_time = steady_clock::now();
}

float TimerChrono::get_delta() {
    steady_clock::time_point current_time = steady_clock::now();
    steady_clock::duration time_span = current_time - start_time;
    return float(time_span.count()) * steady_clock::period::num
                                    / steady_clock::period::den;
}

void TimerChrono::sleep_until(const float min_delta) {
    float delta = get_delta();
    if (delta < min_delta) {
        int sleep_ms = (int)(1000.f * min_delta - delta);
        sleep_for(milliseconds(sleep_ms));
    }
}
