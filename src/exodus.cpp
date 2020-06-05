#include "exodus.h"

#include "shared.h"

#include "platform.h"
#include "timer/timer.h"
#include "audio/audio.h"
#include "draw/draw.h"
#include "input/input.h"

#include "state/exodus_state.h"
#include "state/ephemeral_state.h"

#include "mode/mode_base.h"
#include "mode/intro.h"
#include "mode/menu.h"
#include "mode/galaxy_gen.h"
#include "mode/galaxy_map.h"
#include "mode/galaxy_intro.h"
#include "mode/guild_exterior.h"
#include "mode/guild_hq.h"
#include "mode/guild_bar.h"
#include "mode/fly.h"
#include "mode/star_map.h"
#include "mode/planet_map.h"
#include "mode/planet_status.h"
#include "mode/planet_colonise.h"
#include "mode/lunar_battle.h"

#include <csignal>

const float MAX_FPS = 60.f;
const float MIN_FRAME_DELTA = 1.f / MAX_FPS;

Log::Logger L(Log::Level::DEBUG);
AUDIOMANAGER audio_manager;
DRAWMANAGER draw_manager;
INPUTMANAGER input_manager;

ExodusState exostate;
EphemeralState ephstate;

volatile bool running;

void signal_handler(int signum) {
    running = false;
}

Exodus::Exodus() {
    mode = nullptr;
    mode_stack_head = 0;
    current_mode = MODE_None;
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

    float frame_start;
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
    Fly mode_fly;
    StarMap mode_starmap;
    PlanetMap mode_planetmap;
    PlanetStatus mode_planetstatus;
    PlanetColonise mode_planetcolonise;
    LunarBattle mode_lunarbattle;
    mode_map[MODE_Intro] = (ModeBase*) &mode_intro;
    mode_map[MODE_Menu] = (ModeBase*) &mode_menu;
    mode_map[MODE_GalaxyGen] = (ModeBase*) &mode_galaxygen;
    mode_map[MODE_GalaxyMap] = (ModeBase*) &mode_galaxymap;
    mode_map[MODE_GalaxyIntro] = (ModeBase*) &mode_galaxyintro;
    mode_map[MODE_GuildExterior] = (ModeBase*) &mode_guildexterior;
    mode_map[MODE_GuildHQ] = (ModeBase*) &mode_guildhq;
    mode_map[MODE_GuildBar] = (ModeBase*) &mode_guildbar;
    mode_map[MODE_Fly] = (ModeBase*) &mode_fly;
    mode_map[MODE_StarMap] = (ModeBase*) &mode_starmap;
    mode_map[MODE_PlanetMap] = (ModeBase*) &mode_planetmap;
    mode_map[MODE_PlanetStatus] = (ModeBase*) &mode_planetstatus;
    mode_map[MODE_PlanetColonise] = (ModeBase*) &mode_planetcolonise;
    mode_map[MODE_LunarBattle] = (ModeBase*) &mode_lunarbattle;

    push_mode(MODE_Intro);

    running = true;

    MousePos mouse_pos = {-1, -1};
    MousePos click_pos = {-1, -1};

    while (running) {
        frame_start = game_timer.get_delta();

        ExodusMode next = mode->update(delta_time);
        if (next != ExodusMode::MODE_None) {
            if (next == ExodusMode::MODE_Pop) {
                pop_mode();
            } else if (next == ExodusMode::MODE_Reload) {
                // No stack change - just reset the current mode
                set_mode(current_mode);
            } else {
                if (next == ExodusMode::MODE_GalaxyMap) {
                    reset_mode_stack();
                }
                push_mode(next);
            }
        }

        draw_manager.update(delta_time, mouse_pos, click_pos);

        if (!input_manager.update()) {
            running = false;
        }

        if (input_manager.consume(Input::K_Escape)) {
            running = false;
        }

        mouse_pos = input_manager.get_mouse_pos();
        click_pos = input_manager.read_click();

        game_timer.sleep_until(frame_start + MIN_FRAME_DELTA);
        delta_time = game_timer.get_delta() - frame_start;

#ifdef DBG
        if (input_manager.read(Input::K_LShift)) {
            delta_time *= 10;
        }

        if (input_manager.read(Input::K_RShift)) {
            delta_time *= 10;
        }
#endif
    }

    cleanup();

    return 0;
}

void Exodus::set_mode(ExodusMode new_mode) {
    const char* mode_name = mode ? mode->name : "<NONE>";
    if (mode) mode->exit();
    mode = mode_map[new_mode];
    const char* new_mode_name = mode->name;
    L.debug("MODE: %s -> %s", mode_name, new_mode_name);
    current_mode = new_mode;
    mode->enter();
    // When we see a mode transition, allow the new one an update
    // before the draw manager kicks in.
    mode->update(0);
}

void Exodus::push_mode(ExodusMode new_mode) {
    if (mode_map[new_mode]->should_push_to_stack()) {
        mode_stack[mode_stack_head++] = new_mode;
        if (mode_stack_head >= MODE_STACK_SIZE)
            L.fatal("Mode stack full!");
    }
    set_mode(new_mode);
}

void Exodus::pop_mode() {
    if (mode_stack_head <= 1)
        L.fatal("Attempt to pop from empty mode stack");
    set_mode(mode_stack[--mode_stack_head - 1]);
}

void Exodus::reset_mode_stack() {
    L.debug("Resetting mode stack");
    mode_stack_head = 0;
}

ExodusMode Exodus::get_prev_mode() {
    if (mode_stack_head > 0)
        return mode_stack[mode_stack_head - 1];
    return ExodusMode::MODE_None;
}
