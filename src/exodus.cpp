#include "exodus.h"

#include "platform.h"
#include "timer/timer.h"

#include <csignal>

const float MAX_FPS = 60.f;
const float MIN_FRAME_DELTA = 1.f / MAX_FPS;

volatile bool running;

void signal_handler(int signum) {
    running = false;
}

Exodus::Exodus() {
}

Exodus::~Exodus() {
}

bool Exodus::init() {
    signal(SIGINT, signal_handler);
    return true;
}

int Exodus::run(int argc, char** argv) {
    if (!init()) {
        return 1;
    }

    running = true;

    TIMER timer;
    float delta_time = 0.f;

    while (running) {
        timer.start();
        // TODO: The game.
        timer.sleep_until(MIN_FRAME_DELTA);
        delta_time = timer.get_delta();
    }

    return 0;
}
