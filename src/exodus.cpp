#include "exodus.h"

#include "shared.h"

#include "platform.h"
#include "timer/timer.h"
#include "audio/audio.h"
#include "draw/draw.h"
#include "input/input.h"

#include "state/exodus_state.h"

#include "mode/mode_base.h"
#include "mode/intro.h"
#include "mode/menu.h"
#include "mode/galaxy_gen.h"
#include "mode/galaxy_map.h"
#include "mode/galaxy_intro.h"
#include "mode/guild_exterior.h"
#include "mode/guild_hq.h"
#include "mode/guild_bar.h"

#include <csignal>

const float MAX_FPS = 60.f;
const float MIN_FRAME_DELTA = 1.f / MAX_FPS;

Log::Logger L(Log::Level::DEBUG);
AUDIOMANAGER audio_manager;
DRAWMANAGER draw_manager;
INPUTMANAGER input_manager;

ExodusState exostate;

volatile bool running;

void signal_handler(int signum) {
    running = false;
}

Exodus::Exodus() {
    mode = nullptr;
    current_mode = ExodusMode::MODE_None;
    prev_mode    = ExodusMode::MODE_None;
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

    TIMER game_timer;
    TIMER frame_timer;
    float delta_time = 0.f;

    game_timer.start();

#ifdef SDL
    // FIXME: SDL seems to require some time before
    //        renders will work reliably...
    game_timer.sleep_until(0.05);
#endif

    draw_manager.draw_init_image();
    draw_manager.load_resources();
    audio_manager.load_resources();

    Intro mode_intro;
    Menu  mode_menu;
    GalaxyGen  mode_galaxygen;
    GalaxyMap  mode_galaxymap;
    GalaxyIntro  mode_galaxyintro;
    GuildExterior mode_guildexterior;
    GuildHQ mode_guildhq;
    GuildBar mode_guildbar;
    mode_map[MODE_Intro] = (ModeBase*) &mode_intro;
    mode_map[MODE_Menu] = (ModeBase*) &mode_menu;
    mode_map[MODE_GalaxyGen] = (ModeBase*) &mode_galaxygen;
    mode_map[MODE_GalaxyMap] = (ModeBase*) &mode_galaxymap;
    mode_map[MODE_GalaxyIntro] = (ModeBase*) &mode_galaxyintro;
    mode_map[MODE_GuildExterior] = (ModeBase*) &mode_guildexterior;
    mode_map[MODE_GuildHQ] = (ModeBase*) &mode_guildhq;
    mode_map[MODE_GuildBar] = (ModeBase*) &mode_guildbar;

    set_mode(MODE_Intro);

    running = true;

    MousePos mouse_pos = {-1, -1};
    MousePos click_pos = {-1, -1};

    while (running) {
        frame_timer.start();

        ExodusMode next = mode->update(delta_time);
        if (next != ExodusMode::MODE_None) {
            set_mode(next);
        }

        draw_manager.update(mouse_pos, click_pos);

        if (!input_manager.update()) {
            running = false;
        }

        if (input_manager.consume(Input::K_Escape)) {
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

void Exodus::set_mode(ExodusMode new_mode) {
    const char* mode_name = mode ? mode->name : "<NONE>";
    if (mode) mode->exit();
    prev_mode = current_mode;
    current_mode = new_mode;
    mode = mode_map[new_mode];
    const char* new_mode_name = mode->name;
    L.debug("MODE: %s -> %s", mode_name, new_mode_name);
    mode->enter();
}

ExodusMode Exodus::get_prev_mode() {
    return prev_mode;
}
