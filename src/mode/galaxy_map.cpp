#include "galaxy_map.h"

#include "assetpaths.h"

#include "exodus_features.h"

#include "util/str.h"
#include "util/value.h"

#define BLINK_TIME 0.5

#ifdef DBG
extern ExodusDebug exodebug;
#endif

enum ID {
    SELECTED,
    FLEET_MARKER,
    MONTH_PASSING_BG,
    MONTH_PASSING,
    FRAMED_IMG,
    SPACEPORT2,
    SPACEPORT_SHIP0,
    SPACEPORT_SHIP1,
    SPACEPORT_SHIP2,
    END,
};

GalaxyMap::GalaxyMap() : ModeBase("GalaxyMap"), GalaxyDrawer(), PanelDrawer(PNL_Galaxy), CommPanelDrawer(), BulletinDrawer(), FrameDrawer() {
    stage = GM_SwapIn;
    selected_ft = nullptr;
    selected_ft_blink = 0;
    first_spaceport_time = 0;

    mp_state.mp_stage = MP_None;
    mp_state.mpai_stage = (MonthPassAIStage)0;
    mp_state.mpp_stage = (MonthPassPlanetStage)0;
    mp_state.month_pass_time = 0;

    do_first_city = false;
    do_first_spaceport = false;
    do_meteor = false;
    do_meltdown = false;
    do_lunar_battle = false;
    do_guild_title = GUILDTITLE_None;

    active_mission = false;
    mission_n_strikes = 0;
}

void GalaxyMap::enter() {
    ModeBase::enter(ID::END);
    draw_galaxy(false);

    DrawTarget tgt = TGT_Primary;
    if (exodus.get_prev_mode() == ExodusMode::MODE_GalaxyGen) {
        tgt = TGT_Secondary;
    }

    draw_panel_bg(tgt);

    if (tgt != TGT_Primary) {
        draw_manager.pixelswap_start(&galaxy_panel_area);
    }


    if (ephstate.get_ephemeral_state() == EPH_ResumeFly) {
        ephstate.clear_ephemeral_state();
        stage = GM_Fly;
    } else if (ephstate.get_ephemeral_state() == EPH_SelectPlanet) {
        stage = GM_SelectStar;
    } else if (mp_state.mp_stage == MP_None) {
        stage = GM_SwapIn;
    } else {
        stage = GM_MonthPassing;
    }

    // PROCbackmusic
    if (exostate.get_orig_month() >= 2) {
        ephstate.default_music = onein(4) ? MUS_OVERVIEW : mpart2mus(17);
        if (audio_manager.get_playing() == mpart2mus(1)) {
            if (onein(3)) {
                ephstate.default_music = mpart2mus(17);
            }
        } else if (audio_manager.get_playing() == mpart2mus(2)) {
                ephstate.default_music = mpart2mus(4);
        } else if (audio_manager.get_playing() == mpart2mus(6)) {
                ephstate.default_music = mpart2mus(13);
        } else if (audio_manager.get_playing() == mpart2mus(8)) {
                ephstate.default_music = mpart2mus(2);
        } else if (audio_manager.get_playing() == mpart2mus(9)) {
                ephstate.default_music = mpart2mus(17);
        } else if (audio_manager.get_playing() == mpart2mus(10)) {
                ephstate.default_music = mpart2mus(13);
        } else if (audio_manager.get_playing() == mpart2mus(17)) {
            if (onein(5)) {
                ephstate.default_music = mpart2mus(3);
            }
        }
    }

    audio_manager.target_music(ephstate.default_music);

    // Leave selected_ft as-is

    draw_manager.show_cursor(true);
}

const float FADE_SPEED = 10.f;

ExodusMode GalaxyMap::update(float delta) {
    int draw_x, draw_y;
    FlyTarget *ft;
    SpriteClick click;
    CommAction action;

    if (draw_manager.pixelswap_active() || draw_manager.fade_active()) {
        return ExodusMode::MODE_None;
    }

    Galaxy *gal = exostate.get_galaxy();
    Player *player = exostate.get_active_player();

    selected_ft_blink += delta;
    while (selected_ft_blink > 2*BLINK_TIME)
        selected_ft_blink -= 2*BLINK_TIME;

    switch (stage) {
        case GM_SwapIn:
            draw_manager.save_background();
            stage = GM_Idle;
            // Ensure we prepare the next frame straight away
            // Otherwise causes an annoying flicker e.g. at the end of Fly...
            return update(0);
        case GM_Idle:
            //return ExodusMode::MODE_Ending;
            if (player->get_race() == RACE_Human && !player->intro_seen()) {
                exostate.set_active_flytarget(gal->get_guild());
                return ExodusMode::MODE_GalaxyIntro;
            }

            draw_manager.show_cursor(true);

            ft = get_clicked_flytarget();

            if (ft) {
                exostate.set_active_flytarget(ft);
            }

            if (!selected_ft) {
                ft = exostate.get_active_flytarget();
            }

            if (ft && ft != selected_ft) {
                draw_manager.draw(id(ID::SELECTED), nullptr);
                selected_ft = ft;
                selected_ft_blink = BLINK_TIME;
            }

            if (selected_ft && selected_ft_blink >= BLINK_TIME) {
                get_draw_position(selected_ft, draw_x, draw_y);
                draw_manager.draw(
                        id(ID::SELECTED),
                        IMG_TS1_MK1,
                        {draw_x, draw_y, 0.5, 0.5, 1, 1});
            } else {
                draw_manager.draw(id(ID::SELECTED), nullptr);
            }

            update_panel_info_player(TGT_Primary, player);
            update_panel_info_ft(TGT_Primary, player, selected_ft);

            {
                FlyTarget *fleet_pos = exostate.loc2tgt(player->get_location().get_target());
                const char* marker_icon = IMG_TS1_DEST;
                get_draw_position(fleet_pos, draw_x, draw_y);

                if (!player->get_location().in_flight()) {
                    switch (player->get_fleet_marker_idx()) {
                        case 0:
                            marker_icon = IMG_TS1_ICON1;
                            break;
                        case 1:
                            marker_icon = IMG_TS1_ICON2;
                            break;
                        case 2:
                            marker_icon = IMG_TS1_ICON3;
                            break;
                        case 3:
                            marker_icon = IMG_TS1_ICON4;
                            break;
                        case 4:
                            marker_icon = IMG_TS1_ICON5;
                            break;
                        default:
                            L.fatal("Invalid fleet marker index");
                            break;
                    }
                }

                draw_manager.draw(
                    id(ID::FLEET_MARKER),
                    marker_icon,
                    {draw_x - 10, draw_y + 10, 0.5, 0.5, 1, 1});

#ifdef DBG
                if (exodebug.show_player_markers) {
                    char text[3];
                    for (int i = 0; i < N_PLAYERS; ++i) {
                        Player *p = exostate.get_player(i);
                        FlyTarget *fleet_pos = exostate.loc2tgt(p->get_location().get_target());
                        get_draw_position(fleet_pos, draw_x, draw_y);
                        snprintf(text, sizeof(text), "%d", i);
                        RGB col = {0, 0xFF, 0};
                        if (p->get_location().in_flight()) {
                            col = {0xFF, 0, 0};
                        }
                        draw_manager.draw_text(
                            exodebug.player_markers[i],
                            Font::Large,
                            text,
                            Justify::Left,
                            draw_x - 10, draw_y - 10,
                            col);
                    };
                }
#endif
            }

            click = draw_manager.query_click(id_panel);
            if (click.id) {
                if (click.x < 0.25) {
                    // Fly
                    FlyTarget *tgt = exostate.loc2tgt(player->get_location().get_target());
                    if (selected_ft != tgt) {
                        if (!player->get_location().in_flight()) {
                            if (player->get_starship().pct_damage_thrust > 50) {
                                comm_open(DIA_S_ThrustBroken);
                                stage = GM_FlyConfirm;
                                return ExodusMode::MODE_None;
                            }

                            comm_ctx.location = exostate.tgt2loc(selected_ft);
                            // Orig stores pixel locations instead of low-res grid values
                            // See PROC_Csunpos for original's multiplaication by 43
                            int dx = (selected_ft->x - tgt->x) * 43;
                            int dy = (selected_ft->y - tgt->y) * 43;
                            int dist = sqrt((dx*dx) + (dy*dy));
                            int ftime = 1 + (dist/300);
                            if (ftime > 1 && player->has_invention(INV_OrbitalMassThrust)) {
                                ftime /= 3;
                            }
                            comm_ctx.months = max(1, ftime);
                            comm_open(DIA_S_PlanFly);
                            stage = GM_FlyConfirm;
                            return ExodusMode::MODE_None;
                        } else {
#if FEATURE_COUNSELLOR_EXTRA
                            comm_open(DIA_S_FlyAlreadyFlying);
                            stage = GM_Counsellor;
#endif
                        }
                    } else {
#if FEATURE_COUNSELLOR_EXTRA
                        comm_open(DIA_S_FlyAlreadyThere);
                        stage = GM_Counsellor;
#endif
                    }
                } else if (click.x < 0.5) {
                    stage = GM_Menu;
                    menu_open(MM_Ctrl);
                } else if (click.x < 0.75) {
                    stage = GM_Menu;
                    menu_open(MM_Stat);
                } else {
                    // Zoom
                    if (player->get_location().has_visited(exostate.tgt2loc(selected_ft))) {
                        bool ok = true;
                        FlyTarget *guild = exostate.get_galaxy()->get_guild();
                        if (selected_ft == guild) {
                            if (player->get_location().in_flight()) {
                                L.debug("Can't zoom to guild - in flight");
                                ok = false;
                            }
                            if (ok) {
                                int loc = player->get_location().get_target();
                                FlyTarget *tgt = exostate.loc2tgt(loc);
                                if (tgt != guild) {
                                    L.debug("Can't zoom to guild - elsewhere");
                                    ok = false;
                                }
                            }
                        }
                        if (ok) {
                            exostate.set_active_flytarget(selected_ft);
                            exostate.set_active_planet(-1);
                            draw_manager.show_cursor(false);
                            draw_manager.fade_black(1.2f, 24);
                            if (selected_ft == gal->get_guild()) {
                                stage = GM_Zoom2Guild;
                            } else {
                                stage = GM_Zoom2Star;
                            }
                        }
                    } else {
#if FEATURE_COUNSELLOR_EXTRA
                        comm_open(DIA_S_ZoomButNotVisited);
                        stage = GM_Counsellor;
#endif
                        L.debug("Can't zoom - not visited");
                    }
                }
            }

            // Hotkeys
            if (input_manager.consume(K_Space)) {
                stage = GM_MonthPassing;
            }

            if (input_manager.consume(K_D)) {
                return ExodusMode::MODE_Distances;
            }

            // Orig is 'R' for 'Recall' but 'N' seems more intuitive
            if (input_manager.consume(K_R) || input_manager.consume(K_N)) {
                return ExodusMode::MODE_News;
            }

            break;
        case GM_Zoom2Guild:
            return ExodusMode::MODE_GuildExterior;
        case GM_Zoom2Star:
            return ExodusMode::MODE_StarMap;
        case GM_FlyConfirm:
            action = comm_update(delta);
            if (action == CA_Proceed) {
                ephstate.fly_plan.loc = comm_ctx.location;
                ephstate.fly_plan.months = comm_ctx.months;
                // Decide if pirate attack happens
                if (exostate.get_orig_month() < 7) {
                    player->nopirates = 0;
                }
                FlyTarget *loc = exostate.loc2tgt(player->get_location().get_target());
                if (loc != gal->get_guild()) {
                    int r = 2;
                    if (loc->pirates == 0) {
                        r = 4;
                    } else if (loc->pirates == 1) {
                        r = 3;
                    }
                    if ((exostate.get_orig_month() > 5 && onein(r)) || (player->nopirates > 5)) {
                        // Alien encounter
                        L.info("ALIEN ENCOUNTER");
                        loc->pirates = 0;
                        return ExodusMode::MODE_AlienVessel;
                    } else {
                        player->nopirates++;
                    }
                }
                stage = GM_Fly;
                break;
            } else if (action == CA_Abort) {
                comm_close();
                stage = GM_Idle;
            }
            break;
        case GM_Fly:
            player->get_location().set_target(ephstate.fly_plan.loc, ephstate.fly_plan.months);
            return ExodusMode::MODE_Fly;
        case GM_Counsellor:
            action = comm_update(delta);
            if (action != CA_None) {
                comm_close();
                stage = GM_Idle;
            }
            break;
        case GM_SelectStar:
            {
                // TODO: Need a way to cancel this - especially if no options possible!
                // TODO: Recover mission MC if cancelled?
                // TODO: "SELECT A STAR" message
                // TODO: (Show star name on mouseover?)
                // TODO: Some way to highlight available stars
                ft = get_clicked_flytarget();
                if (ft && ft != exostate.get_galaxy()->get_guild()) {
                    int ft_idx = exostate.tgt2loc(ft);
                    if (player->get_location().has_visited(ft_idx)) {
                        *(ephstate.selectplanet_star) = ft_idx;
                        exostate.set_active_flytarget(ft);
                        return ExodusMode::MODE_StarMap;
                    }
                }
            }
            break;
        case GM_MonthPassing:
            {
                update_panel_info_player(TGT_Primary, exostate.get_player(0));
                update_panel_info_ft(TGT_Primary, exostate.get_player(0), selected_ft);

                if (exostate.final_month()) {
                    if (ephstate.get_ephemeral_state() != EPH_GameOver) {
                        ephstate.set_ephemeral_state(EPH_GameOver);
                        ephstate.game_over_reason = GAMEOVER_Failed;
                        draw_manager.fade_black(1.2f, 24);
                    } else if (!draw_manager.fade_active()) {
                        return ephstate.get_appropriate_mode();
                    }
                } else {
                    stage = GM_MonthPassMain;
                }
                return ExodusMode::MODE_None;
            }
            break;
        case GM_MonthPassMain:
            {
                update_panel_info_player(TGT_Primary, exostate.get_player(0));
                update_panel_info_ft(TGT_Primary, exostate.get_player(0), selected_ft);

                if (bulletin_is_open()) {
                    bulletin_update(delta);
                    if (!bulletin_acknowledged()) {
                        break;
                    } else if (do_first_city){
                        do_first_city = false;
                        bulletin_ensure_closed();
                        draw_manager.draw(
                            id(ID::FRAMED_IMG),
                            IMG_CT1_EXPORT,
                            {5, 7, 0, 0, 1, 1});
                        frame_draw();
                        stage = GM_MP_FirstCity;
                        return ExodusMode::MODE_None;
                    } else if (do_meteor) {
                        do_meteor = false;
                        bulletin_ensure_closed();
                        // TODO: PlanetMap should show number of strikes
                        return ephstate.get_appropriate_mode();
                    } else if (do_meltdown) {
                        do_meltdown = false;
                        bulletin_ensure_closed();
                        return ephstate.get_appropriate_mode();
                    } else if (do_lunar_battle) {
                        do_lunar_battle = false;
                        bulletin_ensure_closed();
                        return ephstate.get_appropriate_mode();
                    } else if (ephstate.get_ephemeral_state() == EPH_ResearchCheck) {
                        ephstate.research.done = bulletin_was_yesno_yes();
                        bulletin_ensure_closed();
                        return ephstate.get_appropriate_mode();
                    } else if (do_guild_title != GUILDTITLE_None) {
                        bulletin_ensure_closed();
                        bulletin_start_new(false);
                        bulletin_set_bg(IMG_ME6_MENU);
                        bulletin_set_flag(IMG_TS1_FLAG16);
                        bulletin_set_text_col(COL_TEXT3);
                        bulletin_set_next_text("MESSAGE FROM SPACE GUILD");
                        bulletin_set_next_text("");

                        switch (do_guild_title) {
                            case GUILDTITLE_Warrior:
                                bulletin_set_next_text("We are watching your advancements with");
                                bulletin_set_next_text("interest.");
                                break;
                            case GUILDTITLE_Strong:
                                bulletin_set_next_text("You are making remarkable advancements.");
                                bulletin_set_next_text("The Space Guild is impressed.");
                                bulletin_set_next_text("Maybe you are the right candidate for");
                                bulletin_set_next_text("the Guildmaster position.");
                                break;
                            case GUILDTITLE_Conqueror:
                                bulletin_set_next_text("You have grown strong. This is a sign");
                                bulletin_set_next_text("that you are a skilful sovereign.");
                                bulletin_set_next_text("If you continue your way like this,");
                                bulletin_set_next_text("we are looking forward to seeing you");
                                bulletin_set_next_text("at the Guildmaster position.");
                                break;
                            case GUILDTITLE_Mighty:
                                bulletin_set_next_text("We are still watching your steps with");
                                bulletin_set_next_text("interest. We know that you will");
                                bulletin_set_next_text("finally achieve what you are heading");
                                bulletin_set_next_text("for.");
                                break;
                            case GUILDTITLE_Great:
                                bulletin_set_next_text("Go on like this, and we will stand");
                                bulletin_set_next_text("by your side.");
                                break;
                            case GUILDTITLE_Warlord:
                                bulletin_set_next_text("We are waiting for the final steps");
                                bulletin_set_next_text("to be made.");
                                break;
                            case GUILDTITLE_Emperor:
                                bulletin_set_next_text("We are waiting for you.");
                                break;
                            default:
                                break;
                        }

                        if (player->committed_any_infractions()) {
                            bulletin_set_next_text("");
                            bulletin_set_next_text("But please note that you have violated");
                            bulletin_set_next_text("the Space Guild Rules.");
                        }

                        do_guild_title = GUILDTITLE_None;
                        return ExodusMode::MODE_None;
                    }
                }

                if (comm_is_open()) {
                    if (comm_update(delta) == CA_None) {
                        break;
                    }
                }

                if (do_first_spaceport) {
                    Planet *p = exostate.get_active_planet();
                    do_first_spaceport = false;
                    bulletin_ensure_closed();
                    DrawArea a = {0, 0, RES_X, 404};
                    draw_manager.set_source_region(id(ID::FRAMED_IMG), &a);
                    draw_manager.draw(
                        id(ID::FRAMED_IMG),
                        p->sprites()->spaceport,
                        {5, 7, 0, 0, 1, 1});
                    char t[PLANET_MAX_NAME + 32];
                    snprintf(t, sizeof(t), "%s has built a space harbour!", p->get_name());
                    draw_manager.draw_text(
                        t,
                        Justify::Left,
                        12, 378,
                        COL_TEXT2);
                    frame_draw();
                    first_spaceport_time = 0;
                    stage = GM_MP_FirstSpaceport;
                    return ExodusMode::MODE_None;
                }

                audio_manager.target_music(ephstate.default_music);

                ExodusMode next_mode = month_pass_update();
                mp_state.month_pass_time += delta;
                if (mp_state.mp_stage == MP_None) {
                    bulletin_ensure_closed();
                    // The only place we emerge from month-pass-specific stages...
                    stage = GM_Idle;

                    // This is mainly to redraw stars in case a sun expansion occurred
                    draw_galaxy(false);
                    draw_panel_bg(TGT_Primary);

                    // Ensure we prepare the next frame straight away
                    // Otherwise causes an annoying flicker on month pass end
                    update(0);
                }
                return next_mode;
            }
        case GM_MP_FirstCity:
            if (draw_manager.clicked()) {
                draw_manager.draw(
                    id(ID::FRAMED_IMG),
                    nullptr);
                frame_remove();
                stage = GM_MonthPassing;
            }
            break;
        case GM_MP_FirstSpaceport:
            if (!first_spaceport_update(delta)) {
                draw_manager.set_source_region(id(ID::FRAMED_IMG), nullptr);
                draw_manager.draw(
                    id(ID::FRAMED_IMG),
                    nullptr);
                frame_remove();
                stage = GM_MonthPassing;
            }
            break;
        case GM_Menu:
            if (menu_is_open()) {
                // This is a bit inelegant, but we can spend MC whilst the menu
                // is open - and as the panel is visible, it's confusing if this
                // isn't kept up to date.
                update_panel_info_player(TGT_Primary, player);
                ExodusMode new_mode = menu_update(delta);
                if (new_mode != ExodusMode::MODE_None) {
                    menu_close();
                    return new_mode;
                }
                switch (menu_get_action()) {
                    case MA_Close:
                        menu_close();
                        stage = GM_Idle;
                        break;
                    case MA_EquipShip:
                        menu_close();
                        return ExodusMode::MODE_ShipEquip;
                    case MA_BuildArtificialWorld:
                        menu_close();
                        selected_ft = nullptr;
                        draw_manager.draw(id(ID::SELECTED), nullptr);
                        selected_ft_blink = 0;
                        stage = GM_ArtificialWorldStarSelect;
                        break;
                    case MA_Quit:
                        menu_close();
                        comm_open(DIA_S_Quit);
                        stage = GM_QuitConfirm;
                        break;
                    default:
                        break;
                }
            } else {
                stage = GM_Idle;
            }
            break;
        case GM_ArtificialWorldStarSelect:
            {
                // Allow player to cancel
                if (input_manager.consume(K_Escape)) {
                    player->give_mc(COST_ART);
                    stage = GM_Idle;
                    break;
                }

                Galaxy *gal = exostate.get_galaxy();

                ft = get_clicked_flytarget();

                if (ft && (ft != gal->get_guild())) {
                    exostate.set_active_flytarget(ft);
                    if (ft == selected_ft) {
                        Star *s = (Star*) ft;
                        int player_idx = exostate.get_active_player_idx();

                        // FIXME: It's a bit hacky to rely on the input system to remember this...
                        const char* name = input_manager.get_input_text(PLANET_MAX_NAME);

                        if (s->construct_artificial_world(player_idx, name)) {
                            stage = GM_Idle;
                            break;
                        }
                    }
                }

                if (ft && ft != selected_ft) {
                    draw_manager.draw(id(ID::SELECTED), nullptr);
                    selected_ft = ft;
                    selected_ft_blink = BLINK_TIME;
                }

                if (selected_ft && selected_ft_blink >= BLINK_TIME) {
                    get_draw_position(selected_ft, draw_x, draw_y);
                    draw_manager.draw(
                            id(ID::SELECTED),
                            IMG_TS1_MK1,
                            {draw_x, draw_y, 0.5, 0.5, 1, 1});
                } else {
                    draw_manager.draw(id(ID::SELECTED), nullptr);
                }

                // TODO: Perhaps this should be indicated elsewhere, so the player
                //       can still use the usual readout to help decide where to
                //       select.
                panel_set_text("SELECT A STAR");
            }
            break;
        case GM_QuitConfirm:
            if (ephstate.get_ephemeral_state() != EPH_GameOver) {
                action = comm_update(delta);
                if (action == CA_Proceed) {
                    ephstate.set_ephemeral_state(EPH_GameOver);
                    ephstate.game_over_reason = GAMEOVER_Failed;
                    draw_manager.fade_black(1.2f, 24);
                } else if (action == CA_Abort) {
                    comm_close();
                    stage = GM_Idle;
                    break;
                }
            } else if (!draw_manager.fade_active()) {
                return ephstate.get_appropriate_mode();
            }

            break;
        default:
            break;
    }

    return ExodusMode::MODE_None;
}

void GalaxyMap::exit() {
    comm_ensure_closed();
    ModeBase::exit();
}

bool GalaxyMap::first_spaceport_update(float delta) {
    Planet *p = exostate.get_active_planet();
    PlanetClass cls = p->get_class();

    int x = 5;
    int y = 0;
    float ys = 1.f;

    if (cls == Forest)       y = 111;
    if (cls == Desert)       y = 151;
    if (cls == Volcano)      y = 125;
    if (cls == Rock)         y = 135;
    if (cls == Ice)          y = 133;
    if (cls == Terra)      { y = 111; ys = 2.f; }
    if (cls == Artificial)   y = 153;

    // Ship animations
    // TODO: Polish (smoother anims etc)

    float t = first_spaceport_time;

    if (cls == Forest) {
        // TODO: Placeholder - check what this looks like in orig
        int ship0_x = 800 - fmod(t+20,40)*20;
        draw_manager.draw(
            id(ID::SPACEPORT_SHIP0),
            IMG_HS3_S1,
            {ship0_x, 160,
             .5f, .5f, 2, 2});
        int ship1_x = -100 + fmod(t+20,44)*18;
        draw_manager.draw(
            id(ID::SPACEPORT_SHIP1),
            IMG_HS3_G1,
            {ship1_x, 130,
             .5f, .5f, 2, 2});
    }

    if (cls == Desert) {
        float tb;

        static Anim ship0_anim(23,
            IMG_HS1_F1,  IMG_HS1_F2,  IMG_HS1_F3,  IMG_HS1_F4,
            IMG_HS1_F5,  IMG_HS1_F5,  IMG_HS1_F5,  IMG_HS1_F5,
            IMG_HS1_F5,  IMG_HS1_F5,  IMG_HS1_F5,  IMG_HS1_F5,
            IMG_HS1_F5,  IMG_HS1_F5,  IMG_HS1_F5,  IMG_HS1_F5,
            IMG_HS1_F6,  IMG_HS1_F7,  IMG_HS1_F8,  IMG_HS1_F9,
            IMG_HS1_F10, IMG_HS1_F11, nullptr);

        tb = fmod(t+12,14);
        int ship0_x = 310;// - (tb<5.9 ? 0 : (tb-5.9)*30);
        int ship0_y = 260 - tb*14;
        float interp = tb<3.5 ? 0 : fmin(1,(tb-3.5)*.3f);
        draw_manager.draw(
            id(ID::SPACEPORT_SHIP0),
            ship0_anim.interp(interp),
            {ship0_x, ship0_y,
             .5f, .5f, 2, 2});

        static Anim ship1_anim(9,
            IMG_HS1_P8, IMG_HS1_P7, IMG_HS1_P6, IMG_HS1_P5,
            IMG_HS1_P4, IMG_HS1_P3, IMG_HS1_P2, IMG_HS1_P1,
            IMG_HS1_P0);

        tb = fmod(t,20);
        float ship1_x_interp = fclamp((tb-4)/16.f, 0, 1);
        float ship1_y_interp = fclamp(tb/4.f, 0, 1);
        int ship1_x = 276 - ship1_x_interp*400.f;
        int ship1_y = 270 - ship1_y_interp*76.f;
        interp = fclamp((tb-3)/3, 0, 1);
        draw_manager.draw(
            id(ID::SPACEPORT_SHIP1),
            ship1_anim.interp(interp),
            {ship1_x, ship1_y,
             .5f, .5f, 2, 2});

        static Anim ship2_anim(9,
            IMG_HS1_S0, IMG_HS1_S1, IMG_HS1_S2, IMG_HS1_S3,
            IMG_HS1_S4, IMG_HS1_S5, IMG_HS1_S6, IMG_HS1_S7,
            IMG_HS1_S8);

        tb = fmod(t,20);
        float ship2_x_interp = fclamp(tb/16, 0, 1);
        float ship2_y_interp = fclamp((tb-16.f)/4.f, 0, 1);
        int ship2_x = -160 + ship2_x_interp*400.f;
        int ship2_y = 230 + ship2_y_interp*36.f;
        interp = fclamp((tb-15.5)/3, 0, 1);
        draw_manager.draw(
            id(ID::SPACEPORT_SHIP2),
            ship2_anim.interp(interp),
            {ship2_x, ship2_y,
             .5f, .5f, 2, 2});
    }

    if (cls == Volcano) {
        int ship0_x = 350 - fmod(t+2,30)*13;
        draw_manager.draw(
            id(ID::SPACEPORT_SHIP0),
            IMG_HS2_S,
            {ship0_x, 148,
             .5f, .5f, 2, 2});

        static Anim ship1_anim(10,
            IMG_HS2_C1, IMG_HS2_C2, IMG_HS2_C3, IMG_HS2_C4, IMG_HS2_C5,
            IMG_HS2_C6, IMG_HS2_C7, IMG_HS2_C8, IMG_HS2_C9, IMG_HS2_C10);

        float tb = fmod(t,30);
        int ship1_x = 470 + (tb<8 ? 0 : fmod(t-8,30)*80);
        int ship1_y = 156 - tb*4;
        float interp = tb<7.5 ? 0 : fmin(1,tb-7.5);
        draw_manager.draw(
            id(ID::SPACEPORT_SHIP1),
            ship1_anim.interp(interp),
            {ship1_x, ship1_y,
             .5f, .5f, 2, 2});
    }

    if (cls == Rock) {
        int ship0_x = 800 - fmod(t+20,40)*20;
        int ship1_x = -100 + fmod(t+20,44)*18;
        draw_manager.draw(
            id(ID::SPACEPORT_SHIP0),
            IMG_HS3_S1,
            {ship0_x, 160,
             .5f, .5f, 2, 2});
        draw_manager.draw(
            id(ID::SPACEPORT_SHIP1),
            IMG_HS3_G1,
            {ship1_x, 208,
             .5f, .5f, 2, 2});
    }

    if (cls == Ice) {
        // TODO: Placeholder - check what this looks like in orig
        int ship0_x = 800 - fmod(t+20,40)*20;
        draw_manager.draw(
            id(ID::SPACEPORT_SHIP0),
            IMG_HS3_S1,
            {ship0_x, 160,
             .5f, .5f, 2, 2});
        int ship1_x = -100 + fmod(t+20,44)*18;
        draw_manager.draw(
            id(ID::SPACEPORT_SHIP1),
            IMG_HS3_G1,
            {ship1_x, 208,
             .5f, .5f, 2, 2});
    }

    if (cls == Terra) {
        // TODO: Placeholder - check what this looks like in orig
        int ship0_x = 800 - fmod(t+20,40)*20;
        draw_manager.draw(
            id(ID::SPACEPORT_SHIP0),
            IMG_HS3_S1,
            {ship0_x, 160,
             .5f, .5f, 2, 2});
        int ship1_x = -100 + fmod(t+20,44)*18;
        draw_manager.draw(
            id(ID::SPACEPORT_SHIP1),
            IMG_HS3_G1,
            {ship1_x, 208,
             .5f, .5f, 2, 2});
    }

    if (cls == Artificial) {
        // TODO: Placeholder - check what this looks like in orig
        // (I don't think this should even be possible!)
        int ship0_x = 800 - fmod(t+20,40)*20;
        draw_manager.draw(
            id(ID::SPACEPORT_SHIP0),
            IMG_HS3_S1,
            {ship0_x, 160,
             .5f, .5f, 2, 2});
        int ship1_x = -100 + fmod(t+20,44)*18;
        draw_manager.draw(
            id(ID::SPACEPORT_SHIP1),
            IMG_HS3_G1,
            {ship1_x, 208,
             .5f, .5f, 2, 2});
    }

    draw_manager.draw(
        id(ID::SPACEPORT2),
        p->sprites()->spaceport2,
        {x, y, 0, 0, 2, ys});

    frame_draw();

    first_spaceport_time += delta;

    if (draw_manager.clicked()) {
        draw_manager.draw(id(ID::SPACEPORT2), nullptr);
        draw_manager.draw(id(ID::SPACEPORT_SHIP0), nullptr);
        draw_manager.draw(id(ID::SPACEPORT_SHIP1), nullptr);
        draw_manager.draw(id(ID::SPACEPORT_SHIP2), nullptr);
        return false;
    }

    return true;
}

void GalaxyMap::next_mp_stage() {
    mp_state.mp_stage = (MonthPassStage)((int)mp_state.mp_stage + 1);
    L.debug("MP stage %d", mp_state.mp_stage);
    // Reset all mp-stage state tracking
    mp_state.mp_player_idx = 0;
    mp_state.mp_star_idx = 0;
    mp_state.mp_planet_idx = 0;
}

void GalaxyMap::next_mpai_stage() {
    mp_state.mpai_stage = (MonthPassAIStage)((int)mp_state.mpai_stage + 1);
    Player *p = exostate.get_active_player();
    L.verb("[%s] MPAI stage %d", p->get_full_name(), mp_state.mpai_stage);
    // Reset all mpai-stage state tracking
    mp_state.mpai_player_idx = 0;
    mp_state.mpai_star_idx = 0;
    mp_state.mpai_planet_idx = 0;
    mp_state.mpai_substage = 0;
}

void GalaxyMap::next_mpp_stage() {
    mp_state.mpp_stage = (MonthPassPlanetStage)((int)mp_state.mpp_stage + 1);
    Star *s = exostate.get_active_star();
    Planet *p = exostate.get_active_planet();
    L.verb("[%s/%s] MPP stage %d", s->name, p->get_name(), mp_state.mpp_stage);

}

void GalaxyMap::month_pass_start() {
    L.info("-------- MONTH PASSING --------");
    mp_state.month_pass_time = 0;
    mp_state.mpai_stage = (MonthPassAIStage)0;
    mp_state.mpp_stage = (MonthPassPlanetStage)0;

    mp_state.mp_player_idx = 0;
    mp_state.mp_star_idx = 0;
    mp_state.mp_planet_idx = 0;
    mp_state.mpai_player_idx = 0;
    mp_state.mpai_star_idx = 0;
    mp_state.mpai_planet_idx = 0;
    mp_state.mpai_substage = 0;

    if (mp_state.mp_stage != MP_None) {
        L.fatal("We started a month-pass whilst one was still in progress");
    }

    {
        int x = 160;
        int y = 180;
        int h = 40;

        draw_manager.fill(
            id(ID::MONTH_PASSING_BG),
            {x-2, y-2, RES_X - x*2+4, h+4},
            {0, 0, 0}
        );

        draw_manager.fill(
            id(ID::MONTH_PASSING),
            {x, y, RES_X - x*2, h},
            {0x50, 0x50, 0x50}
        );

        draw_manager.draw_text(
            "One month is passing by",
            Justify::Centre,
            RES_X / 2, y+9,
            COL_TEXT
        );
    }

    reset_planet_report();
    exostate.advance_month();
}

void GalaxyMap::month_pass_end() {
    mp_state.mp_stage = MP_None;
    mp_state.mpai_stage = (MonthPassAIStage)0;
    mp_state.mpp_stage = (MonthPassPlanetStage)0;

    L.info("-------- MONTH PASSED --------");

    Player *p;

    // Reset to the first active player
    bool reset;
    for (int i = 0; i < N_PLAYERS; ++i) {
        p = exostate.set_active_player(i);
        if (p && p->is_participating() && p->is_human()) {
            reset = true;
            break;
        }
    }

    if (!reset) {
        L.fatal("No viable human player to resume play!");
    }

    exostate.set_active_flytarget(exostate.loc2tgt(p->get_location().get_target()));
}

ExodusMode GalaxyMap::month_pass_update() {
    // Perform month updates here - but we can request a change of mode.
    // We don't need to do everything at once - so battles etc can happen
    // However, we should also be able to execute other GalaxyMap stages,
    // for example, to support enemy comms with us for trade dialogs etc.
    // We can set this directly and return from this update - but we must
    // *promise* to only enter a subset of month-pass-specific stages.
    // We *cannot* enter GM_Idle until all month pass logic is complete!

    int n_stars;
    Galaxy *gal = exostate.get_galaxy();
    Star *stars = gal->get_stars(n_stars);

    if (mp_state.mp_stage == MP_None) {
        month_pass_start();
        next_mp_stage();
        // Allow an update to display the 'waiting' dialogue...
        return ExodusMode::MODE_None;
    }

    if (mp_state.mp_stage == MP_TimeDelay) {
        // Time delay here so the "Month passing" notification doesn't flicker.
        if (mp_state.month_pass_time > 1.f) {
            draw_manager.draw(id(ID::MONTH_PASSING), nullptr);
            draw_manager.draw(id(ID::MONTH_PASSING_BG), nullptr);
            next_mp_stage();
        } else {
            return ExodusMode::MODE_None;
        }
    }

    if (mp_state.mp_stage == MP_CheckMissionFail) {
        // TODO: Check if we've run out of time for our mission
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_StarshipRepairs) {
        for (; mp_state.mp_player_idx < N_PLAYERS; ++mp_state.mp_player_idx) {
            Player *p = exostate.set_active_player(mp_state.mp_player_idx);
            Starship &ship = p->get_starship();
            int crew = ship.crew;
            ship.pct_damage_thrust -= (int)(crew / 5);
            ship.pct_damage_comms  -= (int)(crew / 5);
            ship.pct_damage_struct -= (int)(crew / 3);
            if (ship.pct_damage_thrust < 0) ship.pct_damage_thrust = 0;
            if (ship.pct_damage_comms  < 0) ship.pct_damage_comms  = 0;
            if (ship.pct_damage_struct < 0) ship.pct_damage_struct = 0;
        }
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_UpdateReputation) {
        for (; mp_state.mp_player_idx < N_PLAYERS; ++mp_state.mp_player_idx) {
            Player *p = exostate.set_active_player(mp_state.mp_player_idx);
            if (p->get_reputation() < 3 && onein(20)) {
                p->adjust_reputation(1);
            }
        }
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_SunExpansion) {
        // PROCsunexpand
        // FIXME: Ensure stars are drawn correctly when the bulletin is closed
        for (; mp_state.mp_star_idx < n_stars; ++mp_state.mp_star_idx) {
            exostate.set_active_flytarget(&stars[mp_state.mp_star_idx]);
            Star *s = exostate.get_active_star();

            Planet *fst = s->get_planet(0);
            Planet *snd = s->get_planet(1);

            StarSize sz = s->get_size();

            if ((int)(s->get_size()) > (int)STAR_Huge || onein(2000)) {
                sz = s->expand();
            }

            // TODO: Bulletin data from fillarray files SN1-SN4

            if ((int)sz > (int)STAR_Huge) {
                if ((int)sz < (int)STAR_Dwarf) {
                    bulletin_start_new(false);
                    bulletin_set_bg(IMG_ME4_MENU);
                    bulletin_set_flag(IMG_TS1_FLAG16);
                    bulletin_set_next_text("SUN %s IS EXPANDING", tmp_caps(s->name));
                }
            }

            switch (sz) {
                case STAR_Expand1:
                    if (fst && fst->exists()) fst->change_class(Desert);
                    break;
                case STAR_Expand2:
                    if (fst && fst->exists()) fst->change_class(Volcano);
                    if (snd && snd->exists()) snd->change_class(Desert);
                    break;
                case STAR_Expand3:
                    if (fst && fst->exists()) fst->destroy();
                    if (snd && snd->exists()) snd->change_class(Volcano);
                    break;
                case STAR_Expand4:
                    bulletin_start_new(false);
                    bulletin_set_bg(IMG_ME4_MENU);
                    bulletin_set_flag(IMG_TS1_FLAG16);
                    bulletin_set_next_text("SUN %s HAS COLLAPSED", tmp_caps(s->name));
                    for (int i = 0; i < 3; ++i) {
                        Planet *pl = s->get_planet(i);
                        if (pl && pl->exists()) {
                            pl->destroy();
                        }
                    }
                    for (int i = 3; i < 5; ++i) {
                        Planet *pl = s->get_planet(i);
                        if (pl && pl->exists() && pl->get_class() != Artificial) {
                            pl->change_class(Ice);
                        }
                    }
                    // TODO: Orig moves any players here to the guild
                    // TODO: Orig moves star if no planets
                    break;
                default:
                    break;
            }

            if (bulletin_is_open()) {
                // Ensure we start with the next star when we resume
                ++mp_state.mp_star_idx;
                return ExodusMode::MODE_None;
            }
        }
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_MoveArtificialPlanets) {
        // TODO - PROCarrivewp
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_EnemyTactics) {
        for (; mp_state.mp_player_idx < N_PLAYERS; ++mp_state.mp_player_idx) {
            Player *p = exostate.set_active_player(mp_state.mp_player_idx);
            if (p->is_human()) {
                continue;
            }

            // FIXME: Bit of a hack really, but ensures that bulletins
            // generated in the AI update (such as CPU player dying etc)
            // are always removed from the screen after being acknowledged.
            // This should really be expressed more cleanly...
            bulletin_ensure_closed();

            ExodusMode next_mode = month_pass_ai_update();
            if (mp_state.mpai_stage != MPAI_End) {
                return next_mode;
            }

            mp_state.mpai_stage = (MonthPassAIStage)0;
        }
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_ArtificialWorldNews) {
        // TODO - PROCwpc
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_EnemyActions) {
        // Orig: PROCenemytactics / PROCeta
        // This is planet maintenance performed by AI
        for (; mp_state.mp_star_idx < n_stars; ++mp_state.mp_star_idx) {
            for (; mp_state.mp_planet_idx < STAR_MAX_PLANETS; ++mp_state.mp_planet_idx) {
                Planet *p = stars[mp_state.mp_star_idx].get_planet(mp_state.mp_planet_idx);
                ai_planet_update(p);
            }
            mp_state.mp_planet_idx = 0;
        }
        next_mp_stage();
    }

    // TODO: Orig calls PROCfillstatus (counsellor messages and month report)
    // in between PROCenemytactics and PROCeta - is it OK to run it after both instead?

    if (mp_state.mp_stage == MP_NoPlanetsCounsellor) {
        if (comm_is_open()) {
            comm_ensure_closed();
        } else {
            Player *player = exostate.get_player(0);
            if (!exostate.multiplayer() && (exostate.get_n_planets(player) <= 0)) {
                int m = exostate.get_orig_month();
                // TODO: Orig thresholds here are 7 and 5 - not 5 and 3 - but these values
                // get the same behaviour. Check that other month thresholds are OK.
                if (m < 5) {
                    comm_open(m < 3 ? DIA_S_FirstPlanetAdvice : DIA_S_FirstPlanetAdviceUrgent);
                    return ExodusMode::MODE_None;
                }
            }
        }
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_EndOfMonthReport) {
        if (monthreport_is_open()) {
            if (draw_manager.clicked()) {
                monthreport_close();
            } else {
                return ExodusMode::MODE_None;
            }
        } else {
            Player *player = exostate.get_player(0);
            if (!exostate.multiplayer() && (exostate.get_n_planets(player) > 0)) {
                monthreport_open();
                return ExodusMode::MODE_None;
            }
        }
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_UpdatePirateProbabilities) {
        for (; mp_state.mp_star_idx < n_stars; ++mp_state.mp_star_idx) {
            Star *s = &stars[mp_state.mp_star_idx];
            int n_owned = 0;
            for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
                Planet *p = s->get_planet(i);
                if (p && p->exists() && p->is_owned()) {
                    n_owned++;
                }
                if (n_owned == 0) {
                    s->pirates = RND(3);
                } else if (n_owned < 3) {
                    s->pirates = RND(2);
                } else if (n_owned < 5) {
                    s->pirates = 2;
                } else {
                    s->pirates = 3;
                }
            }
        }
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_PlanetBackgroundUpdate) {
        for (; mp_state.mp_star_idx < n_stars; ++mp_state.mp_star_idx) {
            for (; mp_state.mp_planet_idx < STAR_MAX_PLANETS; ++mp_state.mp_planet_idx) {
                Planet *p = stars[mp_state.mp_star_idx].get_planet(mp_state.mp_planet_idx);
                if (!(p && p->exists()))
                    continue;

                p->month_reset();

                if (onein(7)) p->adjust_unrest(-1);

                if (p->is_owned()) {
                    if (onein(250)) p->surfchange();
                }

                // TODO: Owned planet maintenance
            }
            mp_state.mp_planet_idx = 0;
        }
        next_mp_stage();
    }

    /*
     * These are the updates that were originally performed by PROCcal_plan.
     * This is a little awkward - ideally each of these 'MPP' stages would be a
     * separate 'MP' stage here, as all planets are independent - however that
     * doesn't really work, because a report containing info across multiple stages
     * is generated for each planet - so we'd have to record things to report on later
     * on a per-planet basis. This wouldn't be a problem, except that certain events
     * in the planet update require user interaction (rebel attacks, paying to avoid
     * military facility shutdown etc) - and need to be resolved to inform later stages
     * (units lost in rebel battle free up garrison space which affects later unit
     * production etc). Essentially, we *can't* complete any planet report without
     * the ability to exit from this update and return to it after an interactive
     * resolution. Furthermore, when we perform interactions, we don't want the player
     * to see e.g. interactive stage for Planet A, interactive stage for Planet B,
     * report for Planet A then report for Planet B - we want to deal with each planet
     * in its entirety one at a time.
     * As a resolution, we have a separate 'MPP' stage-set which is tracked independently
     * of the 'MP' stages which will always complete one planet at a time, which still
     * allows us to temporarily exit into other modes or mode stages at any time.
     */
    if (mp_state.mp_stage == MP_PlanetMainUpdate) {
        for (; mp_state.mp_star_idx < n_stars; ++mp_state.mp_star_idx) {
            exostate.set_active_flytarget(&stars[mp_state.mp_star_idx]);
            for (; mp_state.mp_planet_idx < STAR_MAX_PLANETS; ++mp_state.mp_planet_idx) {
                Planet *p = stars[mp_state.mp_star_idx].get_planet(mp_state.mp_planet_idx);
                if (!(p && p->exists())) {
                    continue;
                }

                // This only applies to owned planets...
                if (!p->is_owned()) {
                    // ...Unless we've already started processing this planet,
                    // in which case we should proceed.
                    if (!p->monthly_processing_in_progress()) {
                        continue;
                    }
                }

                p->monthly_processing_start();

                exostate.set_active_planet(mp_state.mp_planet_idx);
                ExodusMode next_mode = month_pass_planet_update();

                if (mp_state.mpp_stage != MPP_End) {
                    return next_mode;
                }

                mp_state.mpp_stage = (MonthPassPlanetStage)0;
                reset_planet_report();
            }
            mp_state.mp_planet_idx = 0;
        }
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_UpdateAlienFly) {
        for (int i = 0; i < N_PLAYERS; ++i) {
            Player *p = exostate.get_player(i);
            if (p && p->is_participating() && (!p->is_human())) {
                if (p->get_location().in_flight()) {
                    if (!p->get_location().advance()) {
                        L.warn("CPU players shouldn't take >1 month to reach destination");
                    }
                    if (p->get_tactic() > 0) {
                        p->next_tactic();
                    }
                }
            }
        }
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_Missions) {
        for (; mp_state.mp_player_idx < N_PLAYERS; ++mp_state.mp_player_idx) {
            Player *p = exostate.set_active_player(mp_state.mp_player_idx);
            const Mission mission = p->get_mission();

            if (active_mission) {
                if (p->has_mission()) {
                    // Resolve the mission here
                    active_mission = false;
                    L.debug("[%s] EXECUTE MISSION", p->get_full_name());

                    Planet *pl = exostate.get_active_planet();
                    int owner_idx = pl->get_owner();
                    Player *owner = exostate.get_player(owner_idx);

                    switch (mission.type) {
                        case MT_None:
                            L.error("No mission type - has_mission() should be false");
                            break;
                        case MT_TerrorComm:
                        case MT_TerrorPlu:
                        case MT_TerrorArmy:
                        case MT_TerrorPort:
                        case MT_TerrorTrade:
                        case MT_TerrorMine:
                            p->clear_mission();
                            if (ephstate.get_ephemeral_state() == EPH_Destruction) {
                                L.debug("Terrorist mission took effect");
                                return ephstate.get_appropriate_mode();
                            } else {
                                L.debug("Terrorist mission did not take effect");
                            }

                            switch (mission.type) {
                                case MT_TerrorComm:
                                    pl->adjust_unrest(1);
                                    break;
                                case MT_TerrorPlu:
                                    pl->adjust_unrest(2);
                                    break;
                                case MT_TerrorArmy:
                                    pl->adjust_unrest(1);
                                    break;
                                case MT_TerrorPort:
                                    pl->adjust_unrest(1);
                                    break;
                                case MT_TerrorTrade:
                                    pl->adjust_unrest(2);
                                    break;
                                case MT_TerrorMine:
                                    pl->adjust_unrest(1);
                                    break;
                                default:
                                    L.error("Mission %d should have unrest case", mission.type);
                                    break;
                            }
                            break;
                        case MT_TerrorAgri:
                            {
                                p->clear_mission();

                                pl->adjust_unrest(1);

                                int to_die = RND(15);
                                if (pl->count_stones(STONE_Agri) < 15) to_die = RND(5);

                                L.debug("Executing plant poisoning: %d units", to_die);

                                int sz = pl->get_size_blocks();
                                for (int j = 0; j < sz; ++j) {
                                    for (int i = 0; i < sz; ++i) {
                                        if (pl->get_stone(i, j) == STONE_Agri) {
                                            L.debug("Poisoning %d,%d", i, j);
                                            pl->set_stone(i, j, STONE_AgriDead);
                                            --to_die;
                                            if (to_die <= 0) break;
                                        }
                                    }
                                    if (to_die <= 0) break;
                                }

                            }
                            break;
                        case MT_RandomBomb:
                            p->clear_mission();
                            ephstate.set_ephemeral_state(EPH_Destruction);
                            ephstate.destruction.type = DESTROY_NRandom;
                            ephstate.destruction.tgt_stones.reset();
                            ephstate.destruction.n_strikes = mission_n_strikes;
                            ephstate.destruction.enable_explosions = true;
                            ephstate.destruction.enable_explosion_anims = true;
                            ephstate.destruction.irradiated = false;
                            ephstate.destruction.show_target = false;
                            // Enemy doesn't know you've attacked them (TODO: Check this)
                            ephstate.destruction.destroyer_idx = -1;
                            ephstate.destruction.terror = false;
                            ephstate.destruction.nuke = false;
                            ephstate.destruction.draw = owner->is_human();
                            return ephstate.get_appropriate_mode();
                        case MT_Nuclear:
                            p->clear_mission();
                            ephstate.set_ephemeral_state(EPH_Destruction);
                            ephstate.destruction.type = DESTROY_NRandom;
                            ephstate.destruction.tgt_stones.reset();
                            ephstate.destruction.n_strikes = 400;
                            ephstate.destruction.enable_explosions = false;
                            ephstate.destruction.enable_explosion_anims = false;
                            ephstate.destruction.irradiated = true;
                            ephstate.destruction.show_target = false;
                            // Enemy doesn't know you've attacked them (TODO: Check this)
                            ephstate.destruction.destroyer_idx = -1;
                            ephstate.destruction.terror = false;
                            ephstate.destruction.nuke = true;
                            ephstate.destruction.draw = true;
                            return ephstate.get_appropriate_mode();
                    }
                    continue;
                } else {
                    L.error("Player should have mission if active_mission set");
                }
            }

            L.debug("CHECK MISSION %d", mp_state.mp_player_idx);
            if (p->has_mission()) {
                Star *st = &stars[mission.star_idx];
                exostate.set_active_flytarget(st);
                exostate.set_active_planet(mission.planet_idx);
                Planet *pl = exostate.get_active_planet();
                if (pl && pl->is_owned()) {
                    int owner_idx = pl->get_owner();
                    Player *owner = exostate.get_player(owner_idx);

                    exostate.set_active_player(mp_state.mp_player_idx);
                    L.info("[%s] MISSION - %s (%s)", p->get_full_name(), pl->get_name(), owner->get_full_name());
                    bulletin_start_new(false);
                    bulletin_set_bg(pl->sprites()->bulletin_bg);
                    bulletin_set_active_player_flag();
                    bulletin_write_planet_info(st, pl);
                    // Just set bulletin here - if we need to trigger an action
                    // on close, set active_mission and return
                    // Otherwise, p->clear_mission();

                    switch (mission.type) {
                        case MT_None:
                            L.error("Mission with no type - has_mission() should be false");
                            break;
                        case MT_TerrorComm:
                        case MT_TerrorAgri:
                        case MT_TerrorPlu:
                        case MT_TerrorArmy:
                        case MT_TerrorPort:
                        case MT_TerrorTrade:
                        case MT_TerrorMine:
                            {
                                // PROCspyspot
                                int r = RND(owner->has_invention(INV_IndustryGuard) ? 5 : 10);
                                // TODO: r increases with attacker's officer quality
                                if (r < 4) {
                                    // TODO: Spy captured
                                    L.info("[%s] SPY CAPTURED - %s", p->get_full_name(), pl->get_name());
                                    //audio_manager.target_music(mpart2mus(9));
                                    p->clear_mission();
                                } else {
                                    audio_manager.target_music(mpart2mus(9));

                                    if (mission.type == MT_TerrorAgri) {
                                        L.debug("Mission type: Poison plants");
                                        exostate.register_news(NI_PlantsPoisoned);
                                        bulletin_start_new(true);
                                        bulletin_set_bg(pl->sprites()->bulletin_bg);
                                        bulletin_set_active_player_flag();
                                        bulletin_write_planet_info(st, pl);
                                        bulletin_set_next_text("PLANTS POISONED AT %s", tmp_caps(pl->get_name()));
                                        bulletin_set_next_text("");
                                        bulletin_set_next_text("Large parts of cultivated area");
                                        bulletin_set_next_text("have collapsed.");
                                    } else {
                                        L.debug("Mission type: Terrorist bombing");

                                        ephstate.destruction.tgt_stones.reset();

                                        switch (mission.type) {
                                            case MT_TerrorComm:
                                                ephstate.destruction.tgt_stones.add(STONE_Base);
                                                break;
                                            case MT_TerrorPlu:
                                                ephstate.destruction.tgt_stones.add(STONE_Plu);
                                                break;
                                            case MT_TerrorArmy:
                                                switch (rand() % 3) {
                                                    case 0:
                                                        ephstate.destruction.tgt_stones.add(STONE_Inf);
                                                        break;
                                                    case 1:
                                                        ephstate.destruction.tgt_stones.add(STONE_Gli);
                                                        break;
                                                    case 2:
                                                        ephstate.destruction.tgt_stones.add(STONE_Art);
                                                        break;
                                                }
                                                break;
                                            case MT_TerrorPort:
                                                // We only target the power station here
                                                ephstate.destruction.tgt_stones.add(STONE_Port2);
                                                break;
                                            case MT_TerrorTrade:
                                                ephstate.destruction.tgt_stones.add(STONE_Trade);
                                                break;
                                            case MT_TerrorMine:
                                                ephstate.destruction.tgt_stones.add(STONE_Mine);
                                                break;
                                            default:
                                                L.error("Mission type %d should not be processed here", mission.type);
                                                break;
                                        }


                                        bool did_anything = pl->count_stones(ephstate.destruction.tgt_stones) > 0;

                                        if (did_anything) {
                                            ephstate.set_ephemeral_state(EPH_Destruction);
                                            ephstate.destruction.type = DESTROY_NStones;
                                            ephstate.destruction.n_strikes = 1;
                                            ephstate.destruction.enable_explosions = true;
                                            ephstate.destruction.enable_explosion_anims = true;
                                            ephstate.destruction.irradiated = false;
                                            ephstate.destruction.show_target = true;
                                            // Enemy doesn't know you've attacked them (TODO: Check this)
                                            ephstate.destruction.destroyer_idx = -1;
                                            ephstate.destruction.terror = true;
                                            ephstate.destruction.nuke = false;
                                            ephstate.destruction.draw = owner->is_human();
                                        } else {
                                            // Things should clear this before use anyway, but we'll tidy this up as we don't need it
                                            ephstate.destruction.tgt_stones.reset();
                                        }

                                        exostate.register_news(NI_TerroristAttack);
                                        bulletin_start_new(true);
                                        bulletin_set_bg(pl->sprites()->bulletin_bg);
                                        bulletin_set_active_player_flag();
                                        bulletin_write_planet_info(st, pl);
                                        bulletin_set_next_text("TERRORIST ATTCK AT %s", tmp_caps(pl->get_name()));

                                        if (did_anything) {
                                            bulletin_set_next_text("");
                                            if (ephstate.destruction.draw) {
                                                bulletin_set_next_text("Visual replay follows.");
                                            } else {
                                                bulletin_set_next_text("A technical unit has been destroyed.");
                                            }
                                        }
                                    }

                                    // TODO: Spy captured case probably needs to set this as well, so we can move it up one level
                                    active_mission = true;
                                    return ExodusMode::MODE_None;
                                }
                            }
                            break;
                        case MT_RandomBomb:
                            mission_n_strikes = RND(7) + 9;
                            bulletin_set_next_text("");
                            bulletin_set_next_text("");
                            bulletin_set_next_text(
                                "ORBITAL ATTACK AT %s",
                                tmp_caps(pl->get_name()));
                            bulletin_set_next_text(
                                "The planet has taken %d hits.",
                                mission_n_strikes);
                            active_mission = true;
                            return ExodusMode::MODE_None;
                        case MT_Nuclear:
                            bulletin_set_next_text("");
                            bulletin_set_next_text("");
                            audio_manager.target_music(mpart2mus(9));
                            bulletin_set_next_text(
                                "NUCLEAR ATTACK AT %s",
                                tmp_caps(pl->get_name()));
                            active_mission = true;
                            return ExodusMode::MODE_None;
                    }
                } else {
                    L.info("[%s]: CANCEL MISSION: Invalid planet", p->get_full_name());
                }
            }
        }
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_PayOfficers) {
        // TODO - PROCpayoff etc
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_GuildCommendations) {
        Galaxy *gal = exostate.get_galaxy();
        int n_stars;
        gal->get_stars(n_stars);
        int ab = 2 + (n_stars / 20);
        for (; mp_state.mp_player_idx < N_PLAYERS; ++mp_state.mp_player_idx) {
            Player *p = exostate.set_active_player(mp_state.mp_player_idx);
            if (p && p->is_participating() && p->is_human()) {
                GuildTitle title = p->get_guild_title();
                int title_int = (int)title;
                int planets = exostate.get_n_planets(p);
                int ac = planets / ab;
                if (ac < (int)GUILDTITLE_MAX) {
                    if (ac > title_int) {
                        // New title
                        GuildTitle new_title = (GuildTitle)ac;
                        p->set_guild_title(new_title);
                        const char* name = p->get_full_name();
                        const char* title_str = p->get_guild_title_str();
                        audio_manager.target_music(mpart2mus(10));
                        bulletin_start_new(false);
                        bulletin_set_flag(IMG_TS1_FLAG16);
                        bulletin_set_text_col(COL_TEXT3);
                        bulletin_set_next_text("NEW TITLE FOR %s", tmp_caps(p->get_full_name()));
                        bulletin_set_next_text("");
                        bulletin_set_next_text("You now own the title");
                        bulletin_set_next_text("%s \"%s\"", name, title_str);
                        bulletin_set_next_text("");
                        switch (new_title) {
                            case GUILDTITLE_Warrior:
                                bulletin_set_next_text("This title shows that you are able to");
                                bulletin_set_next_text("rule even more than just a single planet.");
                                bulletin_set_next_text("But your enemies see your actions as a");
                                bulletin_set_next_text("forthcoming danger.");
                                break;
                            case GUILDTITLE_Strong:
                                bulletin_set_next_text("With your growing number of planets,");
                                bulletin_set_next_text("there also seem to be more and more");
                                bulletin_set_next_text("enemies.");
                                bulletin_set_next_text("Now is the time to build a strong");
                                bulletin_set_next_text("and powerful empire.");
                                break;
                            case GUILDTITLE_Conqueror:
                                bulletin_set_next_text("Not many lords have claimed this title");
                                bulletin_set_next_text("yet. It shows your power and your skill.");
                                bulletin_set_next_text("But this must not be all you are able");
                                bulletin_set_next_text("to achieve. You have to become even");
                                bulletin_set_next_text("stronger.");
                                break;
                            case GUILDTITLE_Mighty:
                                bulletin_set_next_text("Now you are a strong and powerful lord.");
                                bulletin_set_next_text("There are not many with the same power");
                                bulletin_set_next_text("in this galaxy. But can you fulfil your");
                                bulletin_set_next_text("mission?");
                                bulletin_set_next_text("It is up to you.");
                                break;
                            case GUILDTITLE_Great:
                                bulletin_set_next_text("The time has come.");
                                bulletin_set_next_text("You are closer to your aim than you");
                                bulletin_set_next_text("have ever been. But you have not");
                                bulletin_set_next_text("reached it yet.");
                                bulletin_set_next_text("Good luck.");
                                break;
                            case GUILDTITLE_Warlord:
                                bulletin_set_next_text("Go on.");
                                bulletin_set_next_text("You alone can do it.");
                                break;
                            case GUILDTITLE_Emperor:
                                bulletin_set_next_text("The future is in your hand.");
                                break;
                            default:
                                break;
                        }

                        do_guild_title = new_title;
                        return ExodusMode::MODE_None;
                    }
                }
            }
        }
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_AlienExile) {
        // PROCcheckalive
        // TODO: In a multiplayer game, check if all humans are dead, or one winner remains
        for (; mp_state.mp_player_idx < N_PLAYERS; ++mp_state.mp_player_idx) {
            Player *p = exostate.set_active_player(mp_state.mp_player_idx);
            // Check all CPU lords participating...
            if (p && p->is_participating() && !p->is_human()) {
                // ...who don't have a planet...
                if (exostate.get_n_planets(p) == 0) {
                    if (p->can_afford(150)) {
                        p->set_tactic(9);
                        // Ensure CPU is at destination
                        p->get_location().advance();
                    } else if (p->leave_galaxy()) {
                        // ...and can't afford to colonise a new one.
                        audio_manager.target_music(mpart2mus(9));
                        bulletin_start_new(false);
                        bulletin_set_flag(IMG_TS1_FLAG16);
                        bulletin_set_text_col(COL_TEXT3);
                        bulletin_set_next_text("%s FAILED", tmp_caps(p->get_name()));
                        bulletin_set_next_text("");
                        bulletin_set_next_text("%s has lost all planets and", p->get_full_name());
                        bulletin_set_next_text("left the galaxy.");
                        // TODO: News item with PROCdonotice
                        return ExodusMode::MODE_None;
                    }
                }
            }
        }

        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_UpdateHumanFly) {
        for (; mp_state.mp_player_idx < N_PLAYERS; ++mp_state.mp_player_idx) {
            Player *p = exostate.set_active_player(mp_state.mp_player_idx);
            if (p && p->is_participating() && p->is_human()) {
                if (p->get_location().advance()) {
                    // Show arrival animation for human players
                    if (p->is_human()) {
                        return ExodusMode::MODE_Fly;
                    }
                }
            }
        }
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_End) {
        // When we decide we're done with updates...
        month_pass_end();
        return ExodusMode::MODE_None;
    }

    return ExodusMode::MODE_None;
}

ExodusMode GalaxyMap::month_pass_ai_update() {
    Player *player = exostate.get_active_player();
    int player_idx = exostate.get_player_idx(player);

    Galaxy *gal = exostate.get_galaxy();
    int n_stars;
    Star *stars = gal->get_stars(n_stars);

    if (mp_state.mpai_stage == MPAI_Return) {
        if (exostate.get_n_active_cpu_players() > 0) {
            if (onein(400) && (exostate.get_n_unowned_planets() > 0) && player->return_to_galaxy()) {
                int m = exostate.get_orig_month();
                player->give_mc(RND(300) + (m*50));
                player->set_tactic(9);
                player->get_location().set_target(exostate.get_random_star_idx(), 1);
                player->get_location().advance();
                player->transfer_inf(m/2);
                player->transfer_gli(m/2);
                player->transfer_art(m/2);
                audio_manager.target_music(mpart2mus(8));
                bulletin_start_new(false);
                bulletin_set_flag(IMG_TS1_FLAG16);
                bulletin_set_text_col(COL_TEXT3);
                bulletin_set_next_text("%s HAS RETURNED", tmp_caps(player->get_name()));
                bulletin_set_next_text("");
                bulletin_set_next_text("We have just received the news that");
                bulletin_set_next_text("%s has come back.", player->get_full_name());
                next_mpai_stage();
                return ExodusMode::MODE_None;
            }
        }
        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_Die) {
        if (exostate.get_n_active_cpu_players() > 0 && exostate.get_orig_month() > 50) {
            if (onein(1000)) {
                char oldname[MAX_PLAYER_NAME];
                char oldfullname[MAX_PLAYER_FULLNAME];
                snprintf(oldname, MAX_PLAYER_NAME, player->get_name());
                snprintf(oldfullname, MAX_PLAYER_FULLNAME, player->get_full_name());
                Gender oldgender = player->get_gender();
                if (exostate.kill(player)) {
                    audio_manager.target_music(mpart2mus(9));
                    bulletin_start_new(false);
                    bulletin_set_flag(IMG_TS1_FLAG16);
                    bulletin_set_text_col(COL_TEXT3);
                    bulletin_set_next_text("%s IS DEAD", tmp_caps(oldname));
                    bulletin_set_next_text("");
                    bulletin_set_next_text("For reasons unknown,");
                    bulletin_set_next_text("%s has died.", oldfullname);
                    bulletin_set_next_text("");
                    bulletin_set_next_text("The empire has been taken over by");
                    bulletin_set_next_text("%s follower, %s.",
                                           (oldgender == GENDER_Female) ? "her" : "his",
                                           player->get_full_name());
                    next_mpai_stage();
                    return ExodusMode::MODE_None;
                }
            }
        }
        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_Hostilities) {
        if (onein(40)) {
            // If we're hostile to someone, 1/40 chance we forgive them
            // If we're hostile to no-one, 1/40 chance we gain a hostility
            Player *hostile_to = exostate.get_hostile_to(*player);
            if (hostile_to) {
                player->clear_hostility();
            } else {
                exostate.set_random_hostility(*player);
            }
        }
        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_Alliances) {
        bool resume = false;

        if (mp_state.mpai_substage == 3) {
            // TODO: Respond to lunar battle report
            ephstate.clear_ephemeral_state();
            mp_state.mpai_substage = 0;
            resume = true;
        }

        if (mp_state.mpai_substage == 2) {
            switch (comm_action_check()) {
                case CA_Abort:
                    comm_close();
                    mp_state.mpai_substage = 0;
                    resume = true;
                    break;
                case CA_Attack:
                    comm_close();
                    mp_state.mpai_substage = 3;
                    ephstate.set_ephemeral_state(EPH_LunarBattlePrep);
                    ephstate.lunar_battle.aggressor_type = AGG_Player;
                    ephstate.lunar_battle.aggressor_idx = player_idx;
                    return ephstate.get_appropriate_mode();
                default:
                    L.fatal("Unexpected comm action on CPU alliance offer: %d");
            }
        }

        if (mp_state.mpai_substage == 1) {
            // Arrival cutscene finished
            comm_open(DIA_S_CPU_Offer);
            mp_state.mpai_substage = 2;
            return ExodusMode::MODE_None;
        }

        if (!player->get_location().in_flight()) {
            int r = 20;
            if (exostate.count_alliances(player_idx) <= 2) {
                switch (player->get_flag(2)) {
                    case AI_Hi:
                        r = 4;
                        break;
                    case AI_Md:
                        r = 6;
                        break;
                    case AI_Lo:
                        r = 10;
                        break;
                }
            }
            if (resume || onein(r)) {
                // PROCet_alliances
                // Can cast - AI never visit the guild
                Star *s = (Star*)exostate.loc2tgt(player->get_location().get_target());
                int &i = mp_state.mpai_planet_idx;
                for (; i < STAR_MAX_PLANETS; ++i) {
                    Planet *p = s->get_planet(i);
                    if (!(p && p->exists() && p->is_owned())) {
                        continue;
                    }

                    exostate.set_active_flytarget(s);
                    exostate.set_active_planet(i);

                    int owner_idx = p->get_owner();
                    if (owner_idx == player_idx) {
                        continue;
                    }
                    Player *owner = exostate.get_player(owner_idx);

                    if (player->is_hostile_to(owner_idx)) {
                        continue;
                    }

                    if (owner->is_human()) {
                        if (!onein(10)) {
                            continue;
                        }
                        if (exostate.has_all_alliances(player_idx, owner_idx)) {
                            continue;
                        }
                        if (owner->get_reputation() <= 0) {
                            continue;
                        }
                        mp_state.mpai_substage = 1;
                        // When we resume, we'll want to start with the next planet
                        mp_state.mpai_planet_idx++;
                        return ExodusMode::MODE_Arrive;
                    } else {
                        if (!onein(16)) {
                            continue;
                        }
                        if (exostate.is_allied(player_idx, owner_idx)) {
                            continue;
                        }
                        if (player->get_reputation() <= 0) {
                            continue;
                        }
                        exostate.set_all_alliances(player_idx, owner_idx);
                    }
                }

                int r = 50;
                if (player->get_flag(1) == AI_Md) {
                    r = 20;
                } else if (player->get_flag(1) == AI_Lo) {
                    r = 10;
                }
                // FIXME: Randomly lose alliances - should exclude player?
                for (int i = 0; i < N_PLAYERS; ++i) {
                    if (onein(r)) {
                        exostate.unset_alliances(player_idx, i);
                    }
                }
            }
        }
        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_UpdateTaxes) {
        if (exostate.get_orig_month() < 25) {
            player->set_tax(0);
        } else if (player->get_tactic() != 6) {
            player->init_tax();
        }
        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_DecideTerrorAttacks) {
        // TODO
        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_FleetPurchase) {
        // TODO
        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_NewOfficers) {
        if (exostate.get_orig_month() > 20 && onein(2)) {
            for (int i = 0; i < 3; ++i) {
                Officer off = OFF_Battle;
                if (i == 1) off = OFF_Science;
                if (i == 2) off = OFF_Secret;
                if (player->get_officer(off) < OFFQ_Good) {
                    if (player->attempt_spend_with_remaining(800, 1)) {
                        player->set_officer(off, OFFQ_Good);
                        break;
                    }
                }
                if (player->get_officer(off) < OFFQ_Average) {
                    if (player->attempt_spend_with_remaining(400, 1)) {
                        player->set_officer(off, OFFQ_Average);
                        break;
                    }
                }
            }
        }
        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_DevelopArtificialPlanet) {
        // TODO
        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_MoveArtificialPlanet) {
        // TODO
        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_CheckArmy) {
        if (exostate.owns_a_planet(player)) {
            int biggest_army = 0;
            for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
                Star *s = &stars[star_idx];
                for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                    Planet *p = s->get_planet(planet_idx);
                    if (!(p && p->exists())) {
                        continue;
                    }
                    if (p->get_owner() == player_idx) {
                        if (p->army_full() && player->get_tactic() == 0) {
                            player->set_tactic(1);
                        }
                        int army_sz = p->get_army_size();
                        if (army_sz > biggest_army) {
                            biggest_army = army_sz;
                        }
                    } else if (p->is_owned()) {
                        if (!onein(5)) {
                            continue;
                        }
                        int t = player->get_tactic();
                        if (!(t == 0 || t == 1)) {
                            continue;
                        }
                        if (exostate.get_orig_month() <= 9) {
                            continue;
                        }
                        if (exostate.has_alliance(p->get_owner(), player_idx, ALLY_War)) {
                            continue;
                        }
                        if (biggest_army/3 >= p->get_army_size()) {
                            player->set_tactic(1);
                            player->set_ai_attack(star_idx, planet_idx);
                        }
                    }
                }
            }
        }
        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_SwitchTactics) {
        // PROCet_newtact
        if (player->get_tactic() == 0 && onein(3)) {
            L.debug("[%s] TACTIC CURRENT: %d", player->get_full_name(), player->get_tactic());
            int r = RND(6);
            int t = 0;
            switch (player->get_flag(0)) {
                case AI_Hi:
                    if (r < 4) {
                        t = 1;
                    } else if (r == 4) {
                        t = 3;
                    } else {
                        t = 2;
                    }
                    break;
                case AI_Md:
                    t = RND(3);
                case AI_Lo:
                    r = RND(8);
                    if (r < 5) {
                        t = 2;
                    } else if (r < 8) {
                        t = 3;
                    } else {
                        t = 1;
                    }
                    break;
            }

            int m = exostate.get_orig_month();
            if (t == 2 && m < 12) {
                t = 0;
            }

            if (t == 1 && m < 40 && (!onein(3))) {
                t = 0;
            }

            if (m > 110 && onein(3)) {
                t = 1;
            }

            if (player->can_afford(2001)) {
                t = 2;
            }

            if (t == 3) {
                player->set_tactic(20);
            }

            bool afford = player->can_afford(130);
            if (t == 2) {
                if (afford || exostate.get_total_net_income(player_idx) >= 9) {
                    int m = exostate.get_orig_month();
                    if (player->can_afford(m > 30 ? 31 : 61)) {
                        player->set_tactic(6);
                        player->get_location().unset_target();
                    } else {
                        player->set_tactic(20);
                    }
                } else {
                    player->set_tactic(20);
                }
            }

            if (t == 1) {
                player->set_tactic(1);
                player->get_location().unset_target();
            }

            int army = player->get_fleet().freight.army_size();
            if (army > 0) {
                bool done = false;
                for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
                    Star *s = &stars[star_idx];
                    for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                        Planet *p = s->get_planet(planet_idx);
                        if (p && p->exists() && p->is_owned()) {
                            if (p->get_owner() == player_idx) {
                                continue;
                            }
                            if (p->get_army_size() > 0) {
                                continue;
                            }
                        }
                        player->set_tactic(4);
                        // TODO: Fairly certain CPU journeys are always 1 month, but check
                        player->get_location().set_target(star_idx, 1);
                        player->get_location().set_planet_target(planet_idx);
                        done = true;
                        break;
                    }
                    if (done) {
                        break;
                    }
                }
            }

            if (!exostate.owns_a_planet(player)) {
                player->set_tactic(9);
            }

            if (player->get_tactic() == 6) {
                bool ok = false;
                for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
                    Star *s = &stars[star_idx];
                    for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                        Planet *p = s->get_planet(planet_idx);
                        if (p && p->exists() && !p->is_owned()) {
                            ok = true;
                            break;
                        }
                    }
                    if (ok) {
                        break;
                    }
                }
                if (!ok) {
                    player->set_tactic(1);
                }
            }
            L.debug("[%s] TACTIC NEW: %d", player->get_full_name(), player->get_tactic());
        }
        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_ExecTactics) {
        Player *p = player;
        if (p->get_tactic() > 0) {
            if (p->get_tactic() == 6) {
                p->set_tax(0);
            }
            if (p->get_tactic() != 6 && exostate.get_orig_month() > 25) {
                p->init_tax();
            }
            if (p->get_tactic() == 6 && p->can_afford(191)) {
                p->set_tactic(7);
            }
            if (p->get_tactic() == 7 || p->get_tactic() == 1) {
                /*
                 * PROCe_tact1
                 *
                 * Travel to the planet owned by us with the largest army
                 */
                L.debug("[%s] PROCe_tact1 : FLEET -> LARGE ARMY", player->get_full_name());
                player->get_location().unset_target();
                int quality = 0;
                for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
                    Star *s = &stars[star_idx];
                    for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                        Planet *p = s->get_planet(planet_idx);
                        if (p && p->exists() && p->is_owned() && p->get_owner() == player_idx) {
                            int army = p->get_army_size();
                            if (army > quality) {
                                quality = army;
                                player->get_location().set_target(star_idx, 1);
                                player->get_location().set_planet_target(planet_idx);
                                L.debug("[%s] PROCe_tact1 : Targeting %s", player->get_full_name(), p->get_name());
                            }
                        }
                    }
                }
                if (player->get_location().in_flight()) {
                    L.debug("[%s] PROCe_tact1 : Travelling", player->get_full_name());
                    if (player->has_invention(INV_OrbitalMassThrust) && onein(2)) {
                        player->next_tactic();
                        player->get_location().complete();
                    }
                } else {
                    player->next_tactic();
                }
                if (player->get_tactic() == 1) {
                    int star_idx = player->get_location().get_target();
                    int planet_idx = player->get_location().get_planet_target();
                    Star *star = &stars[star_idx];
                    Planet *planet = star->get_planet(planet_idx);
                    if (!(planet && planet->exists())) {
                        L.error("No planet in PROCe_tact1: %s %d", star->name, planet_idx);
                    }
                    if (planet->get_army_size() < 4) {
                        player->set_tactic(0);
                        player->get_location().complete();
                    }
                }
                L.debug("[%s] PROCe_tact1 END", player->get_full_name());
            }
            if (!p->get_location().in_flight() && (p->get_tactic() == 8 || p->get_tactic() == 2)) {
                /*
                 * PROCe_tact2
                 *
                 * Transfer some battle units from our target planet to our fleet
                 */
                L.debug("[%s] PROCe_tact2 : BATTLE UNITS -> FLEET", player->get_full_name());
                int star_idx = player->get_location().get_target();
                int planet_idx = player->get_location().get_planet_target();
                Star *star = &stars[star_idx];
                Planet *planet = star->get_planet(planet_idx);
                const Freight& f = player->get_fleet().freight;
                if (planet && planet->exists() && planet->get_owner() == player_idx) {
                    int planet_inf, planet_gli, planet_art;
                    planet->get_army(planet_inf, planet_gli, planet_art);
                    L.debug("[%s] PROCe_tact2 : PLANET %d/%d/%d", player->get_full_name(), planet_inf, planet_gli, planet_art);
                    L.debug("[%s] PROCe_tact2 : FLEET %d/%d/%d", player->get_full_name(), f.infantry, f.gliders, f.artillery);
                    L.debug("[%s] PROCe_tact2 : Transferring...", player->get_full_name());
                    for (int i = 0; i < 3 && player->get_freight_capacity() > 0; ++i) {
                        switch (i) {
                            case 0:
                                planet->adjust_army(0, 0, player->transfer_art(planet_art/3));
                                break;
                            case 1:
                                planet->adjust_army(0, player->transfer_gli(planet_gli/3), 0);
                                break;
                            case 2:
                                planet->adjust_army(player->transfer_inf(planet_inf/3), 0, 0);
                                break;
                        }
                    }

                    planet->get_army(planet_inf, planet_gli, planet_art);
                    L.debug("[%s] PROCe_tact2 : PLANET %d/%d/%d", player->get_full_name(), planet_inf, planet_gli, planet_art);
                    L.debug("[%s] PROCe_tact2 : FLEET %d/%d/%d", player->get_full_name(), f.infantry, f.gliders, f.artillery);

                    int n = exostate.get_n_planets(player);
                    if ((n > 1 && onein(2)) || (n == 1 && onein(5))) {
                        player->prev_tactic();
                    } else {
                        player->next_tactic();
                    }
                } else {
                    player->prev_tactic();
                }
                L.debug("[%s] PROCe_tact2 END", player->get_full_name());
            }
            if (!p->get_location().in_flight() && p->get_tactic() == 9) {
                /*
                 * PROCe_tact3
                 *
                 * Travel to a nice-looking unowned planet to claim
                 */
                L.debug("[%s] PROCe_tact3 : FIND PLANET TO CLAIM", player->get_full_name());
                int set_star = -1;
                int set_planet = -1;
                for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
                    Star *s = &stars[star_idx];
                    for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                        Planet *p = s->get_planet(planet_idx);
                        if (!(p && p->exists()) || p->is_owned()) {
                            continue;
                        }
                        bool ok = true;
                        for (int i = 0; i < N_PLAYERS; ++i) {
                            Player *pl = exostate.get_player(i);
                            if (!(pl && pl->is_participating() && pl != player)) {
                                continue;
                            }
                            int ts = pl->get_location().get_target();
                            int tp = pl->get_location().get_planet_target();
                            if (ts == star_idx && tp == planet_idx) {
                                ok = false;
                                break;
                            }
                        }
                        if (ok) {
                            set_star = star_idx;
                            set_planet = planet_idx;
                            break;
                        }
                    }
                    if (set_star >= 0 && set_planet >= 0) {
                        break;
                    }
                }

                if (set_star >= 0 && set_planet >= 0) {
                    bool found = false;
                    // Orig repeats forever - we cancel eventually and use the one we already faound
                    for (int attempts = 0; attempts < 1000; ++attempts) {
                        int quality = 0;
                        for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
                            Star *s = &stars[star_idx];
                            for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                                Planet *p = s->get_planet(planet_idx);
                                if (!(p && p->exists()) || p->is_owned()) {
                                    continue;
                                }

                                int a = 0;
                                PlanetClass cls = p->get_class();
                                if (cls == Forest)  a = 4;
                                if (cls == Desert)  a = 2;
                                if (cls == Volcano) a = 1;
                                if (cls == Rock)    a = 3;
                                if (cls == Ice)     a = 2;
                                if (cls == Terra)   a = 4;
                                // Orig doesn't check artificial planets - we'll just skip these
                                if (a == 0) {
                                    continue;
                                }

                                if (a > quality && onein(7)) {
                                    bool ok = true;
                                    for (int i = 0; i < N_PLAYERS; ++i) {
                                        Player *pl = exostate.get_player(i);
                                        if (!(pl && pl->is_participating() && pl != player)) {
                                            continue;
                                        }
                                        int ts = pl->get_location().get_target();
                                        int tp = pl->get_location().get_planet_target();
                                        if (ts == star_idx && tp == planet_idx) {
                                            ok = false;
                                            break;
                                        }
                                    }
                                    if (ok) {
                                        set_star = star_idx;
                                        set_planet = planet_idx;
                                        found = true;
                                        break;
                                    } else {
                                        quality = 0;
                                    }
                                }
                            }
                            if (found) {
                                break;
                            }
                        }
                        if (found) {
                            break;
                        }
                    }

                    player->get_location().set_target(set_star, 1);
                    player->get_location().set_planet_target(set_planet);

                    if (player->get_location().in_flight()) {
                        if (player->has_invention(INV_OrbitalMassThrust) && onein(2)) {
                            player->next_tactic();
                            player->get_location().complete();
                        }
                    } else {
                        player->set_tactic(10);
                    }
                } else {
                    player->set_tactic(3);
                }
                L.debug("[%s] PROCe_tact3 END", player->get_full_name());
            }
            if (!p->get_location().in_flight() && p->get_tactic() == 3) {
                /*
                 * PROCe_tact4
                 *
                 * Select a planet to attack and travel there (weakest, or most cities, or strongest)
                 */
                L.debug("[%s] PROCe_tact4 : FIND PLANET TO ATTACK", player->get_full_name());
                int star_idx;
                int planet_idx;
                player->get_ai_attack(star_idx, planet_idx);
                bool stop = false;
                if (star_idx >= 0 && planet_idx >= 0) {
                    Planet *p = stars[star_idx].get_planet(planet_idx);
                    if (p && p->exists()) {
                        player->get_location().set_target(star_idx, 1);
                        player->get_location().set_planet_target(planet_idx);
                    }
                    player->clear_ai_attack();
                    if (p->is_owned() && p->get_owner() != player_idx) {
                        player->next_tactic();
                        if (!player->get_location().in_flight()) {
                            player->set_tactic(5);
                            stop = true;
                        }
                    }
                }
                if (!stop) {
                    int strongdef = 0;
                    int weakdef = 10000;
                    bool nodef = false;
                    int mostcities = 0;
                    int strong_star = -1;      int strong_planet = -1;
                    int weak_star = -1;        int weak_planet = -1;
                    int nodef_star = -1;       int nodef_planet = -1;
                    int mostcities_star = -1;  int mostcities_planet = -1;
                    const Freight &f = player->get_fleet().freight;
                    int army_size = f.infantry + f.gliders + 2*f.artillery;
                    bool ok = false;
                    for (int j = 0; j < 2; ++j) {
                        for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
                            Star *s = &stars[star_idx];
                            for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                                Planet *p = s->get_planet(planet_idx);
                                if (!(p && p->exists() && p->is_owned())) {
                                    continue;
                                }
                                int owner = p->get_owner();
                                if (owner == player_idx) {
                                    continue;
                                }
                                // We prioritise attacks on the player we're hostile to
                                if (j == 0 && !player->is_hostile_to(owner)) {
                                    continue;
                                }
                                int p_inf, p_gli, p_art;
                                p->get_army(p_inf, p_gli, p_art);
                                int p_army_size = p_inf + p_gli + 2*p_art;

                                /*
                                 * FIXME: Lunar bases are ignored in the planet's army size,
                                 * but it seems like they should factor in heavily!
                                 * OTOH maybe this would interfere with strong_star/strong_planet
                                 */
                                if (3*p_army_size >= 2*army_size) {
                                    continue;
                                }

                                ok = true;

                                /*
                                 * FIXME: Strange that orig doesn't like alliance combinations here!
                                 * I suspect this may not have been intentional, and may reflect
                                 * and earlier design in which multiple alliances weren't possible.
                                 */
                                AIFlag f = player->get_flag(1);
                                if (f == AI_Hi || (f == AI_Md && onein(2))) {
                                    if (exostate.has_only_alliance(player_idx, owner, ALLY_NonAttack)) {
                                        ok = false;
                                    }
                                    if (exostate.has_only_alliance(player_idx, owner, ALLY_War)) {
                                        ok = false;
                                    }
                                }

                                if (ok) {
                                    if (p_army_size < weakdef && onein(2)) {
                                        weakdef = p_army_size;
                                        weak_star = star_idx;
                                        weak_planet = planet_idx;
                                    }
                                    if (p_army_size > strongdef && onein(2)) {
                                        strongdef = p_army_size;
                                        strong_star = star_idx;
                                        strong_planet = planet_idx;
                                    }
                                    if (p_army_size == 0) {
                                        nodef = true;
                                        nodef_star = star_idx;
                                        nodef_planet = planet_idx;
                                    }
                                    int cities = p->get_n_cities();
                                    if (cities > mostcities && onein(2)) {
                                        mostcities = cities;
                                        mostcities_star = star_idx;
                                        mostcities_planet = planet_idx;
                                    }
                                }
                            }
                        }
                        if (weak_star >= 0 && weak_planet >= 0) {
                            break;
                        }
                    }

                    if (weak_star == -1 && strong_star == -1 && mostcities_star == -1) {
                        ok = false;
                    }

                    if (ok) {
                        if (weak_star >= 0 && weak_planet >= 0) {
                            L.debug("[%s] PROCe_tact4 : TARGET WEAK", player->get_full_name());
                            player->get_location().set_target(weak_star, 1);
                            player->get_location().set_planet_target(weak_planet);
                        }
                        if (nodef) {
                            L.debug("[%s] PROCe_tact4 : TARGET NODEF", player->get_full_name());
                            player->get_location().set_target(nodef_star, 1);
                            player->get_location().set_planet_target(nodef_planet);
                        }
                        if (onein(3) || weak_star == -1) {
                            L.debug("[%s] PROCe_tact4 : TARGET MOSTCITIES", player->get_full_name());
                            if (mostcities_star >= 0 && mostcities_planet >= 0) {
                                player->get_location().set_target(mostcities_star, 1);
                                player->get_location().set_planet_target(mostcities_planet);
                            }
                        }
                        if (onein(3) || weak_star == -1) {
                            L.debug("[%s] PROCe_tact4 : TARGET STRONG", player->get_full_name());
                            if (strong_star >= 0 && strong_planet >= 0) {
                                player->get_location().set_target(strong_star, 1);
                                player->get_location().set_planet_target(strong_planet);
                            }
                        }
                        player->next_tactic();
                        if (!player->get_location().in_flight()) {
                            player->set_tactic(5);
                        }
                    } else {
                        player->set_tactic(onein(5) ? 0 : 1);
                    }
                }
                L.debug("[%s] PROCe_tact4 END", player->get_full_name());
            }
            if (!p->get_location().in_flight() && p->get_tactic() == 10) {
                /*
                 * PROCe_tact5
                 *
                 * Claim target planet
                 */
                L.debug("[%s] PROCe_tact5 : CLAIM PLANET", player->get_full_name());
                int star_idx = player->get_location().get_target();
                int planet_idx = player->get_location().get_planet_target();
                Star *s = &stars[star_idx];
                Planet *p = s->get_planet(planet_idx);
                if (p && p->exists() && !p->is_owned()) {
                    if (player->attempt_spend_cpuforce(190)) {
                        p->set_owner(player_idx);
                        // FIXME: Set to 0 properly...
                        p->adjust_unrest(-99);
                        p->prepare_for_cpu_lord();
                        if (!p->is_named()) {
                            // TODO: Ensure unique name here. Orig suffixes ' 2', ' 3' etc
                            const char* name_suggest = p->get_name_suggestion();
                            p->set_name(name_suggest);
                        }
                        int cap = p->get_resource_cap();
                        int p_inf, p_gli, p_art;
                        p->get_army(p_inf, p_gli, p_art);
                        // Fill planet army from freight as much as possible without exceeding limit
                        p->adjust_army(
                            player->transfer_inf(-(cap - p_inf)),
                            player->transfer_gli(-(cap - p_gli)),
                            player->transfer_art(-(cap - p_art)));

                        // TODO: Orig writes a news item here with PROCnpmessage

                        L.debug("[%s] PROCe_tact5 : CLAIMED %s AT %s", player->get_full_name(), p->get_name(), s->name);

                        player->set_tactic(0);

                        // PROCenemytactics call
                        ai_planet_update(p);
                    } else {
                        L.warn("[%s] Prevented planet claim due to funds", player->get_full_name());
                    }
                }
                player->set_tactic(0);
                player->get_location().unset_target();
                L.debug("[%s] PROCe_tact5 END", player->get_full_name());
            }
            if (!p->get_location().in_flight() && p->get_tactic() == 5) {
                /*
                 * PROCe_tact6
                 *
                 * Launch an attack (or bail out)
                 */
                L.debug("[%s] PROCe_tact6 : CONFIRM/LAUNCH ATTACK", player->get_full_name());
                int star_idx = player->get_location().get_target();
                int planet_idx = player->get_location().get_planet_target();
                Star *s = &stars[star_idx];
                Planet *p = s->get_planet(planet_idx);

                // TODO: Things in this function from PROCenemyattack

                // Resolve to attack a planet
                if (mp_state.mpai_substage == 0) {
                    if (p && p->exists() && p->is_owned() && p->get_owner() != player_idx) {
                        // FIXME: Factor in lunar base here?
                        int p_army = p->get_army_size();
                        int army = player->get_fleet().freight.army_size();
                        if (p_army > army) {
                            // FIXME: I think this is pointless - orig sets back to 0 afterwards anyway
                            player->set_tactic(1);
                            mp_state.mpai_substage = 10;
                        } else {
                            // TODO: Orig bails if planet is aritificial and lord has artificial world
                            mp_state.mpai_substage = 1;
                        }
                    } else {
                        mp_state.mpai_substage = 10;
                    }
                }

                // Attack a planet
                if (mp_state.mpai_substage == 1) {
                    L.debug("[%s] PROCe_tact6 : ANNOUNCING ATTACK ON %s AT %s ", player->get_full_name(), p->get_name(), s->name);
                    exostate.set_active_flytarget(s);
                    exostate.set_active_planet(planet_idx);
                    int owner_idx = p->get_owner();
                    Player *owner = exostate.get_player(owner_idx);
                    if (owner->is_human()) {
                        mp_state.mpai_substage = 2;
                        return ExodusMode::MODE_Arrive;
                    } else {
                        mp_state.mpai_substage = 4;
                    }
                }

                // Attacking human - arrived, opening comms
                if (mp_state.mpai_substage == 2) {
                    mp_state.mpai_substage = 3;
                    comm_open(DIA_S_CPU_Attack);
                    return ExodusMode::MODE_None;
                }

                // Attacking human - resolving comms
                if (mp_state.mpai_substage == 3) {
                    switch (comm_action_check()) {
                        case CA_Abort:
                            comm_close();
                            mp_state.mpai_substage = 10;
                            break;
                        case CA_Attack:
                            comm_close();
                            mp_state.mpai_substage = 5;
                            break;
                        default:
                            L.fatal("Unexpected comm action on CPU attack: %d");
                    }
                }

                // Attacking CPU, requesting support from war allies
                if (mp_state.mpai_substage == 4) {
                    // TODO
                    mp_state.mpai_substage = 5;
                }

                // Launching attack
                if (mp_state.mpai_substage == 5) {
                    L.debug("[%s] PROCe_tact6 : LAUNCHING ATTACK ON %s AT %s ", player->get_full_name(), p->get_name(), s->name);
                    mp_state.mpai_substage = 6;
                    ephstate.set_ephemeral_state(EPH_LunarBattlePrep);
                    ephstate.lunar_battle.aggressor_type = AGG_Player;
                    ephstate.lunar_battle.aggressor_idx = player_idx;
                    return ephstate.get_appropriate_mode();
                }

                // Handle outcome of battle
                if (mp_state.mpai_substage == 6) {
                    // TODO: Respond to lunar battle report
                    ephstate.clear_ephemeral_state();
                    // TODO: If CPU planet + won + in a region player has explored, bulletin
                    // TODO: News item
                    // TODO: Freight adjustment of units
                    // TODO: If won, some artificial planet logic
                    mp_state.mpai_substage = 10;
                }

                // Always end with this
                if (mp_state.mpai_substage == 10) {
                    player->set_tactic(0);
                    player->get_location().unset_target();
                }
                L.debug("[%s] PROCe_tact6 END", player->get_full_name());
            }
            if (p->get_tactic() == 20) {
                /*
                 * PROCe_tact7
                 *
                 * Travel to the planet owned by us with the most resources
                 */
                L.debug("[%s] PROCe_tact7 : FLEET -> MOST RESOURCES", player->get_full_name());
                int resources = 0;
                int most_resources_star = -1;
                int most_resources_planet = -1;
                for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
                    Star *s = &stars[star_idx];
                    for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                        Planet *p = s->get_planet(planet_idx);
                        if (p && p->exists() && p->get_owner() == player_idx) {
                            if (p->get_total_reserves() > resources) {
                                resources = p->get_total_reserves();
                                most_resources_star = star_idx;
                                most_resources_planet = planet_idx;
                                L.debug("[%s] PROCe_tact7 : Targeting", player->get_full_name(), p->get_name());
                            }
                        }
                    }
                }
                if (resources > 3) {
                    player->get_location().set_target(most_resources_star, 1);
                    player->get_location().set_planet_target(most_resources_planet);
                    if (!player->get_location().in_flight()) {
                        player->next_tactic();
                    }
                } else {
                    player->set_tactic(0);
                }
                L.debug("[%s] PROCe_tact7 END", player->get_full_name());
            }
            if (!p->get_location().in_flight() && p->get_tactic() == 21) {
                /*
                 * PROCe_tact8
                 *
                 * Transfer resources from the target planet to our fleet
                 */
                L.debug("[%s] PROCe_tact8 : RESOURCES -> FLEET", player->get_full_name());
                int star_idx = player->get_location().get_target();
                int planet_idx = player->get_location().get_planet_target();
                Star *star = &stars[star_idx];
                Planet *planet = star->get_planet(planet_idx);
                if (planet && planet->exists() && planet->get_owner() == player_idx) {
                    L.debug("[%s] PROCe_tact8 : Transferring...", player->get_full_name());
                    int planet_min = planet->get_reserves_min();
                    int planet_fd = planet->get_reserves_food();
                    int planet_plu = planet->get_reserves_plu();
                    for (int i = 0; i < 3 && player->get_freight_capacity() > 0; ++i) {
                        switch (i) {
                            case 0:
                                planet->adjust_reserves(player->transfer_min(planet_min/2), 0, 0);
                                break;
                            case 1:
                                planet->adjust_reserves(0, player->transfer_fd(planet_fd/2), 0);
                                break;
                            case 2:
                                planet->adjust_reserves(0, 0, player->transfer_plu(planet_plu/2));
                                break;
                        }
                    }

                    int n = exostate.get_n_planets(player);
                    if ((n > 1 && onein(2)) || (n == 1)) {
                        player->next_tactic();
                    } else {
                        player->prev_tactic();
                    }
                } else {
                    player->prev_tactic();
                }
                L.debug("[%s] PROCe_tact8 END", player->get_full_name());
            }
            if (p->get_tactic() == 22) {
                /*
                 * PROCe_tact9
                 *
                 * Check if enemy-owned planets exist
                 */
                L.debug("[%s] PROCe_tact9 : CHECK ENEMY-OWNED PLANETS", player->get_full_name());
                bool other_owned_planets = false;
                for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
                    Star *s = &stars[star_idx];
                    for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                        Planet *p = s->get_planet(planet_idx);
                        if (p && p->exists() && p->is_owned() && p->get_owner() != player_idx) {
                            L.debug("[%s] PROCe_tact9 : YES", player->get_full_name());
                            other_owned_planets = true;
                            break;
                        }
                    }
                    if (other_owned_planets) {
                        break;
                    }
                }
                if (!other_owned_planets) {
                    L.debug("[%s] PROCe_tact9 : NO", player->get_full_name());
                    player->set_tactic(0);
                }
                L.debug("[%s] PROCe_tact9 END", player->get_full_name());
            }
            if (!p->get_location().in_flight() && p->get_tactic() == 22) {
                /*
                 * PROCe_tact10
                 *
                 * Travel to an enemy-owned planet with the intention of trading
                 */
                L.debug("[%s] PROCe_tact10 : FIND PLANET TO TRADE WITH", player->get_full_name());
                // FIXME: PROCe_tact9 proves there exists a planet to find, but this is horrible...
                int a = 0;
                while (a == 0) {
                    for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
                        Star *s = &stars[star_idx];
                        for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                            Planet *p = s->get_planet(planet_idx);
                            if (p && p->exists() && p->is_owned() && p->get_owner() != player_idx) {
                                int owner_idx = p->get_owner();
                                Player *owner = exostate.get_player(owner_idx);
                                if (exostate.has_alliance(player_idx, owner_idx, ALLY_Trade)) {
                                    a = 2;
                                }
                                if (!(player->is_hostile_to(owner_idx) || owner->is_hostile_to(player_idx))) {
                                    // Weird that orig clobbers a=2 here... was this intentional?
                                    a = 1;
                                } else {
                                    if (onein(40)) {
                                        a = 1;
                                    }
                                }
                                if (a == 1 && (!onein(10))) {
                                    a = 0;
                                }

                                if (a > 0) {
                                    player->get_location().set_target(star_idx, 1);
                                    player->get_location().set_planet_target(planet_idx);
                                L.debug("[%s] PROCe_tact10 : TARGETING %s at %s", player->get_full_name(), p->get_name(), s->name);
                                }
                            }
                        }
                    }
                }
                if (player->get_location().in_flight()) {
                    L.debug("[%s] PROCe_tact10 : Travelling", player->get_full_name());
                    if (player->has_invention(INV_OrbitalMassThrust) && onein(2)) {
                        player->next_tactic();
                        player->get_location().complete();
                    }
                } else {
                    player->next_tactic();
                }
                L.debug("[%s] PROCe_tact10 END", player->get_full_name());
            }
            if (!p->get_location().in_flight() && p->get_tactic() == 23) {
                /*
                 * PROCe_tact11
                 *
                 * Request trade with target planet
                 */
                L.debug("[%s] PROCe_tact11 : REQUEST TRADE", player->get_full_name());
                int star_idx = player->get_location().get_target();
                int planet_idx = player->get_location().get_planet_target();
                Star *star = &stars[star_idx];
                Planet *planet = star->get_planet(planet_idx);

                if (!(planet && planet->exists())) {
                    L.warn("[%s] Invalid planet in PROCe_tact11", player->get_full_name());
                    player->set_tactic(22);
                    return ExodusMode::MODE_None;
                }

                if (!(planet->is_owned())) {
                    player->set_tactic(22);
                    return ExodusMode::MODE_None;
                }

                int owner_idx = planet->get_owner();
                Player *owner = exostate.get_player(owner_idx);

                exostate.set_active_flytarget(star);
                exostate.set_active_planet(planet_idx);

                if (mp_state.mpai_substage == 0) {
                    if (owner->is_human()) {
                        mp_state.mpai_substage = 1;
                        return ExodusMode::MODE_Arrive;
                    } else {
                        mp_state.mpai_substage = 2;
                    }
                }

                if (mp_state.mpai_substage == 1) {
                    mp_state.mpai_substage = 2;
                    comm_open(DIA_S_CPU_Trade);
                    return ExodusMode::MODE_None;
                }

                if (mp_state.mpai_substage == 2) {
                    mp_state.mpai_substage = 0;
                    bool proceed = false;

                    if (owner->is_human()) {
                        switch (comm_action_check()) {
                            case CA_Trade:
                                comm_close();
                                proceed = true;
                                break;
                            case CA_Abort:
                                comm_close();
                                break;
                            default:
                                L.fatal("Unexpected comm action on CPU trade offer: %d");
                        }
                    } else {
                        proceed = true;
                    }

                    if (proceed) {
                        L.debug("[%s] PROCe_tact11 : INITIATING TRADE", player->get_full_name());
                        int a, b, c, n;
                        switch (planet->initiate_trade(player_idx)) {
                            case TRADE_Good:
                                a=4; b=1; c=3; n=20;
                                break;
                            case TRADE_Fair:
                                a=3; b=1; c=2; n=10;
                                break;
                            case TRADE_Bad:
                                a=3; b=1; c=1; n=30;
                                break;
                            default:
                                L.warn("[%s] Invalid trade in PROCe_tact11", player->get_full_name());
                                player->set_tactic(22);
                                return ExodusMode::MODE_None;
                        }

                        if (!owner->is_human()) {
                            if (!exostate.has_alliance(player_idx, owner_idx, ALLY_Trade)) {
                                if (!player->attempt_spend_cpuforce(n)) {
                                    L.warn("[%s] Preventing trade that was not affordable",
                                            player->get_full_name());
                                    player->set_tactic(22);
                                    return ExodusMode::MODE_None;
                                }
                            }
                        }

                        const Freight &f = player->get_fleet().freight;
                        int sold_food = f.food;
                        player->give_mc(a*player->transfer_min(-f.minerals));
                        player->give_mc(b*player->transfer_fd(-f.food));
                        player->give_mc(c*player->transfer_plu(-f.plutonium));
                        player->set_tactic(0);

                        // PROClordbuy
                        if (!tradebuy_start(sold_food, rand()%6, rand()%5, rand()%4)) {
                            /*
                             * If tradebuy_start returns false, this is
                             * a human-owned planet and we should enter into a
                             * stage where we can call tradebuy_update() until
                             * it returns true.
                             */
                            mp_state.mpai_substage = 3;
                            return ExodusMode::MODE_None;
                        }

                    } else {
                        player->set_tactic(22);
                    }
                }

                /*
                 * We drop into this substage when a human tradebuy interaction is resolving.
                 * We remain here until tradebuy_update() returns true.
                 */
                if (mp_state.mpai_substage == 3) {
                    if (tradebuy_update()) {
                        mp_state.mpai_substage = 0;
                    } else {
                        return ExodusMode::MODE_None;
                    }
                }

                L.debug("[%s] PROCe_tact11 END", player->get_full_name());
            }
            if (p->get_tactic() == 0) {
                /*
                 * PROCe_tact12
                 *
                 * Travel to the planet owned by us with the largest army
                 */
                L.debug("[%s] PROCe_tact12 : FLEET -> LARGE ARMY", player->get_full_name());
                int quality = 0;
                for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
                    Star *s = &stars[star_idx];
                    for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                        Planet *p = s->get_planet(planet_idx);
                        if (p && p->exists() && p->get_owner() == player_idx) {
                            int army = p->get_army_size();
                            if (army > quality) {
                                quality = army;
                                player->get_location().set_target(star_idx, 1);
                                player->get_location().set_planet_target(planet_idx);
                                L.debug("[%s] PROCe_tact12 : TARGETING %s AT %s", player->get_full_name(), p->get_name(), s->name);
                            }
                        }
                    }
                }
                L.debug("[%s] PROCe_tact12 END", player->get_full_name());
            }
        }
        next_mpai_stage();
    }

    return ExodusMode::MODE_None;
}

ExodusMode GalaxyMap::month_pass_planet_update() {
    Star *s = (Star*)exostate.get_active_flytarget();
    Planet *p  = exostate.get_active_planet();
    Player *owner = nullptr;

    // At the start of processing, planets will always be owned.
    // However, if they become disowned during processing, we
    // still return to proceed to subsequent stages.
    // This can only happen after the comm station collapses -
    // although the owner can change after a rebellion.
    if (p->is_owned()) {
        exostate.set_active_player(p->get_owner());
        owner = exostate.get_player(p->get_owner());
    }

    if (mp_state.mpp_stage == MPP_ShuffleTrade) {
        if (onein(7)) p->randomise_trade_quality();
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_FirstCity) {
        if (owner && !exostate.first_city_done) {
            if (owner->is_human() && p->count_stones(STONE_City) > 0) {
                audio_manager.target_music(mpart2mus(10));
                exostate.first_city_done = true;
                bulletin_start_new(false);
                bulletin_set_flag(IMG_TS1_FLAG16);
                bulletin_set_text_col(COL_TEXT3);
                bulletin_set_next_text("%s HAS BUILT THE FIRST CITY", tmp_caps(owner->get_name()));
                bulletin_set_next_text("");
                bulletin_set_next_text("Words cannot describe what has been");
                bulletin_set_next_text("achieved by building the first city for the");
                bulletin_set_next_text("homeless colonists.");
                bulletin_set_next_text("");
                bulletin_set_next_text("This might be the beginning of a new era.");
                bulletin_set_next_text("");
                bulletin_set_next_text("But beware. A city can also be a centre");
                bulletin_set_next_text("for system enemies.");
                bulletin_set_next_text("");
                bulletin_set_next_text("A picture of the new city follows.");
                do_first_city = true;
                next_mpp_stage();
                return ExodusMode::MODE_None;
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_FirstSpaceport) {
        if (!exostate.first_spaceport_done) {
            if (owner->is_human()
                && p->count_stones(STONE_Port0) > 0
                && p->count_stones(STONE_Port1) > 0
                && p->count_stones(STONE_Port2) > 0) {
                audio_manager.target_music(mpart2mus(10));
                exostate.first_spaceport_done = true;
                do_first_spaceport = true;
                next_mpp_stage();
                return ExodusMode::MODE_None;
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_ResearchCheck) {
        ephstate.research.done = false;
        if (exostate.get_orig_month() > 2 && p->has_stone(STONE_City)) {
            int threshold = 1;
            int q = (int)owner->get_officer(OFF_Science);
            // This means threshold can be -ve - I wonder if the -2 here
            // mistakenly assumed that the quality started from 1...?
            threshold += 2*q - 2;
            threshold += owner->get_tax() / 20;
            if (RND(40) <= threshold) {
                if (update_researchcheck(s, p)) {
                    return ExodusMode::MODE_None;
                }
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_Research) {
        if (update_research(p)) {
            return ExodusMode::MODE_None;
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_Meteors) {
        if (onein(200)) {
            exostate.register_news(NI_Meteor);
            bulletin_start_new(false);
            bulletin_set_bg(IMG_ME5_MENU);
            bulletin_set_active_player_flag();
            bulletin_write_planet_info(s, p);
            bulletin_set_next_text("METEOR STRIKES %s", tmp_caps(p->get_name()));
            bulletin_set_next_text("");
            if (owner->has_invention(INV_RadarExtension)) {
                bulletin_set_next_text("The Radar Extension System made");
                bulletin_set_next_text("an early destroying of the dangerous");
                bulletin_set_next_text("fragments possible.");
            } else {
                bulletin_set_next_text("A meteor has reached the planet %s", p->get_name());
                bulletin_set_next_text("and several fragments of it have hit");
                bulletin_set_next_text("the world's surface.");
                bulletin_set_next_text("");

                ephstate.set_ephemeral_state(EPH_Destruction);
                ephstate.destruction.type = DESTROY_NRandom;
                ephstate.destruction.tgt_stones.reset();
                ephstate.destruction.n_strikes = RND(7) + 1; // 2-8 hits
                ephstate.destruction.enable_explosions = true;
                ephstate.destruction.enable_explosion_anims = true;
                ephstate.destruction.irradiated = false;
                ephstate.destruction.show_target = false;
                ephstate.destruction.destroyer_idx = -1;
                ephstate.destruction.terror = false;
                ephstate.destruction.nuke = true;
                ephstate.destruction.draw = exostate.get_active_player()->is_human();
                do_meteor = true;

                L.debug("METEOR: %d impacts", ephstate.destruction.n_strikes);
                if (owner->is_human()) {
                    bulletin_set_next_text("Visual replay follows.");
                } else {
                    bulletin_set_next_text("The planet has taken %d hits.",
                                           ephstate.destruction.n_strikes);
                }
            }
            next_mpp_stage();
            return ExodusMode::MODE_None;
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_KillArmies) {
        // We kill at least 1 of each thing, so if has_army() returns
        // true, we know that *something* is going to die. >:D
        if (onein(90)) {
            if (p->has_army()) {
                int kill_inf = RND(10);
                int kill_gli = RND(10);
                int kill_art = RND(10);
                p->adjust_army(-1*kill_inf, -1*kill_gli, -1*kill_art);
                bulletin_start_new(true);
                bulletin_set_bg(p->sprites()->bulletin_bg);
                bulletin_set_active_player_flag();
                bulletin_write_planet_info(s, p);
                bulletin_set_next_text("PRIORITY ALPHA MESSAGE");
                bulletin_set_next_text("");
                bulletin_set_next_text("Defects at %s have caused", p->get_name());
                bulletin_set_next_text("some battle machines to be destructed.");
                // TODO: Should we tell the player how many have been destroyed?
                exostate.register_news(NI_BattleMachinesDestructed);
                next_mpp_stage();
                return ExodusMode::MODE_None;
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_ClimateChange) {
        if (onein(250)) {
            exostate.register_news(NI_GeologicalChange);
            const char* before = p->get_class_str_lower();
            p->surfchange();
            const char* after = p->get_class_str_lower();
            bulletin_start_new(true);
            bulletin_set_bg(p->sprites()->bulletin_bg);
            bulletin_set_active_player_flag();
            bulletin_write_planet_info(s, p);
            // No special music here (PROCsurfchange)
            bulletin_set_next_text("GEOLOGICAL CHANGE AT %s", tmp_caps(p->get_name()));
            bulletin_set_next_text("");
            bulletin_set_next_text("Due to a global climate effect,");
            bulletin_set_next_text("the planet's surface has changed from");
            bulletin_set_next_text("%s into %s.", before, after);
            next_mpp_stage();
            return ExodusMode::MODE_None;
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_Epidemic) {
        // PROCepidemic(1) case
        // PROCepidemic(2) case (plants poisoned) is handled as MT_TerrorAgri
        if (onein(150)) {
            if (exostate.get_orig_month() >= 10 && !owner->has_invention(INV_Acid)) {
                int to_die = RND(15);
                if (p->count_stones(STONE_Agri) < 15) to_die = RND(5);
                int sz = p->get_size_blocks();
                for (int j = 0; j < sz; ++j) {
                    for (int i = 0; i < sz; ++i) {
                        if (p->get_stone(i, j) == STONE_Agri) {
                            p->set_stone(i, j, STONE_AgriDead);
                            --to_die;
                            if (to_die <= 0) break;
                        }
                    }
                    if (to_die <= 0) break;
                }
                p->adjust_unrest(2);
                audio_manager.target_music(mpart2mus(9));
                exostate.register_news(NI_Plague);
                bulletin_start_new(true);
                bulletin_set_bg(p->sprites()->bulletin_bg);
                bulletin_set_active_player_flag();
                bulletin_write_planet_info(s, p);
                bulletin_set_next_text("PLAGUE AT %s", tmp_caps(p->get_name()));
                bulletin_set_next_text("");
                bulletin_set_next_text("Large parts of cultivated area");
                bulletin_set_next_text("have collapsed.");
                next_mpp_stage();
                return ExodusMode::MODE_None;
            }
        }
        next_mpp_stage();
    }

    // Doesn't cause planet loss - only damage
    if (mp_state.mpp_stage == MPP_AlienAttack) {
        if (onein(200) && exostate.get_orig_month() >= 10) {
            audio_manager.target_music(mpart2mus(8));
            exostate.register_news(NI_AlienAttack);
            bulletin_start_new(false);
            bulletin_set_bg(p->sprites()->bulletin_bg);
            bulletin_set_active_player_flag();
            bulletin_write_planet_info(s, p);
            bulletin_set_next_text("ALIEN PIRATES AT %s", tmp_caps(p->get_name()));
            bulletin_set_next_text("");
            bulletin_set_next_text("Alien space pirates are about to");
            bulletin_set_next_text("storm %s's moon and the planet.", p->get_name());

            ephstate.set_ephemeral_state(EPH_LunarBattlePrep);
            ephstate.lunar_battle.aggressor_type = AGG_Aliens;
            do_lunar_battle = true;

            next_mpp_stage();
            return ExodusMode::MODE_None;
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_AlienAttackResult) {
        if (ephstate.get_ephemeral_state() == EPH_LunarBattleReport) {
            LunarBattleReport &rpt = ephstate.lunar_battle_report;
            ephstate.clear_ephemeral_state();
            bulletin_start_new(true);
            bulletin_set_bg(p->sprites()->bulletin_bg);
            bulletin_set_active_player_flag();
            bulletin_write_planet_info(s, p);
            if (rpt.aggressor_won) {
                bulletin_set_next_text("The pirates have succeeded.");
                bulletin_set_next_text("Nobody can prevent them now");
                bulletin_set_next_text("from plundering the planet.");
                p->plunder();
            } else {
                ephstate.set_ephemeral_state(EPH_AlienResearch);
                // TODO: This should use full name with SG title
                bulletin_set_next_text("%s could keep the planet.", owner->get_full_name());
                bulletin_set_next_text("The pirates did not succeed.");
            }
            next_mpp_stage();
            return ExodusMode::MODE_None;
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_AlienResearchCheck) {
        ephstate.research.done = false;
        if (ephstate.get_ephemeral_state() == EPH_AlienResearch) {
            ephstate.clear_ephemeral_state();
            if (update_researchcheck(s, p)) {
                return ExodusMode::MODE_None;
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_AlienResearch) {
        if (update_research(p)) {
            return ExodusMode::MODE_None;
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_DiscoverSpecies) {
        // TODO
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_CityEpidemic) {
        if (onein(150)) {
            if (!owner->has_invention(INV_MultiFunctionalVaccine)) {
                int cities = p->count_stones(STONE_City);
                if (cities > 0) {
                    if (exostate.get_orig_month() >= 10) {
                        int closedown = min(cities, RND(4));
                        for (int i = closedown; i > 0; --i) {
                            int x, y;
                            p->find_random_stone(STONE_City, x, y);
                            p->set_stone(x, y, STONE_Rubble);
                        }
                        audio_manager.target_music(mpart2mus(8));
                        exostate.register_news(NI_Epidemic);
                        bulletin_start_new(true);
                        bulletin_set_bg(p->sprites()->bulletin_bg);
                        bulletin_set_active_player_flag();
                        bulletin_write_planet_info(s, p);
                        bulletin_set_next_text("EPIDEMIC AT %s", tmp_caps(p->get_name()));
                        bulletin_set_next_text("");
                        const char* a = closedown == 1 ? "city" : "cities";
                        const char* b = closedown == 1 ? "has" : "have";
                        bulletin_set_next_text("%d %s %s been haunted by an", closedown, a, b);
                        bulletin_set_next_text("unknown epidemic. The %s closed", a);
                        bulletin_set_next_text("down.");
                        next_mpp_stage();
                        return ExodusMode::MODE_None;
                    }
                }
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_RebelAttack) {
        int cities = p->get_n_cities();
        if (cities > 0 && exostate.get_orig_month() > 2) {
            if (p->get_class() != Artificial && p->get_unrest() > 9) {
                int robots = p->get_robots();
                if ((robots < cities*3) || onein(4)) {
                    p->adjust_unrest(-3);
                    audio_manager.target_music(mpart2mus(8));
                    bulletin_start_new(false);
                    bulletin_set_bg(p->sprites()->bulletin_bg);
                    bulletin_set_active_player_flag();
                    bulletin_write_planet_info(s, p);
                    bulletin_set_next_text("REVOLUTION AT %s", tmp_caps(p->get_name()));
                    bulletin_set_next_text("");
                    bulletin_set_next_text("The population of %s has sent", p->get_name());
                    bulletin_set_next_text("rebel warriors into a lunar battle");
                    bulletin_set_next_text("in order to depose %s.", owner->get_full_name());

                    ephstate.set_ephemeral_state(EPH_LunarBattlePrep);
                    ephstate.lunar_battle.aggressor_type = AGG_Rebels;
                    do_lunar_battle = true;

                    next_mpp_stage();
                    return ExodusMode::MODE_None;
                }
            }
        }
        next_mpp_stage();
    }

    // Can cause the owner to change - we should return to ensure
    // that the 'owner' variable is updated when we resume processing.
    if (mp_state.mpp_stage == MPP_RebelAttackResult) {
        if (ephstate.get_ephemeral_state() == EPH_LunarBattleReport) {
            LunarBattleReport &rpt = ephstate.lunar_battle_report;
            ephstate.clear_ephemeral_state();
            bulletin_start_new(true);
            bulletin_set_bg(p->sprites()->bulletin_bg);
            bulletin_set_active_player_flag();
            bulletin_write_planet_info(s, p);

            Player *new_owner = nullptr;
            int new_owner_idx = -1;
            int attempts = 0;
            while (attempts < 500 && !new_owner) {
                attempts++;
                int idx = rand() % N_PLAYERS;
                Player *pl = exostate.get_player(idx);
                if (!pl)
                    continue;
                if (pl == owner)
                    continue;
                if (!pl->is_participating())
                    continue;
                // Orig enforces has_visited (SunE), but will get stuck if no candidates
                int star_idx = exostate.get_active_star_idx();
                if (attempts < 250 && !pl->get_location().has_visited(star_idx))
                    continue;
                new_owner_idx = idx;
                new_owner = pl;
                break;
            }

            bulletin_set_next_text("The revolution of %s", p->get_name());
            bulletin_set_next_text("");
            if (rpt.aggressor_won) {
                exostate.register_news(NI_SuccessfulRevolution);
                bulletin_set_next_text("The rebels have succeeded. So the");
                bulletin_set_next_text("planet needs a new leader. The people");
                if (false) {
                    const char *new_owner_name = new_owner->get_full_name();
                    bulletin_set_next_text("have chosen %s to rule it!", new_owner_name);
                    p->set_owner(new_owner_idx);
                } else {
                    // Orig doesn't do this, but we can't find a new owner!
                    bulletin_set_next_text("have chosen to establish a republic!");
                    p->disown();
                    // If there's no owner, jump ahead to after MPP_LosePlanetControl
                    // These are the steps that could previously happen with no owner
                    mp_state.mpp_stage = MPP_LosePlanetControl;
                    next_mpp_stage();
                    return ExodusMode::MODE_None;
                }
            } else {
                exostate.register_news(NI_Revolution);
                bulletin_set_next_text("The rebels did not succeed. The planet");
                bulletin_set_next_text("remains in %s's hand.", owner->get_full_name());
                if (!rpt.rebel_peace && onein(2)) {
                    bulletin_set_next_text("");
                    bulletin_set_next_text("Rebels have attacked the depots!");
                    if (owner->has_invention(INV_IndustryGuard)) {
                        bulletin_set_next_text("The Industry Guard Computers prevented");
                        bulletin_set_next_text("any damage.");
                    } else {
                        bulletin_set_next_text("Food, minerals or plutonium might have");
                        bulletin_set_next_text("been stolen.");
                        int mn = p->get_reserves_min();
                        int fd = p->get_reserves_food();
                        int pl = p->get_reserves_plu();
                        int mn_lost = mn <= 0 ? 0 : RND(mn);
                        int fd_lost = fd <= 0 ? 0 : RND(fd);
                        int pl_lost = pl <= 0 ? 0 : RND(pl);
                        p->adjust_reserves(-mn_lost, -fd_lost, -pl_lost);
                        p->adjust_unrest(1);
                    }
                }
            }
            next_mpp_stage();
            return ExodusMode::MODE_None;
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_LawsIncreaseUnrest) {
        if (p->laws_cause_unrest()) p->adjust_unrest(1);
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_ParksReduceUnrest) {
        if (onein(2)) {
            int parks = p->count_stones(STONE_Park);
            int cities = p->count_stones(STONE_City);
            if (parks > cities/3) p->adjust_unrest(-1);
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_ClearRadiation) {
        p->clear_radiation();
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_ReactorMeltdown) {
        if (onein(200)) {
            if (p->has_stone(STONE_Plu)) {
                audio_manager.target_music(mpart2mus(8));
                bulletin_start_new(false);
                bulletin_set_bg(p->sprites()->bulletin_bg);
                bulletin_set_active_player_flag();
                bulletin_write_planet_info(s, p);
                bulletin_set_next_text("NUCLEAR MELTDOWN AT %s", tmp_caps(p->get_name()));
                bulletin_set_next_text("");
                bulletin_set_next_text("");
                bulletin_set_next_text("A badly protected plutonium reactor");
                bulletin_set_next_text("has contaminated %s.", p->get_name());
                ephstate.set_ephemeral_state(EPH_Destruction);
                ephstate.destruction.type = DESTROY_NStones;
                ephstate.destruction.tgt_stones.reset();
                ephstate.destruction.tgt_stones.add(STONE_Plu);
                ephstate.destruction.n_strikes = 1;
                ephstate.destruction.enable_explosions = true;
                ephstate.destruction.enable_explosion_anims = true;
                ephstate.destruction.irradiated = true;
                ephstate.destruction.show_target = true;
                ephstate.destruction.destroyer_idx = -1;
                ephstate.destruction.terror = false;
                ephstate.destruction.nuke = true;
                ephstate.destruction.draw = exostate.get_active_player()->is_human();
                do_meltdown = true;
                next_mpp_stage();
                return ExodusMode::MODE_None;
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_ClimateChangeDueToCultivation) {
        if (p->get_class() == Desert || p->get_class() == Rock) {
            if (!p->surfchange_happened_this_month()) {
                int threshold = 86;
                if (p->get_size() == PLANET_Medium) threshold = 117;
                if (p->get_size() == PLANET_Large)  threshold = 153;
                if (p->count_stones(STONE_Agri) >= threshold) {
                    p->change_class(Terra);
                    exostate.register_news(NI_SurfChangeCultivation);
                    bulletin_start_new(true);
                    bulletin_set_bg(p->sprites()->bulletin_bg);
                    bulletin_set_active_player_flag();
                    bulletin_write_planet_info(s, p);
                    audio_manager.target_music(mpart2mus(5));
                    bulletin_set_next_text("SURFACE CHANGE due to cultivation");
                    bulletin_set_next_text("");
                    bulletin_set_next_text("Due to global cultivation,");
                    bulletin_set_next_text("the planet's surface has changed from");
                    bulletin_set_next_text("bleak rubble into green plains.");
                    next_mpp_stage();
                    return ExodusMode::MODE_None;
                }
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_ClimateChangeDueToDeforestation) {
        if (p->get_class() == Forest) {
            if (!p->surfchange_happened_this_month()) {
                int threshold = 30;
                if (p->get_size() == PLANET_Medium) threshold = 40;
                if (p->get_size() == PLANET_Large)  threshold = 51;
                if (p->count_stones(STONE_NaturalLarge) < threshold) {
                    p->change_class(Rock);
                    exostate.register_news(NI_SurfChangeClearing);
                    bulletin_start_new(true);
                    bulletin_set_bg(p->sprites()->bulletin_bg);
                    bulletin_set_active_player_flag();
                    bulletin_write_planet_info(s, p);
                    audio_manager.target_music(mpart2mus(9));
                    bulletin_set_next_text("SURFACE CHANGE due to clearing");
                    bulletin_set_next_text("");
                    bulletin_set_next_text("Too much forest has been cleared.");
                    bulletin_set_next_text("The planet's surface has changed from");
                    bulletin_set_next_text("green plains into bleak rubble.");
                    return ExodusMode::MODE_None;
                }
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_LosePlanetControl) {
        if (p->count_stones(STONE_Base) == 0) {
            audio_manager.target_music(mpart2mus(9));
            // FIXME: No PROCdonotice here? (Orig doesn't...)
            bulletin_start_new(true);
            bulletin_set_bg(p->sprites()->bulletin_bg);
            bulletin_set_active_player_flag();
            bulletin_write_planet_info(s, p);
            bulletin_set_next_text("COMM. STATION HAS BEEN DESTROYED.");
            bulletin_set_next_text("");
            if (owner->is_human()) {
                bulletin_set_next_text("You have lost all access to the planet.");
            } else {
                bulletin_set_next_text("%s has lost this planet.", owner->get_full_name());
            }
            p->disown();
            next_mpp_stage();
            return ExodusMode::MODE_None;
        }
        next_mpp_stage();
    }

    // 'owner' can be null after this point
    // TODO: Check whether each subsequent stage should happen with no owner

    if (mp_state.mpp_stage == MPP_Income) {
        // TODO - Check the weird case in PROCcal_plan based on the t% DIM
        if (owner) {
            int income = p->get_net_income();
            // If income is 1, we allow it to remain at 1 if taxes are <50%
            if (!(income == 1 && owner->get_tax() < 50)) {
                income = (income * (100 - owner->get_tax())) / 100;
            }
            owner->give_mc(income);
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_ProducePlutonium) {
        p->produce_plutonium();
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_ProduceMilitary) {
        ProductionReport rpt = p->produce_military();
        if (rpt.inf + rpt.gli + rpt.art > 0) {
            report.add_line("Produced battle units: %d / %d / %d",
                            rpt.inf, rpt.gli, rpt.art);
        }
        if (rpt.not_produced > 0) {
            const char *sep = " / ";
            const char *empty = "";
            const char *s0 = (rpt.no_money && (rpt.no_space || rpt.no_plu)) ? sep : empty;
            const char *s1 = (rpt.no_space && rpt.no_plu) ? sep : empty;
            if (rpt.not_produced == 1) {
                report.add_line("1 battle unit has not been produced.");
            } else {
                report.add_line("%d battle units have not been produced.",
                                rpt.not_produced);
            }
            report.add_line("%s%s%s%s%s", rpt.no_money ? "No money" : "", s0,
                                          rpt.no_space ? "No space" : "", s1,
                                          rpt.no_plu ? "No plutonium" : "");
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_Mining) {
        p->mine();
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_ResourceCap1) {
        p->discard_excess_resources();
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_MilitaryFacilityShutdown) {
        // TODO
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_Trade) {
        if (owner) {
            if (p->count_stones(STONE_Trade)) {
                TradeReport rpt = p->monthly_trade();
                report.add_line("Trading Centre sold:");
                report.add_line("Mi: %d / Fd: %d / Pl: %d => %d MC",
                                 rpt.mi,  rpt.fd,  rpt.pl, rpt.mc);
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_VillageGifts) {
        int n_villages = p->count_stones(STONE_Village);
        if (owner && n_villages > 2) {
            int mc = RND(n_villages);
            // FIXME: Should we delay the giving of MC until the bulletin?
            owner->give_mc(mc);
            report.add_line("The native village inhabitants offer");
            report.add_line("presents that are worth %d MC.", mc);
            exostate.register_news(NI_NativesOfferPresents);
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_FoodPerishes) {
        // SUGGEST: Should we include perished food in the report?
        p->perish_food();
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_FoodProduction) {
        p->produce_food();
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_AgriCollapse) {
        if (p->agri_collapse()) {
            report.add_line("Some cultivated area has collapsed.");
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_CityExpansion) {
        for (int i = 0; i < p->count_stones(STONE_City); ++i) {
            if (onein(20)) {
                if (p->expand_city()) {
                    report.add_line("A city has expanded.");
                    if (owner && owner->is_human()) {
                        exostate.register_news(NI_CityExpanded);
                    }
                }
                break;
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_VillageExpansion) {
        for (int i = 0; i < p->count_stones(STONE_Village); ++i) {
            if (onein(18)) {
                // N.B. Orig doesn't report in bulletin or news item.
                // Perhaps this reflects the independence of the village.
                p->expand_village();
                break;
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_ConsumeFood) {
        int food_needed = p->consume_food();
        if (food_needed > 0) {
            report.add_line("There is not enough food for all systems.");
            if (owner && owner->is_human()) {
                exostate.register_news(NI_NotEnoughFood);
            }
            p->adjust_unrest(3);
            int cities_closed = 0;
            bool bases_collapsed = false;
            while (!p->agri_sufficient()) {
                int x, y;
                if (p->find_random_stone(STONE_City, x, y)) {
                    p->set_stone(x, y, STONE_Rubble);
                    cities_closed++;
                    continue;
                }
                if (p->find_random_stone(STONE_Base, x, y)) {
                    p->set_stone(x, y, STONE_AgriDead);
                    continue;
                }
                bases_collapsed = true;
                break;
            }
            if (cities_closed > 0) {
                report.add_line("%d %s closed down.",
                                cities_closed,
                                cities_closed == 1 ? "city has" : "cities have");
            }
            if (bases_collapsed) {
                exostate.register_news(NI_ControlLost);
                report.add_line("COMMAND STATION HAS COLLAPSED");
                if (owner) {
                    if (owner->is_human()) {
                        report.add_line("You have lost all access to the planet.");
                    } else {
                        report.add_line("%s has lost this planet.", owner->get_full_name());
                    }
                }
                p->disown();
            }

        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_ResourceCap2) {
        p->discard_excess_resources();
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_AdvanceUnrest) {
        p->update_unrest_history();
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_AmendMilitaryFunding) {
        p->validate_army_funding();
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_DisplayPlanetReport) {
        if (owner && owner->is_human()) {
            // Skip any reports which don't have content
            if (report.items > 0) {
                bulletin_start_new(true);
                bulletin_set_bg(p->sprites()->bulletin_bg);
                bulletin_set_active_player_flag();
                bulletin_write_planet_info(s, p);
                for (int i = 0; i < report.items; ++i) {
                    bulletin_set_next_text(report.content[i]);
                }
                bulletin_set_next_text("Report ends.");
                next_mpp_stage();
                return ExodusMode::MODE_None;
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_EnsureComplete) {
        // Bit of a hack, but ensures that when we return to display
        // the final planet report, we don't try to exit the planet
        // pass mode until acknowledgement, which seems cleaner.
        next_mpp_stage();
    }

    return ExodusMode::MODE_None;
}

bool GalaxyMap::update_researchcheck(Star* s, Planet *p) {
    if (!p->is_owned())
        return false;

    Player *owner = exostate.get_player(p->get_owner());
    ephstate.research.done = false;
    if (owner->is_human()) {
        // If we're a human player, we need to exit to ask if
        // they wish to pay for the invension (PROCscimoney)
        ephstate.research.cost = max(10 * (RND(owner->get_mc()) / 50), 20);
        if (owner->can_afford(ephstate.research.cost)) {
            ephstate.set_ephemeral_state(EPH_ResearchCheck);
            bulletin_start_new(true);
            bulletin_set_bg(p->sprites()->bulletin_bg);
            bulletin_set_active_player_flag();
            bulletin_write_planet_info(s, p);
            bulletin_set_next_text("Request from scientists");
            bulletin_set_next_text("");
            bulletin_set_next_text("The scientists of %s demand", p->get_name());
            bulletin_set_next_text("%d MC from you to support their latest",
                                   ephstate.research.cost);
            bulletin_set_next_text("project.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Do you wish to pay?");
            bulletin_set_yesno();
            next_mpp_stage();
            return true;
        }
    } else {
        // CPU players will always get the research free of charge
        ephstate.research.cost = 0;
        ephstate.research.done = true;
    }
    return false;
}

bool GalaxyMap::update_research(Planet *p) {
    if (!p->is_owned())
        return false;

    Player *owner = exostate.get_player(p->get_owner());
    if (ephstate.research.done) {
        ephstate.research.done = false;
        if (!owner->attempt_spend(ephstate.research.cost)) {
            L.fatal("Option was given to spend more on research than player's MC");
        }
        Invention inv = owner->get_random_researchable_invention();
        L.debug("%s: RESEARCH %d (%dMC)",
                owner->get_full_name(),
                inv,
                ephstate.research.cost);
        if (inv != INV_MAX) {
            if (!owner->research_invention(inv)) {
                L.fatal("Unable to research invention identified as researchable");
            }
            int mc_reward = RND(10) * 10;
            if (owner->is_human()) {
                bulletin_start_new(false);
                bulletin_set_bg(IMG_ME7_MENU);
                bulletin_set_active_player_flag();
                bulletin_set_text_col(COL_TEXT2);
                bulletin_set_next_text("New invention: %s", owner->get_invention_str(inv));
                bulletin_set_text_col(COL_TEXT_SPEECH);
                bulletin_set_next_text("(%s Research)", owner->get_invention_type_str(inv));
                // TODO: Descriptions
                // TODO: "This makes possible:"
                if (inv > INV_UniversalVaccine) {
                    // TODO: Show awarded credits
                    owner->give_mc(mc_reward);
                }
                next_mpp_stage();
                return true;
            } else {
                // CPU players get MC for all inventions - not just the later ones
                owner->give_mc(mc_reward);
            }
        } else {
            // TODO: New species discovery if all inventions researched
        }
    }

    return false;
}

void GalaxyMap::ai_planet_update(Planet* p) {
    // Orig: PROCenemytactics
    if (!(p && p->exists() && p->is_owned())) {
        return;
    }

    Player* owner = exostate.get_player(p->get_owner());

    if (owner->is_human()) {
        return;
    }

    if (!owner->is_participating()) {
        return;
    }

    if (p->get_unrest() == 0) {
        p->collect_taxes();
    }

    bool hunger = (exostate.get_orig_month() > 2) && !(p->agri_sufficient());

    // If we don't have much money and we're not desperate then do nothing
    if (owner->get_mc() < 30 && !hunger) {
        return;
    }

    int n_planets = exostate.get_n_planets(owner);

    if (   n_planets > 2
        && (RND(n_planets - 1) != 1)
        && owner->get_mc() < 200
        && !hunger) {
           return;
    }

    if (   owner->get_tactic() >= 7
        && (p->get_class() != Artificial)
        && !hunger) {
            return;
    }

    p->ai_update();
}

void GalaxyMap::reset_planet_report() {
    report.items = 0;
}
