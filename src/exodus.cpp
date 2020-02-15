#include "exodus.h"

#include "shared.h"

#include "platform.h"
#include "timer/timer.h"
#include "draw/draw.h"

#include <csignal>

const float MAX_FPS = 60.f;
const float MIN_FRAME_DELTA = 1.f / MAX_FPS;

Log::Logger L(Log::Level::DEBUG);
DRAWMANAGER draw_manager;

volatile bool running;

void signal_handler(int signum) {
    running = false;
}

Exodus::Exodus() {
}

Exodus::~Exodus() {
}

bool Exodus::init() {
    L.info("Initialising Exodus...");
    signal(SIGINT, signal_handler);

    bool ok = true
        && draw_manager.init();

    return ok;
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

        draw_manager.update();

        timer.sleep_until(MIN_FRAME_DELTA);
        delta_time = timer.get_delta();
    }

    return 0;
}
