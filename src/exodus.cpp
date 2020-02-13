#include "exodus.h"

#include "shared.h"

#include "platform.h"
#include "timer/timer.h"
#include "audio/audio.h"
#include "draw/draw.h"
#include "input/input.h"

#include <csignal>

const float MAX_FPS = 60.f;
const float MIN_FRAME_DELTA = 1.f / MAX_FPS;

Log::Logger L(Log::Level::DEBUG);
AUDIOMANAGER audio_manager;
DRAWMANAGER draw_manager;
INPUTMANAGER input_manager;

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

    bool ok = draw_manager.init();

    if (!audio_manager.init()) {
        L.error("Audio subsystem failed to initialise - game will not feature sound");
    }

    return ok;
}

int Exodus::run(int argc, char** argv) {
    if (!init()) {
        return 1;
    }

    draw_manager.load_resources();

    running = true;

    TIMER game_timer;
    TIMER frame_timer;
    float delta_time = 0.f;

    MousePos mouse_pos = {-1, -1};
    MousePos click_pos = {-1, -1};

    while (running) {
        frame_timer.start();

        draw_manager.update(mouse_pos, click_pos);

        if (!input_manager.update()) {
            running = false;
        }

        if (input_manager.read(Input::Escape, true)) {
            running = false;
        }

        mouse_pos = input_manager.get_mouse_pos();
        click_pos = input_manager.read_click();

        frame_timer.sleep_until(MIN_FRAME_DELTA);
        delta_time = frame_timer.get_delta();
    }

    cleanup();

    return 0;
}
