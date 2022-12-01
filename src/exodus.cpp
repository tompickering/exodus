#include "exodus.h"

#include "shared.h"

#include "platform.h"
#include "timer/timer.h"
#include "audio/audio.h"
#include "draw/draw.h"
#include "input/input.h"
#include "save/save.h"

#include "state/exodus_state.h"
#include "state/ephemeral_state.h"

#include "mode/mode_base.h"
#include "mode/intro.h"
#include "mode/menu.h"
#include "mode/galaxy_gen.h"
#include "mode/galaxy_map.h"
#include "mode/galaxy_intro.h"
#include "mode/distances.h"
#include "mode/guild_exterior.h"
#include "mode/guild_hq.h"
#include "mode/guild_bar.h"
#include "mode/fly.h"
#include "mode/star_map.h"
#include "mode/planet_map.h"
#include "mode/planet_status.h"
#include "mode/planet_colonise.h"
#include "mode/planet_transfer.h"
#include "mode/lunar_battle_prep.h"
#include "mode/lunar_battle.h"
#include "mode/trade.h"
#include "mode/fleet_production.h"
#include "mode/ship_equip.h"
#include "mode/news.h"
#include "mode/arrive.h"
#include "mode/alien_vessel.h"
#include "mode/space_battle.h"
#include "mode/game_over.h"
#include "mode/ending.h"

#include <cstdlib>
#include <csignal>

const float MAX_FPS = 60.f;
const float MIN_FRAME_DELTA = 1.f / MAX_FPS;

Log::Logger L(Log::Level::DEBUG);
AUDIOMANAGER audio_manager;
DRAWMANAGER draw_manager;
INPUTMANAGER input_manager;
SAVEMANAGER save_manager;

ExodusState exostate;
EphemeralState ephstate;

#ifdef DBG
ExodusDebug exodebug;
#endif

volatile bool running;

void signal_handler(int signum) {
    running = false;
}

Exodus::Exodus() {
    mode = nullptr;
    mode_stack_head = 0;
    current_mode = MODE_None;
    mode_updated_since_enter = false;
}

Exodus::~Exodus() {
}

bool Exodus::init() {
    L.info("Initialising Exodus...");
    signal(SIGINT, signal_handler);

    bool ok = draw_manager.init();

    if (ok) {
        register_buttons();
    }

    if (!audio_manager.init()) {
        L.error("Audio subsystem failed to initialise - game will not feature sound");
    }

#ifdef DBG
    exodebug.init();
#endif

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
    Distances mode_distances;
    GuildExterior mode_guildexterior;
    GuildHQ mode_guildhq;
    GuildBar mode_guildbar;
    Fly mode_fly;
    StarMap mode_starmap;
    PlanetMap mode_planetmap;
    PlanetStatus mode_planetstatus;
    PlanetColonise mode_planetcolonise;
    PlanetTransfer mode_planettransfer;
    LunarBattlePrep mode_lunarbattleprep;
    LunarBattle mode_lunarbattle;
    Trade mode_trade;
    FleetProduction mode_fleetproduction;
    ShipEquip mode_shipequip;
    News mode_news;
    Arrive mode_arrive;
    AlienVessel mode_alienvessel;
    SpaceBattle mode_spacebattle;
    GameOver mode_gameover;
    Ending mode_ending;
    mode_map[MODE_Intro] = (ModeBase*) &mode_intro;
    mode_map[MODE_Menu] = (ModeBase*) &mode_menu;
    mode_map[MODE_GalaxyGen] = (ModeBase*) &mode_galaxygen;
    mode_map[MODE_GalaxyMap] = (ModeBase*) &mode_galaxymap;
    mode_map[MODE_GalaxyIntro] = (ModeBase*) &mode_galaxyintro;
    mode_map[MODE_Distances] = (ModeBase*) &mode_distances;
    mode_map[MODE_GuildExterior] = (ModeBase*) &mode_guildexterior;
    mode_map[MODE_GuildHQ] = (ModeBase*) &mode_guildhq;
    mode_map[MODE_GuildBar] = (ModeBase*) &mode_guildbar;
    mode_map[MODE_Fly] = (ModeBase*) &mode_fly;
    mode_map[MODE_StarMap] = (ModeBase*) &mode_starmap;
    mode_map[MODE_PlanetMap] = (ModeBase*) &mode_planetmap;
    mode_map[MODE_PlanetStatus] = (ModeBase*) &mode_planetstatus;
    mode_map[MODE_PlanetColonise] = (ModeBase*) &mode_planetcolonise;
    mode_map[MODE_PlanetTransfer] = (ModeBase*) &mode_planettransfer;
    mode_map[MODE_LunarBattlePrep] = (ModeBase*) &mode_lunarbattleprep;
    mode_map[MODE_LunarBattle] = (ModeBase*) &mode_lunarbattle;
    mode_map[MODE_Trade] = (ModeBase*) &mode_trade;
    mode_map[MODE_FleetProduction] = (ModeBase*) &mode_fleetproduction;
    mode_map[MODE_ShipEquip] = (ModeBase*) &mode_shipequip;
    mode_map[MODE_News] = (ModeBase*) &mode_news;
    mode_map[MODE_Arrive] = (ModeBase*) &mode_arrive;
    mode_map[MODE_AlienVessel] = (ModeBase*) &mode_alienvessel;
    mode_map[MODE_SpaceBattle] = (ModeBase*) &mode_spacebattle;
    mode_map[MODE_GameOver] = (ModeBase*) &mode_gameover;
    mode_map[MODE_Ending] = (ModeBase*) &mode_ending;

    push_mode(MODE_Intro);

    running = true;

    MousePos mouse_pos = {-1, -1};
    MousePos click_pos = {-1, -1};
    MousePos click_pos_r = {-1, -1};

#ifdef DBG
    for (int i = 0; i < argc; ++i) {
        if (!strcmp(argv[i], "--setup")) {
            GameConfig config;
            config.n_players = 1;
            config.size = GAL_Medium;
            config.players[0].set_name("Debug");
            config.players[0].set_gender(GENDER_Female);
            config.players[0].set_title("Lady");
            config.players[0].set_ref("milady");
            config.players[0].set_flag_idx(0);
            config.aim = AIM_Might;
            config.enemy_start = ENEMY_Weak;
            exostate.init(config);
            exostate.get_player(0)->set_intro_seen();
            srand(0);
            exostate.generate_galaxy();
            exostate.finalise_galaxy();
            reset_mode_stack();
            push_mode(MODE_GalaxyMap);
        }
    }
#endif

    while (running) {
        frame_start = game_timer.get_delta();

        // Main game update, handled by the current mode
        ExodusMode next = mode->update(mode_updated_since_enter ? delta_time : 0);
        mode_updated_since_enter = true;

        while (next == ExodusMode::MODE_Redo) {
            draw_manager.consume_click();
            next = mode->update(0);
        }

        // Handle any mode transitions returned following update
        if (next != ExodusMode::MODE_None) {
            if (next == ExodusMode::MODE_Pop) {
                pop_mode();
            } else if (next == ExodusMode::MODE_Reload) {
                // No stack change - just reset the current mode
                set_mode(current_mode);
            } else if (next == ExodusMode::MODE_Quit) {
                running = false;
                continue;
            } else if (next == ExodusMode::MODE_NewGame) {
                reset_mode_stack();
                push_mode(MODE_Menu);
            } else {
                if (next == ExodusMode::MODE_GalaxyMap) {
                    reset_mode_stack();
                }
                push_mode(next);
            }
        }

        /*
         * It's possible we have entered a new mode following a return code from
         * mode->update(). In this case, we want to avoid drawing until the new
         * mode has had a chance to update in preparation for render (or even
         * to signal a further mode transition, in which case this will apply
         * again).
         */
        if (mode_updated_since_enter) {
            draw_manager.update(delta_time, mouse_pos, click_pos, click_pos_r);
            game_timer.sleep_until(frame_start + MIN_FRAME_DELTA);
        }

        if (!input_manager.update(delta_time)) {
            running = false;
        }

        /*
        if (input_manager.consume(Input::K_Escape)) {
            running = false;
        }
        */

        mouse_pos = input_manager.get_mouse_pos();
        click_pos = input_manager.read_click();
        click_pos_r = input_manager.read_click_r();

        delta_time = game_timer.get_delta() - frame_start;

#ifdef DBG
        if (input_manager.read(Input::K_LShift)) {
            delta_time *= 10;
        }

        if (input_manager.read(Input::K_RShift)) {
            delta_time *= 10;
        }

        if (input_manager.read(Input::K_C)) {
            click_pos = {1, 1};
        }

        if (input_manager.consume(Input::K_F1)) {
            exodebug.add_mc(1000);
        }

        if (input_manager.consume(Input::K_F2)) {
            exodebug.show_player_markers = !exodebug.show_player_markers;
            L.debug("--- PLAYER LOCATIONS ---");
            for (int i = 0; i < N_PLAYERS; ++i) {
                Player *p = exostate.get_player(i);
                if (p->is_participating()) {
                    PlayerLocation &loc = p->get_location();
                    FlyTarget *ft = exostate.loc2tgt(loc.get_target());
                    L.debug("%s%s%s", p->get_name(), loc.in_flight()?" F ":"   ", ft->name);
                }
            }
        }

        if (input_manager.consume(Input::K_F3)) {
            exodebug.add_units(10);
        }

        if (input_manager.consume(Input::K_F4)) {
            exodebug.full_visibility = !exodebug.full_visibility;
        }

        if (input_manager.consume(Input::K_F5)) {
            exodebug.unlock_all_inventions();
        }

        if (input_manager.consume(Input::K_F6)) {
            exodebug.set_all_alliances();
        }

        if (input_manager.consume(Input::K_F7)) {
            exodebug.add_goods(10);
        }

        if (input_manager.consume(Input::K_F8)) {
            save_manager.save(8);
        }

        if (input_manager.consume(Input::K_F9)) {
            save_manager.load(8);
            reset_mode_stack();
            push_mode(MODE_GalaxyMap);
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
    // Ensure that input state is reset before entering a new mode
    draw_manager.consume_click();
    // Repeating clicks disabled by default
    input_manager.enable_repeating_clicks(false);
    draw_manager.clear_sprite_ids();
    draw_manager.cancel_transitions();
    mode->enter();
    mode_updated_since_enter = false;
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
    if (mode->should_push_to_stack()) {
        if (mode_stack_head <= 1)
            L.fatal("Attempt to pop from empty mode stack");
        set_mode(mode_stack[--mode_stack_head - 1]);
    } else {
        if (mode_stack_head < 1)
            L.fatal("Attempt to pop from empty mode stack");
        // The current mode isn't actually on the stack - just restore top
        set_mode(mode_stack[mode_stack_head - 1]);
    }
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

void Exodus::register_buttons() {
    draw_manager.register_button(IMG_BR10_EXPORT, {0, 0, 108, 26});
    draw_manager.register_button(IMG_BR10_EXPORT, {109, 0, 108, 26});
    draw_manager.register_button(IMG_BR10_EXPORT, {218, 0, 108, 26});
    draw_manager.register_button(IMG_BR10_EXPORT, {325, 0, 108, 26});

    draw_manager.register_button(IMG_BR11_DEF);
    draw_manager.register_button(IMG_BR11_LR, {0, 0, 32, 26});
    draw_manager.register_button(IMG_BR11_LR, {33, 0, 32, 26});
    draw_manager.register_button(IMG_BR11_OK);

    draw_manager.register_button(IMG_BR12_EXPORT, {0, 0, 108, 26});
    draw_manager.register_button(IMG_BR12_EXPORT, {109, 0, 108, 26});
    draw_manager.register_button(IMG_BR12_EXPORT, {217, 0, 108, 26});

    draw_manager.register_button(IMG_BR13_FEMALE);
    draw_manager.register_button(IMG_BR13_MALE);
    draw_manager.register_button(IMG_BUTTON_OTHER);

    draw_manager.register_button(IMG_BR14_EXPORT, {0, 0, 118, 26});
    draw_manager.register_button(IMG_BR14_EXPORT, {119, 0, 118, 26});

    // TODO: IMG_BR16_NBAR

    draw_manager.register_button(IMG_BR1_EXPORT, {0, 0, 123, 26});
    draw_manager.register_button(IMG_BR1_EXPORT, {124, 0, 113, 26});

    draw_manager.register_button(IMG_BR2_EXPORT, {0, 0, 119, 26});
    draw_manager.register_button(IMG_BR2_EXPORT, {120, 0, 119, 26});
    draw_manager.register_button(IMG_BR2_EXPORT, {240, 0, 119, 26});
    draw_manager.register_button(IMG_BR2_EXPORT, {360, 0, 120, 26});

    draw_manager.register_button(IMG_BR3_EXPORT2, {0, 0, 59, 26});
    draw_manager.register_button(IMG_BR3_EXPORT2, {60, 0, 59, 26});
    draw_manager.register_button(IMG_BR3_EXPORT2, {0, 26, 118, 26});

    /*
     * FIXME: These are left/right arrows that are normally used
     * with repeating clicks enabled. However, button press VFX
     * and repeating clicks don't currently work well together,
     * because the VFX rate limits the clicks. We'd need some way
     * to allow query_click() to return true whilst still keeping
     * the click active - but this is messy, and may leave dangling
     * button press state...
     *
     * Just omit these for now.
     *
     * N.B. IMG_BR11_LR can be used for 'push'-able LR arrows.
     */
    /*
    draw_manager.register_button(IMG_BR4_EXPORT3, {0, 0, 32, 26});
    draw_manager.register_button(IMG_BR4_EXPORT3, {33, 0, 32, 26});
    */

    draw_manager.register_button(IMG_BR5_EXPORT4);

    draw_manager.register_button(IMG_BR6_OK);

    draw_manager.register_button(IMG_BR7_E1);
    draw_manager.register_button(IMG_BR7_E2);

    draw_manager.register_button(IMG_BR8_EXPORT, {0, 0, 119, 26});
    draw_manager.register_button(IMG_BR8_EXPORT, {120, 0, 119, 26});
    draw_manager.register_button(IMG_BR8_EXPORT, {240, 0, 119, 26});

    draw_manager.register_button(IMG_BR9_EXPORT, {0, 0, 108, 26});
    draw_manager.register_button(IMG_BR9_EXPORT, {109, 0, 108, 26});
    draw_manager.register_button(IMG_BR9_EXPORT, {218, 0, 108, 26});
    draw_manager.register_button(IMG_BR9_EXPORT, {327, 0, 108, 26});

}
