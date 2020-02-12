#include "exodus.h"

#include "shared.h"

#include "platform.h"
#include "timer/timer.h"
#include "audio/audio.h"
#include "draw/draw.h"

#include <csignal>

const float MAX_FPS = 60.f;
const float MIN_FRAME_DELTA = 1.f / MAX_FPS;

Log::Logger L(Log::Level::DEBUG);
AUDIOMANAGER audio_manager;
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
        && draw_manager.init()
        && audio_manager.init();

    return ok;
}

int Exodus::run(int argc, char** argv) {
    if (!init()) {
        return 1;
    }

    running = true;

    TIMER game_timer;
    TIMER frame_timer;
    float delta_time = 0.f;

    while (running) {
        frame_timer.start();

        draw_manager.update();

        frame_timer.sleep_until(MIN_FRAME_DELTA);
        delta_time = frame_timer.get_delta();
    }

    return 0;
}
