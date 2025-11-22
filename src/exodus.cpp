#include "exodus.h"

const ExodusVersion EXODUS_VERSION = 2;

#include "shared.h"

#include "platform/platform.h"
#include "timer/timer.h"
#include "audio/audio.h"
#include "draw/draw.h"
#include "input/input.h"
#include "save/save.h"
#include "achievements/achievements.h"

#include "state/exodus_state.h"
#include "state/ephemeral_state.h"

#include "mode/mode_base.h"
#include "mode/gamemode_select.h"
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
#include "mode/finance.h"
#include "mode/arrive.h"
#include "mode/alien_vessel.h"
#include "mode/space_battle.h"
#include "mode/game_over.h"
#include "mode/ending.h"

#include <cstdlib>
#include <csignal>

const float MAX_FPS = 60.f;
const float MIN_FRAME_DELTA = 1.f / MAX_FPS;

#ifdef DBG
Log::Logger L(Log::Level::DEBUG);
#else
Log::Logger L(Log::Level::WARN);
#endif

PLATFORM platform;
AUDIOMANAGER audio_manager;
DRAWMANAGER draw_manager;
INPUTMANAGER input_manager;
SAVEMANAGER save_manager;
ACHIEVEMENTMANAGER achievement_manager;

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

bool Exodus::has_option(const char* opt) {
    for (int i = 0; i < _argc; ++i) {
        if (!strncmp(_argv[i], "--", 2)) {
            if (!strcmp(_argv[i] + 2, opt)) {
                return true;
            }
        }
    }

    return false;
}

bool Exodus::init() {
    L.info("Initialising Exodus...");
    signal(SIGINT, signal_handler);

    L.info("Initialising platform...");
    if (platform.init()) {
        L.info("Platform initialised");
    } else {
        L.fatal("Failed to initialise platform");
    }

    DrawManagerOptions draw_manager_options;
    draw_manager_options.fullscreen = !has_option("windowed");
    bool ok = draw_manager.init(draw_manager_options);

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
    _argc = argc;
    _argv = argv;

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

    GameModeSelect mode_gamemodeselect;
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
    Finance mode_finance;
    Arrive mode_arrive;
    AlienVessel mode_alienvessel;
    SpaceBattle mode_spacebattle;
    GameOver mode_gameover;
    Ending mode_ending;
    mode_map[MODE_GameModeSelect] = (ModeBase*) &mode_gamemodeselect;
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
    mode_map[MODE_Finance] = (ModeBase*) &mode_finance;
    mode_map[MODE_Arrive] = (ModeBase*) &mode_arrive;
    mode_map[MODE_AlienVessel] = (ModeBase*) &mode_alienvessel;
    mode_map[MODE_SpaceBattle] = (ModeBase*) &mode_spacebattle;
    mode_map[MODE_GameOver] = (ModeBase*) &mode_gameover;
    mode_map[MODE_Ending] = (ModeBase*) &mode_ending;

    exodus_set_game_mode(EXODUSGAMEMODE_Enhanced);
    push_mode(MODE_Intro);

    running = true;

    MousePos mouse_pos;
    MousePos click_pos;
    MousePos click_pos_r;

#ifdef DBG
    if (has_option("setup")) {
        exodus_set_game_mode(EXODUSGAMEMODE_Enhanced);
        GameConfig config;
        config.n_players = 1;
        config.size = GAL_Medium;
        config.players[0].set_name("Debug");
        config.players[0].set_gender(GENDER_Female);
        config.players[0].set_title("Lady");
        config.players[0].set_ref("milady");
        config.players[0].set_flag_idx(0);
        config.aim = AIM_Might;
        config.enemy_start = ENEMY_Strong;
        exostate().init(config);
        exostate().get_player(0)->set_intro_seen();
        exostate().get_player(0)->set_intro_comm_seen();
        srand(0);
        exostate().generate_galaxy();
        exostate().finalise_galaxy();
        reset_mode_stack();
        push_mode(MODE_GalaxyMap);
    }
#endif

    exodus_set_game_mode(EXODUSGAMEMODE_Enhanced);

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
                reset_all_modes();
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
            draw_manager.update(delta_time, mouse_pos, click_pos, click_pos_r, input_manager.click_held_for(0.15f));
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

        platform.poll();

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
            click_pos.reset();
        }

        if (input_manager.consume(Input::K_F1)) {
            exodebug.add_mc(1000);
        }

        if (input_manager.consume(Input::K_F2)) {
            exodebug.show_player_markers = !exodebug.show_player_markers;
            L.debug("--- PLAYER LOCATIONS ---");
            for (int i = 0; i < N_PLAYERS; ++i) {
                Player *p = exostate().get_player(i);
                if (p->is_participating()) {
                    PlayerLocation &loc = p->get_location();
                    FlyTarget *ft = exostate().loc2tgt(loc.get_target());
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

        if (!input_manager.active_text_input()) {
            if (input_manager.consume(Input::K_P)) {
                exodebug.own_planet();
            }

            if (input_manager.consume(Input::K_O)) {
                exodebug.own_sys_planets();
            }

            if (input_manager.consume(Input::K_Z)) {
                exodebug.print_mc_data();
            }

            if (input_manager.consume(Input::K_X)) {
                exodebug.print_mc_history();
            }
        }
#endif
    }

    cleanup();

    L.info("Shutting down platform...");
    if (platform.shutdown()) {
        L.info("Platform shut down");
    } else {
        L.fatal("Failed to shut down platform");
    }

    exostate_destroy();

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

void Exodus::reset_all_modes() {
    L.debug("Resetting modes");

    new (mode_map[MODE_Intro]) Intro();
    new (mode_map[MODE_Menu]) Menu();
    new (mode_map[MODE_GalaxyGen]) GalaxyGen();
    new (mode_map[MODE_GalaxyMap]) GalaxyMap();
    new (mode_map[MODE_GalaxyIntro]) GalaxyIntro();
    new (mode_map[MODE_Distances]) Distances();
    new (mode_map[MODE_GuildExterior]) GuildExterior();
    new (mode_map[MODE_GuildHQ]) GuildHQ();
    new (mode_map[MODE_GuildBar]) GuildBar();
    new (mode_map[MODE_Fly]) Fly();
    new (mode_map[MODE_StarMap]) StarMap();
    new (mode_map[MODE_PlanetMap]) PlanetMap();
    new (mode_map[MODE_PlanetStatus]) PlanetStatus();
    new (mode_map[MODE_PlanetColonise]) PlanetColonise();
    new (mode_map[MODE_PlanetTransfer]) PlanetTransfer();
    new (mode_map[MODE_LunarBattlePrep]) LunarBattlePrep();
    new (mode_map[MODE_LunarBattle]) LunarBattle();
    new (mode_map[MODE_Trade]) Trade();
    new (mode_map[MODE_FleetProduction]) FleetProduction();
    new (mode_map[MODE_ShipEquip]) ShipEquip();
    new (mode_map[MODE_News]) News();
    new (mode_map[MODE_Finance]) Finance();
    new (mode_map[MODE_Arrive]) Arrive();
    new (mode_map[MODE_AlienVessel]) AlienVessel();
    new (mode_map[MODE_SpaceBattle]) SpaceBattle();
    new (mode_map[MODE_GameOver]) GameOver();
    new (mode_map[MODE_Ending]) Ending();
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
    draw_manager.register_button(IMG_BR10_EXPORT, {326, 0, 108, 26});

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

    draw_manager.register_button(IMG_BR14_EXPORT, {0, 0, 122, 26});
    draw_manager.register_button(IMG_BR14_EXPORT, {123, 0, 114, 26});

    draw_manager.register_button(IMG_BR16_NBAR, {96, 0, 108, 26});
    draw_manager.register_button(IMG_BR16_NBAR, {206, 0, 108, 26});
    draw_manager.register_button(IMG_BR16_NBAR, {314, 0, 108, 26});
    draw_manager.register_button(IMG_BR16_NBAR, {422, 0, 108, 26});

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
    draw_manager.register_button(IMG_BR9_EXPORT, {108, 0, 108, 26});
    draw_manager.register_button(IMG_BR9_EXPORT, {218, 0, 108, 26});
    draw_manager.register_button(IMG_BR9_EXPORT, {326, 0, 108, 26});

    draw_manager.register_button(IMG_GF4_HMENU1);
    draw_manager.register_button(IMG_GF4_HMENU2);
    draw_manager.register_button(IMG_GF4_HMENU3);
    draw_manager.register_button(IMG_GF4_HMENU4);
    draw_manager.register_button(IMG_GF4_HMENU5);

    draw_manager.register_button(IMG_GM1_EXIT);

    draw_manager.register_button(IMG_RD1_A1);
    draw_manager.register_button(IMG_RD1_F1);

    draw_manager.register_button(IMG_SU1_BUILD);
    draw_manager.register_button(IMG_SU1_EXPORT);
    draw_manager.register_button(IMG_SU1_PEOPLE);
    draw_manager.register_button(IMG_SU1_TRI, {0, 0, 39, 26});
    draw_manager.register_button(IMG_SU1_TRI, {40, 0, 39, 26});
    draw_manager.register_button(IMG_SU1_TRI, {80, 0, 39, 26});

    /*
    draw_manager.register_button(IMG_TD1_TRADEBACK, {208, 144, 58, 26});
    draw_manager.register_button(IMG_TD1_TRADEBACK, {268, 144, 58, 26});
    */
    draw_manager.register_button(IMG_TD1_TRADEBACK, {208, 172, 118, 26});

    draw_manager.register_button(IMG_TS1_M2_1);
    draw_manager.register_button(IMG_TS1_M2_2);
    draw_manager.register_button(IMG_TS1_SML_MENU, {0, 0, 112, 26});
    draw_manager.register_button(IMG_TS1_SML_MENU, {0, 26, 112, 26});
    draw_manager.register_button(IMG_TS1_SML_MENU, {0, 52, 112, 26});

    draw_manager.register_button(IMG_TS2_M2_1);
    draw_manager.register_button(IMG_TS2_M2_2);
    draw_manager.register_button(IMG_TS2_SML_MENU, {0, 0, 112, 26});
    draw_manager.register_button(IMG_TS2_SML_MENU, {0, 26, 112, 26});
    draw_manager.register_button(IMG_TS2_SML_MENU, {0, 52, 112, 26});

    draw_manager.register_button(IMG_VS0_VPIC, {96, 366, 108, 26});
    draw_manager.register_button(IMG_VS0_VPIC, {206, 366, 108, 26});
    draw_manager.register_button(IMG_VS0_VPIC, {314, 366, 108, 26});
    draw_manager.register_button(IMG_VS0_VPIC, {422, 366, 108, 26});

    draw_manager.register_button(IMG_PRBUTTONS, {0, 0, 33, 26});
    draw_manager.register_button(IMG_PRBUTTONS, {34, 0, 33, 26});
    draw_manager.register_button(IMG_PRBUTTONS, {67, 0, 33, 26});

    draw_manager.register_button(IMG_BTNREPORT, {0, 0, 38, 26});
    draw_manager.register_button(IMG_BTNGUIDE, {0, 0, 38, 26});
    draw_manager.register_button(IMG_BTNZOOM, {0, 0, 98, 26});
    draw_manager.register_button(IMG_BTN_TOP, {0, 0, 98, 26});

    draw_manager.register_button(IMG_BATTLE_INFO, {0, 0, 86, 26});
    draw_manager.register_button(IMG_BATTLE_MAN, {0, 0, 40, 26});
    draw_manager.register_button(IMG_BATTLE_QUIT, {0, 0, 86, 26});
    draw_manager.register_button(IMG_BATTLE_TALK, {0, 0, 86, 26});

    draw_manager.register_button(IMG_BTN_INFO, {0, 0, 118, 26});
    draw_manager.register_button(IMG_BTN_QUIT, {0, 0, 118, 26});
}
