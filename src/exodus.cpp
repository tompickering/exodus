#include "exodus.h"

#include "shared.h"

#include "platform.h"
#include "timer/timer.h"
#include "audio/audio.h"
#include "draw/draw.h"
#include "input/input.h"

#include "state/state_base.h"
#include "state/intro.h"

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
    state = nullptr;
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

    Intro st_intro;
    state_map[ST_Intro] = (StateBase*) &st_intro;

    set_state(ST_Intro);

    running = true;

    TIMER game_timer;
    TIMER frame_timer;
    float delta_time = 0.f;

    MousePos mouse_pos = {-1, -1};
    MousePos click_pos = {-1, -1};

    while (running) {
        frame_timer.start();

        ExodusState next = state->update(delta_time);
        if (next != ExodusState::ST_None) {
            set_state(next);
        }

        draw_manager.update(mouse_pos, click_pos);

        if (!input_manager.update()) {
            running = false;
        }

        if (input_manager.consume(Input::Escape)) {
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

void Exodus::set_state(ExodusState new_state) {
    const char* state_name = state ? state->name : "<NONE>";
    if (state) state->exit();
    state = state_map[new_state];
    const char* new_state_name = state->name;
    L.debug("STATE: %s -> %s", state_name, new_state_name);
    state->enter();
}
