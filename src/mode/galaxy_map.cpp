#include "galaxy_map.h"

#include "assetpaths.h"

#include "exodus_features.h"

#include "save/save.h"

#include "util/iter.h"
#include "util/str.h"
#include "util/value.h"

#define BLINK_TIME 0.5

#ifdef DBG
extern ExodusDebug exodebug;
#endif

extern SAVEMANAGER save_manager;

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

    mission_state = MS_None;
    mission_n_strikes = 0;
}

void GalaxyMap::enter() {
    ModeBase::enter(ID::END);

    DrawTarget tgt = TGT_Primary;
    if (ephstate.galaxymap_pixelswap) {
        tgt = TGT_Secondary;
        draw_galaxy(true);
        ephstate.galaxymap_pixelswap = false;
        draw_manager.pixelswap_start();
    } else {
        draw_galaxy(false);
    }

    draw_panel_bg(tgt);

    if (tgt != TGT_Primary) {
        draw_manager.pixelswap_start();
    }

    artificial_planet_to_move = nullptr;

    if (ephstate.get_ephemeral_state() == EPH_ResumeFly) {
        ephstate.clear_ephemeral_state();
        stage = GM_Fly;
    } else if (ephstate.get_ephemeral_state() == EPH_SelectPlanet) {
        stage = GM_SelectStar;
    } else if (ephstate.get_ephemeral_state() == EPH_MovePlanet) {
        ephstate.clear_ephemeral_state();
        artificial_planet_to_move = exostate.get_active_planet();
        stage = GM_ArtificialWorldStarSelect;
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
            if (player->get_race() == RACE_Human && !player->intro_seen()) {
                exostate.set_active_flytarget(gal->get_guild());
                return ExodusMode::MODE_GalaxyIntro;
            }

            /*
             * Unfortunately, we need to redraw markers whilst idling.
             * This is because text can't be repaired.
             */
            draw_markers(false, true);

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

            {
                bool month_pass = input_manager.consume(K_Space);

#ifdef DBG
                month_pass = month_pass || input_manager.read(K_B);
#endif

                if (month_pass) {
                    stage = GM_MonthPassing;
                }
            }

            if (input_manager.consume(K_D)) {
                return ExodusMode::MODE_Distances;
            }

            if (input_manager.consume(K_E)) {
                return ExodusMode::MODE_ShipEquip;
            }

            if (input_manager.consume(K_M)) {
                stage = GM_Menu;
                menu_open(MM_StarMarker);
                return ExodusMode::MODE_None;
            }

            // Orig is 'R' for 'Recall' but 'N' seems more intuitive
            if (input_manager.consume(K_R) || input_manager.consume(K_N)) {
                return ExodusMode::MODE_News;
            }

            if (input_manager.consume(K_S)) {
                if (QUICKSAVE_SLOT < 0) {
                    comm_open(DIA_S_NoFirstSave);
                    stage = GM_Counsellor;
                    return ExodusMode::MODE_None;
                } else {
                    save_manager.save(QUICKSAVE_SLOT);
                }
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
                if (input_manager.consume(K_Escape) || draw_manager.clicked_r()) {
                    ephstate.clear_ephemeral_state();
                    return ExodusMode::MODE_Reload;
                }

                panel_set_text("SELECT A STAR");

                FlyTarget* mouseover_ft = get_mouseover_flytarget();
                if (mouseover_ft) {
                    update_panel_info_ft(TGT_Primary, exostate.get_player(0), mouseover_ft);
                }

                // SUGGEST: Some way to highlight available stars
                ft = get_clicked_flytarget();
                if (ft && ft != exostate.get_galaxy()->get_guild()) {
                    int ft_idx = exostate.tgt2loc(ft);
                    if (player->get_location().has_visited(ft_idx)) {
                        *(ephstate.selectplanet_star) = ft_idx;
                        exostate.set_active_flytarget(ft);
                        if (ephstate.selectplanet_planet) {
                            return ExodusMode::MODE_StarMap;
                        } else {
                            // We only wanted to select a star
                            return ephstate.selectplanet_resolve();
                        }
                    }
                }
            }
            break;
        case GM_MonthPassing:
            {
                update_panel_info_player(TGT_Primary, exostate.get_player(0));
                update_panel_info_ft(TGT_Primary, exostate.get_player(0), selected_ft);

                // TODO_MP: In multiplayer, check this happens at end of each player's turn
                {
                    exostate.reset_alliance_requests();
                    exostate.clear_attack_preventions();
                }

                if (exostate.final_month()) {
                    if (ephstate.get_ephemeral_state() != EPH_GameOver) {
                        ephstate.set_ephemeral_state(EPH_GameOver);
                        ephstate.game_over_reason = GAMEOVER_Failed;
                        audio_manager.fade_out(1200);
                        draw_manager.fade_black(2.4f, 24);
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
                        // Denote that we are constructing; not moving
                        artificial_planet_to_move = nullptr;
                        stage = GM_ArtificialWorldStarSelect;
                        break;
                    case MA_WaitOneMonth:
                        menu_close();
                        stage = GM_MonthPassing;
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
                    if (!artificial_planet_to_move) {
                        player->give_mc(COST_ART);
                    }
                    artificial_planet_to_move = nullptr;
                    stage = GM_Idle;
                    break;
                }

                Galaxy *gal = exostate.get_galaxy();

                ft = get_clicked_flytarget();

                if (ft && (ft != gal->get_guild())) {
                    exostate.set_active_flytarget(ft);
                    if (!player->get_location().has_visited(exostate.tgt2loc(ft))) {
                        /*
                         * I'm not sure if the original prevented this - but I don't
                         * think you'd want to create a planet in a system that you
                         * can't even view to access the planet map!
                         */
                        // FIXME: Bit hacky to reuse DIA_S_ZoomButNotVisited
                        comm_open(DIA_S_ZoomButNotVisited);
                        stage = GM_ArtificialWorldStarSelectInvalid;
                        break;
                    }

                    Star *s = (Star*) ft;
                    int player_idx = exostate.get_active_player_idx();

                    if (artificial_planet_to_move) {
                        // Moving a planet
                        ArtificialPlanetViable apv = exostate.artificial_planet_viable(s);
                        if (apv == APV_Yes) {
                            int tgt_idx = exostate.tgt2loc(s);
                            artificial_planet_to_move->set_star_target(tgt_idx);
                            artificial_planet_to_move = nullptr;
                            stage = GM_Idle;
                        } else {
                            if (apv == APV_No_StarFull) {
                                comm_open(DIA_S_ArtificialPlanetStarInvalid);
                            } else if (apv == APV_No_MoveDest) {
                                comm_open(DIA_S_ArtificialPlanetStarMoveDest);
                            } else {
                                L.error("Unknown ArtificialPlanetViable result %d", apv);
                                comm_open(DIA_S_ArtificialPlanetStarInvalid);
                            }
                            stage = GM_ArtificialWorldStarSelectInvalid;
                            break;
                        }
                    } else {
                        // Constructing a planet
                        // FIXME: It's a bit hacky to rely on the input system to remember this...
                        const char* name = input_manager.get_input_text(PLANET_MAX_NAME);

                        if (exostate.construct_artificial_planet(s, player_idx, name)) {
                            stage = GM_Idle;
                            break;
                        } else {
                            comm_open(DIA_S_ArtificialPlanetStarInvalid);
                            stage = GM_ArtificialWorldStarSelectInvalid;
                            break;
                        }
                    }
                }

                panel_set_text("SELECT A STAR");

                FlyTarget* mouseover_ft = get_mouseover_flytarget();
                if (mouseover_ft) {
                    update_panel_info_ft(TGT_Primary, exostate.get_player(0), mouseover_ft);
                }
            }
            break;
        case GM_ArtificialWorldStarSelectInvalid:
            {
                if (comm_update(delta) != CA_None) {
                    comm_ensure_closed();
                    stage = GM_ArtificialWorldStarSelect;
                }
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
    bulletin_ensure_closed();
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
    mp_state.mp_officer = (Officer)0;
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

    for (int i = 0; i < N_PLAYERS; ++i) {
        p = exostate.get_player(i);
        if (p && p->is_participating()) {
            // We fix trade charge per player per month, so can't re-roll by re-trying
            p->set_trade_charge();
        }
    }

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
        // Towards start of PROCcalculatefly
        // Orig tests gameover here - but we instead perform this test in GM_MonthPassing
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
        for (; mp_state.mp_star_idx < n_stars; ++mp_state.mp_star_idx) {
            exostate.set_active_flytarget(&stars[mp_state.mp_star_idx]);
            Star *s = exostate.get_active_star();

            Planet *fst = s->get_planet(0);
            Planet *snd = s->get_planet(1);

            StarSize sz = s->get_size();

            if ((int)(s->get_size()) > (int)STAR_Huge || onein(2000)) {
                sz = s->expand();
            }

            if ((int)sz > (int)STAR_Huge) {
                if ((int)sz < (int)STAR_Dwarf) {
                    bulletin_start_new(false);
                    bulletin_set_bg(IMG_ME4_MENU);
                    bulletin_set_flag(IMG_TS1_FLAG16);
                    if (sz < STAR_Expand4) {
                        bulletin_set_next_text("SUN %s IS EXPANDING", tmp_caps(s->name));
                    } else {
                        bulletin_set_next_text("SUN %s HAS COLLAPSED", tmp_caps(s->name));
                    }
                    bulletin_set_next_text("");
                    switch(sz) {
                        case STAR_Expand1:
                            bulletin_set_next_text("The sun will have reached its maximum");
                            bulletin_set_next_text("expansion in 3 months.");
                            bulletin_set_next_text("");
                            bulletin_set_next_text("It is recommended not to enter this system");
                            bulletin_set_next_text("with a spacefleet because of high risks at");
                            bulletin_set_next_text("the time of a supernova.");
                            bulletin_set_next_text("");
                            bulletin_set_next_text("Scientific prognosis: At least the first three");
                            bulletin_set_next_text("planets of this system will not survive");
                            bulletin_set_next_text("the supernova.");
                            break;
                        case STAR_Expand2:
                            bulletin_set_next_text("The sun will have reached its maximum");
                            bulletin_set_next_text("expansion in 2 months.");
                            bulletin_set_next_text("");
                            bulletin_set_next_text("Advice for star traders and private traffic:");
                            bulletin_set_next_text("This system must not be entered because of");
                            bulletin_set_next_text("high risks.");
                            break;
                        case STAR_Expand3:
                            bulletin_set_next_text("The sun will have reached its maximum");
                            bulletin_set_next_text("expansion in 1 month.");
                            bulletin_set_next_text("");
                            bulletin_set_next_text("The first planet of the system has been");
                            bulletin_set_next_text("swallowed.");
                            bulletin_set_next_text("");
                            bulletin_set_next_text("The system must not be entered under any");
                            bulletin_set_next_text("circumstances.");
                            break;
                        case STAR_Expand4:
                            bulletin_set_next_text("The sun has reached its maximum");
                            bulletin_set_next_text("expansion and has gone supernova.");
                            bulletin_set_next_text("");
                            bulletin_set_next_text("The first three planets have been");
                            bulletin_set_next_text("destroyed.");
                            bulletin_set_next_text("");
                            bulletin_set_next_text("Fleets that might not yet have left the");
                            bulletin_set_next_text("system will have taken heavy damage.");
                            break;
                        default:
                            L.warn("No bulletin text for expanding sun size %d", sz);
                            break;
                    }
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
                    /*
                     * SUGGEST: If no planets, orig moves star off-screen and players here are moved to guild -
                     * suggest that we leave this as-is and allow an empty star to remain. This does have gameplay
                     * implications - if you can break an otherwise 2-month flight into 2 one-month flights then
                     * you can decide to change your course in the middle month.
                     */
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
        // PROCarrivewp
        /*
         * Note that this is resolving a move scheduled earlier, by
         * setting the star_target property of a planet.
         */
        for (PlanetIterator pi; !pi.complete(); ++pi) {
            Planet *p = pi.get();
            int tgt_idx = p->get_star_target();
            if (tgt_idx >= 0) {
                // We have a copy of the tgt_index - wipe it on p
                // It'll need to be reset after moving anyway
                p->clear_star_target();
                if (p->get_class() != Artificial) {
                    L.warn("Non-artificial planet scheduled to move: %s", p->get_name());
                }
                Star *current = pi.get_star();
                Star *tgt = &(exostate.get_galaxy()->get_stars()[tgt_idx]);
                if (current == tgt) {
                    L.warn("Planet scheduled to move to its own star: %s", p->get_name());
                    continue;
                }
                Planet *tgt_slot = tgt->get_artificial_world_slot();
                if (tgt_slot->exists()) {
                    L.error("Planet %s scheduled to move to slot containing existing planet", p->get_name());
                    continue;
                }
                if (tgt_slot->get_construction_phase() > 0) {
                    L.error("Planet %s scheduled to move to slot containing planet under construction", p->get_name());
                    continue;
                }

                L.info("Moving planet %s: %s -> %s", p->get_name(), current->name, tgt->name);

                // 'Move' planet to new star
                memcpy(tgt_slot, p, sizeof(Planet));
                // And wipe the original
                new (p) Planet();

                // Handle any CPUs which were targeting this planet
                exostate.stabilise_disappeared_planet(p);
            }
        }
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
        // PROCwpc
        for (; mp_state.mp_star_idx < n_stars; ++mp_state.mp_star_idx) {
            for (; mp_state.mp_planet_idx < STAR_MAX_PLANETS; ++mp_state.mp_planet_idx) {
                Planet *p = stars[mp_state.mp_star_idx].get_planet_nocheck(mp_state.mp_planet_idx);
                if (p && p->finalise_construction()) {
                    Player *owner = exostate.get_player(p->get_owner());
                    if (!owner) {
                        // Shouldn't be possible - we scrap worlds under construction on defeat
                        L.error("No owner on completed artificial world");
                        continue;
                    }
                    bulletin_start_new(false);
                    bulletin_set_bg(p->sprites()->bulletin_bg);
                    bulletin_set_flag(flags[owner->get_flag_idx()]);
                    bulletin_set_next_text("");
                    bulletin_set_next_text("ARTIFICIAL PLANET CONSTRUCTION");
                    bulletin_set_next_text("");
                    bulletin_set_next_text("%s is constructing", owner->get_full_name());
                    bulletin_set_next_text("an artificial planet.");
                    mp_state.mp_planet_idx++;
                    return ExodusMode::MODE_None;
                }
            }
            mp_state.mp_planet_idx = 0;
        }
        bulletin_ensure_closed();
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

    /*
     * Orig calls PROCfillstatus (counsellor messages and month report)
     * in between PROCenemytactics and PROCeta.
     * We run it after processing both - is this OK?
     */

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

                /*
                 * Owned planet maintenance happens here (PROCcal_plan).
                 * We implement this with 'MPP' stages below.
                 */
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

            if (mission_state == MS_AssassinCapturedKillChoice) {
                bool killed = bulletin_was_yesno_yes();

                mission_state = MS_None;

                Star *st = (Star*)exostate.get_active_flytarget();
                Planet *pl = exostate.get_active_planet();

                int owner_idx = pl->get_owner();
                Player *owner = exostate.get_player(owner_idx);

                bulletin_start_new(false);
                bulletin_set_bg(pl->sprites()->bulletin_bg);
                bulletin_set_flag(flags[owner->get_flag_idx()]);
                bulletin_write_planet_info(st, pl);
                bulletin_set_next_text("");

                if (killed) {
                    bulletin_set_next_text("The assassin has been shot.");
                } else {
                    int n = exostate.get_n_planets(p);
                    const Fleet& f = p->get_fleet();

                    bulletin_set_next_text("The assassin co-operates.");
                    bulletin_set_next_text("He gives the following information:");
                    bulletin_set_next_text("");
                    bulletin_set_next_text("He was sent by %s.", p->get_full_name());
                    bulletin_set_next_text("%s owns %d Mega Credits", p->get_name(), p->get_mc());
                    bulletin_set_next_text("and rules %d planet%s.", n, n == 1 ? "" : "s");
                    bulletin_set_next_text("%s's fleet consists of %d", p->get_name(), f.size());
                    bulletin_set_next_text("ships.");
                }

                return ExodusMode::MODE_None;
            }

            if (mission_state == MS_AssassinCaptured || mission_state == MS_AssassinCapturedDead) {
                if (!draw_manager.clicked()) {
                    return ExodusMode::MODE_None;
                }

                draw_manager.draw(
                    id(ID::FRAMED_IMG),
                    nullptr);
                frame_remove();

                Star *st = (Star*)exostate.get_active_flytarget();
                Planet *pl = exostate.get_active_planet();

                int owner_idx = pl->get_owner();
                Player *owner = exostate.get_player(owner_idx);

                exostate.register_news(NI_AssassinCaptured);
                bulletin_start_new(false);
                bulletin_set_bg(pl->sprites()->bulletin_bg);
                bulletin_set_flag(flags[owner->get_flag_idx()]);
                bulletin_write_planet_info(st, pl);
                bulletin_set_next_text("");
                bulletin_set_next_text("ASSASSIN AT %s", tmp_caps(pl->get_name()));
                bulletin_set_next_text("");
                bulletin_set_next_text("An assassin has been captured by");
                bulletin_set_next_text("security units of %s.", pl->get_name());
                bulletin_set_next_text("");

                if (mission_state == MS_AssassinCapturedDead) {
                    // Add feminine case to orig
                    if (onein(2)) {
                        bulletin_set_next_text("He has killed himself.");
                    } else {
                        bulletin_set_next_text("She has killed herself.");
                    }

                    mission_state = MS_None;
                } else {
                    // SUGGEST: Would be good to see when the dream influence system was actually helpful
                    if (owner->is_human()) {
                        bulletin_set_next_text("Maybe he wants to cooperate.");
                        bulletin_set_next_text("");
                        bulletin_set_next_text("Do you wish to kill him?");
                        bulletin_set_yesno();
                        mission_state = MS_AssassinCapturedKillChoice;
                    } else {
                        // Again, feminine case easy to add here with no extra state tracking
                        if (onein(2)) {
                            bulletin_set_next_text("Maybe he wants to cooperate.");
                        } else {
                            bulletin_set_next_text("Maybe she wants to cooperate.");
                        }
                        // The CPU owner discovers who sent the spy, and becomes hostile
                        owner->set_hostile_to(mp_state.mp_player_idx);

                        mission_state = MS_None;
                    }
                }

                return ExodusMode::MODE_None;
            }

            if (mission_state == MS_Active) {
                if (p->has_mission()) {
                    // Resolve the mission here
                    mission_state = MS_None;
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
                            ephstate.destruction.show_n_strikes = false;
                            ephstate.destruction.show_replay = false;
                            ephstate.destruction.enable_explosions = true;
                            ephstate.destruction.enable_explosion_anims = true;
                            ephstate.destruction.irradiated = false;
                            ephstate.destruction.show_target = false;
                            // Enemy doesn't know you've attacked them
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
                            ephstate.destruction.show_n_strikes = false;
                            ephstate.destruction.show_replay = false;
                            ephstate.destruction.enable_explosions = false;
                            ephstate.destruction.enable_explosion_anims = false;
                            ephstate.destruction.irradiated = true;
                            ephstate.destruction.show_target = false;
                            // Enemy doesn't know you've attacked them
                            ephstate.destruction.destroyer_idx = -1;
                            ephstate.destruction.terror = false;
                            ephstate.destruction.nuke = true;
                            ephstate.destruction.draw = true;
                            return ephstate.get_appropriate_mode();
                    }
                    continue;
                } else {
                    L.error("Player should have mission if mission_state is MS_Active");
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
                    // on close, set mission_state and return
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
                                r += 2*(int)p->get_officer(OFF_Secret);
                                r = 0;
                                if (r < 4) {
                                    L.info("[%s] SPY CAPTURED - %s", p->get_full_name(), pl->get_name());

                                    // Cancel bulletin in this case...
                                    bulletin_ensure_closed();

                                    audio_manager.target_music(mpart2mus(9));

                                    draw_manager.draw(
                                        id(ID::FRAMED_IMG),
                                        IMG_CT3_EXPORT,
                                        {5, 7, 0, 0, 1, 1});

                                    char t[64];
                                    snprintf(t, sizeof(t), "Assassin captured at %s", pl->get_name());
                                    draw_manager.draw_text(
                                        t,
                                        Justify::Left,
                                        12, 378,
                                        COL_TEXT2);

                                    frame_draw();

                                    // Clear mission now - this does not go ahead
                                    // This also means we do not re-process this player again when we return after the bulletin
                                    p->clear_mission();

                                    if (r != 1 && !owner->has_invention(INV_DreamInfluence)) {
                                        mission_state = MS_AssassinCapturedDead;
                                    } else {
                                        mission_state = MS_AssassinCaptured;
                                    }

                                    return ExodusMode::MODE_None;
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
                                            ephstate.destruction.show_n_strikes = false;
                                            ephstate.destruction.show_replay = false;
                                            ephstate.destruction.enable_explosions = true;
                                            ephstate.destruction.enable_explosion_anims = true;
                                            ephstate.destruction.irradiated = false;
                                            ephstate.destruction.show_target = true;
                                            // Enemy doesn't know you've attacked them
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

                                    mission_state = MS_Active;
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
                            mission_state = MS_Active;
                            return ExodusMode::MODE_None;
                        case MT_Nuclear:
                            bulletin_set_next_text("");
                            bulletin_set_next_text("");
                            audio_manager.target_music(mpart2mus(9));
                            bulletin_set_next_text(
                                "NUCLEAR ATTACK AT %s",
                                tmp_caps(pl->get_name()));
                            mission_state = MS_Active;
                            return ExodusMode::MODE_None;
                    }
                } else {
                    L.info("[%s]: CANCEL MISSION: Invalid planet", p->get_full_name());
                    p->clear_mission();
                }
            }
        }
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_PayOfficers) {
        for (; mp_state.mp_player_idx < N_PLAYERS; ++mp_state.mp_player_idx) {
            Player *p = exostate.set_active_player(mp_state.mp_player_idx);
            if (p && p->is_participating()) {
                Officer &o = mp_state.mp_officer;
                for (; o < OFFICER_MAX; o = (Officer)((int)o + 1)) {
                    int cost = p->get_officer_cost(o);
                    if (cost <= 0) {
                        continue;
                    }
                    if (p->attempt_spend(cost)) {
                        L.debug("[%s] Paid for officer: %d", p->get_full_name(), cost);
                    } else {
                        L.debug("[%s] Could not pay for officer: %d", p->get_full_name(), cost);
                        bulletin_start_new(false);
                        bulletin_set_active_player_flag();
                        bulletin_set_text_col(COL_TEXT3);
                        bulletin_set_next_text("");
                        bulletin_set_next_text("OFFICER NOT PAID");
                        bulletin_set_next_text("");
#if FEATURE_UNPAID_OFFICER_NAMED
                        bulletin_set_next_text("%s, you are unable to", p->get_full_name());
                        switch (o) {
                            case OFF_Science:
                                bulletin_set_next_text("pay your Science Officer.");
                                break;
                            case OFF_Fleet:
                                bulletin_set_next_text("pay your Fleet Admiral.");
                                break;
                            case OFF_Battle:
                                bulletin_set_next_text("pay your Battle General.");
                                break;
                            case OFF_Secret:
                                bulletin_set_next_text("pay your Secret Service Leader.");
                                break;
                            case OFF_Counsellor:
                                bulletin_set_next_text("pay your Ship Counsellor.");
                                break;
                            default:
                                bulletin_set_next_text("pay an officer.");
                                L.warn("Unknown officer going unpaid");
                                break;
                        }
#else
                        bulletin_set_next_text("%s,", p->get_full_name());
                        bulletin_set_next_text("pay an officer.");
#endif
                        if (onein(6)) {
                            bulletin_set_next_text("The officer has left.");
                            p->set_officer(o, OFFQ_Poor);
                        }

                        o = (Officer)((int)o + 1);
                        return ExodusMode::MODE_None;
                    }
                }
            }
        }
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
        // TODO_MP: In a multiplayer game, check if all humans are dead, or one winner remains
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
                    } else {
                        // ...and can't afford to colonise a new one.
                        exostate.cancel_worlds_under_construction(mp_state.mp_player_idx);
                        if (p->leave_galaxy()) {
                            audio_manager.target_music(mpart2mus(9));
                            bulletin_start_new(false);
                            bulletin_set_flag(IMG_TS1_FLAG16);
                            bulletin_set_text_col(COL_TEXT3);
                            bulletin_set_next_text("%s FAILED", tmp_caps(p->get_name()));
                            bulletin_set_next_text("");
                            bulletin_set_next_text("%s has lost all planets and", p->get_full_name());
                            bulletin_set_next_text("left the galaxy.");
                            // This is a special newsitem which isn't planet-associated
                            NewsItem &news = exostate.register_news_force(NI_LeftGalaxy);
                            news.player_0 = mp_state.mp_player_idx;
                            return ExodusMode::MODE_None;
                        }
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

        if (mp_state.mpai_substage == 4) {
            ephstate.clear_ephemeral_state();
            mp_state.mpai_substage = 0;
            resume = true;
        }

        if (mp_state.mpai_substage == 3) {
            Planet *planet = exostate.get_active_planet();
            switch (comm_action_check()) {
                case CA_Abort:
                    comm_close();
                    mp_state.mpai_substage = 0;
                    resume = true;
                    break;
                case CA_Attack:
                    comm_close();
                    mp_state.mpai_substage = 4;
                    L.debug("[%s] MPAI_Alliances : LAUNCHING ATTACK ON %s", player->get_full_name(), planet->get_name());
                    ephstate.set_ephemeral_state(EPH_LunarBattlePrep);
                    ephstate.lunar_battle.aggressor_type = AGG_Player;
                    ephstate.lunar_battle.aggressor_idx = player_idx;
                    return ephstate.get_appropriate_mode();
                default:
                    L.fatal("Unexpected comm action on CPU alliance attack: %d", comm_action_check());
            }
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
                    comm_open(DIA_S_CPU_Attack);
                    mp_state.mpai_substage = 3;
                    return ExodusMode::MODE_None;
                default:
                    L.fatal("Unexpected comm action on CPU alliance offer: %d", comm_action_check());
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
                // Randomly lose alliances
                int j = 0;

#if FEATURE_HUMAN_NO_RANDOM_ALLY_LOSS
                j = exostate.get_n_human_players();
#endif

                for (; j < N_PLAYERS; ++j) {
                    if (onein(r)) {
                        exostate.unset_alliances(player_idx, j);
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
        int hostile_to = player->get_hostile_to();
        if (!player->has_mission() && hostile_to >= 0 && onein(20)) {
            if (exostate.get_orig_month() > 15 && player->can_afford(70)) {
                // PROCet_terror

                bool target_found = false;

                for (PLANETITER pi(hostile_to); !pi.complete(); ++pi) {
                    L.info("[%s]: PLANNING MISSION", player->get_full_name());
                    player->get_mission_star_ref() = pi.get_star_idx();
                    player->get_mission_planet_ref() = pi.get_idx();
                    target_found = true;
                    break;
                }

                if (target_found) {
                    /*
                     * Orig has a bug here whereby the different costs are evaluated
                     * in an inappropriate order such that only the cheapest can ever
                     * be selected!
                     */
                    MissionType plan = MT_TerrorPort;
                    int cost = 100;

#if FIX_AI_TERROR
                    bool nuke_possible = player->has_invention(INV_OrbitalBombs) && exostate.get_orig_month() > 50;
                    if (nuke_possible && player->can_afford(1500)) {
                        plan = MT_Nuclear;
                        cost = 1000;
                    } else if (player->can_afford(201)) {
                        plan = MT_TerrorAgri;
                        cost = 200;
                    } else if (player->can_afford(151)) {
                        plan = MT_TerrorPlu;
                        cost = 150;
                    }
#endif

                    if (player->attempt_spend(cost)) {
                        L.info("[%s]: COMMITTING MISSION %d", player->get_full_name(), plan);
                        player->set_mission_type(plan);
                    }
                } else {
                    L.debug("[%s]: Could not find target planet - clearing hostility", player->get_full_name());
                    // Our nemesis does not have a valid target planet - clear hostility
                    player->clear_hostility();
                }
            }
        }
        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_FleetPurchase) {
        if (exostate.get_orig_month() > 8 && player->can_afford(11) && player->get_tactic() != 6) {
            Fleet &fleet = player->get_fleet_nonconst();

            int n = RND(7);
            for (int i = 0; i <= n; ++i) {
                switch (rand() % 4) {
                    case 0:
                        if (onein(3) && player->attempt_spend(10)) {
                            L.debug("[%s]: BUY SCOUT", player->get_full_name());
                            fleet.scouts++;
                        }
                        break;
                    case 1:
                        for (int j = 0; j < 3; ++j) {
                            if (player->attempt_spend(15)) {
                                L.debug("[%s]: BUY TRANSPORTER", player->get_full_name());
                                fleet.transporters++;
                            }
                        }
                        break;
                    case 2:
                        /*
                         * SUGGEST: AI don't use warships - drop this? Disadvantage is that
                         * it saves the CPU player money. Advantage is that without this noise
                         * you get more meaningful fleet size info from a captured assassin.
                         */
                        if (player->attempt_spend(20)) {
                            L.debug("[%s]: BUY WARSHIP", player->get_full_name());
                            fleet.warships++;
                        }
                        break;
                    case 3:
                        if (player->get_race() != RACE_Teri && player->attempt_spend(25)) {
                            L.debug("[%s]: BUY BOMBER", player->get_full_name());
                            fleet.bombers++;
                        }
                        break;
                }
            }
        }
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
        Planet *art = exostate.get_planet_under_construction(player_idx);
        int phase = art ? art->get_construction_phase() : 0;
        if (art && phase < 3) {
            if (   (phase == 2 && player->can_afford(1500))
                || (phase == 1 && player->can_afford(1000))) {
                if (player->attempt_spend(1000)) {
                    if (!art->advance_construction_phase()) {
                        L.error("Attempt to advance non-advanceable planet");
                    }
                    L.debug("[%s]: ARTIFICIAL PLANET PHASE %d", player->get_full_name(), phase+1);
                }
            }
        }

        phase = art ? art->get_construction_phase() : 0;

        // Initialise an artificial planet we have just finished creating
        // N.B. 'phase' will get reset to 0 in PROCwpc - then it will be a normal planet
        if (phase >= 3) {
            // PROClordbwp: Initialize an artificial planet
            L.debug("[%s]: COMPLETE ARTIFICIAL PLANET", player->get_full_name());
            /*
             * The first thing this PROC does is to set up various planet properties
             * (PROCdowp), however I don't believe this is needed in our version, as
             * this happens when we initialise the planet object.
             *
             * The second thing is to invoke PROCenemytactics for this lord.
             * Our equivalent is ai_planet_update(), however instead of invoking it
             * on all planets now, we will only invoke on the new planet.
             *
             * This is a deviation from the original, which would process all owned
             * planets at this point.
             */
            ai_planet_update(art);
        }

        if (!art && player->has_invention(INV_OrbitalMassConstruction)) {
            if (onein(5) && player->can_afford(1000)) {
                // PROClorddwp: Begin an artificial planet
                for (StarIterator siter; !siter.complete(); ++siter) {
                    Star *s = siter.get();
                    if (exostate.artificial_planet_viable(s) == APV_Yes) {
                        if (player->attempt_spend(1000)) {
                            if (!exostate.construct_artificial_planet(s, player_idx, nullptr)) {
                                L.error("Should be possible - viability check returned true");
                            }
                            L.debug("[%s]: BEGIN ARTIFICIAL PLANET", player->get_full_name());
                        }
                        break;
                    }
                }
            }
        }

        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_MoveArtificialPlanet) {
        // PROCet_movewar
        if (player->has_invention(INV_OrbitalMassThrust)) {
            for (PlanetIterator pi(player_idx); !pi.complete(); ++pi) {
                Planet *p = pi.get();
                if (p->get_class() == Artificial && onein(10)) {
                    // AI have absolutely no tactical decision-making on where to move...
                    for (STARITER si; !si.complete(); ++si) {
                        Star *tgt = si.get();
                        Planet *tgt_slot = tgt->get_artificial_world_slot();
                        if (tgt_slot == p) {
                            continue;
                        }
                        if (exostate.artificial_planet_viable(tgt) == APV_Yes) {
                            // Move planet here
                            p->set_star_target(exostate.tgt2loc(tgt));
                            break;
                        }
                    }
                }
            }
        }
        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_CheckArmy) {
        if (exostate.owns_a_planet(player)) {
            int biggest_army = 0;
            for (PlanetIterator piter; !piter.complete(); ++piter) {
                Planet *p = piter.get();
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
                        player->set_ai_attack(piter.get_star_idx(), piter.get_idx());
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
                for (PLANETITER piter; !piter.complete(); ++piter) {
                    Planet *p = piter.get();
                    if (p->is_owned()) {
                        if (p->get_owner() == player_idx) {
                            continue;
                        }
                        if (p->get_army_size() > 0) {
                            continue;
                        }
                    }
                    player->set_tactic(4);
                    // CPU journeys are always 1 month
                    player->get_location().set_target(piter.get_star_idx(), 1);
                    player->get_location().set_planet_target(piter.get_idx());
                    break;
                }
            }

            if (!exostate.owns_a_planet(player)) {
                player->set_tactic(9);
            }

            if (player->get_tactic() == 6) {
                bool ok = false;
                for (PlanetIterator piter; !piter.complete(); ++piter) {
                    Planet *p = piter.get();
                    if (!p->is_owned()) {
                        ok = true;
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
                for (PlanetIterator piter(player_idx); !piter.complete(); ++piter) {
                    Planet *p = piter.get();
                    int army = p->get_army_size();
                    if (army > quality) {
                        quality = army;
                        player->get_location().set_target(piter.get_star_idx(), 1);
                        player->get_location().set_planet_target(piter.get_idx());
                        L.debug("[%s] PROCe_tact1 : Targeting %s", player->get_full_name(), p->get_name());
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
                for (PLANETITER piter; !piter.complete(); ++piter) {
                    Planet *p = piter.get();
                    if (p->is_owned()) {
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
                        if (ts == piter.get_star_idx() && tp == piter.get_idx()) {
                            ok = false;
                            break;
                        }
                    }
                    if (ok) {
                        set_star = piter.get_star_idx();
                        set_planet = piter.get_idx();
                        break;
                    }
                }

                if (set_star >= 0 && set_planet >= 0) {
                    bool found = false;
                    // Orig repeats forever - we cancel eventually and use the one we already faound
                    for (int attempts = 0; attempts < 1000; ++attempts) {
                        int quality = 0;
                        for (PLANETITER piter; !piter.complete(); ++piter) {
                            Planet *p = piter.get();
                            if (p->is_owned()) {
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
                                    if (ts == piter.get_star_idx() && tp == piter.get_idx()) {
                                        ok = false;
                                        break;
                                    }
                                }
                                if (ok) {
                                    set_star = piter.get_star_idx();
                                    set_planet = piter.get_idx();
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

                        player->clear_ai_attack();
                        if (p->is_owned() && p->get_owner() != player_idx) {
                            player->next_tactic();
                            if (!player->get_location().in_flight()) {
                                player->set_tactic(5);
                                stop = true;
                            }
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
                        for (PLANETITER piter; !piter.complete(); ++piter) {
                            Planet *p = piter.get();
                            if (!p->is_owned()) {
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

                            int star_idx = piter.get_star_idx();
                            int planet_idx = piter.get_idx();

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
                        p->reset_unrest();
                        p->prepare_for_cpu_lord();
                        if (!p->is_named()) {
                            // Try to ensure unique name here. Orig suffixes ' 2', ' 3' etc

                            bool named = false;
                            const char* name = "";

                            for (int i = 0; i < 20; ++i) {
                                name = p->get_name_suggestion();

                                if (!exostate.planet_name_taken(name)) {
                                    p->set_name(name);
                                    named = true;
                                    break;
                                }

                                if (i > 10) {
                                    char name2[PLANET_MAX_NAME];
                                    snprintf(name2, sizeof(name2), "%s2", name);

                                    if (!exostate.planet_name_taken(name2)) {
                                        p->set_name(name2);
                                        named = true;
                                        break;
                                    }
                                }
                            }

                            if (!named) {
                                // Whatever
                                p->set_name(p->get_name_suggestion());
                            }
                        }
                        int cap = p->get_resource_cap();
                        int p_inf, p_gli, p_art;
                        p->get_army(p_inf, p_gli, p_art);
                        // Fill planet army from freight as much as possible without exceeding limit
                        p->adjust_army(
                            player->transfer_inf(-(cap - p_inf)),
                            player->transfer_gli(-(cap - p_gli)),
                            player->transfer_art(-(cap - p_art)));

                        // PROCnpmessage
                        NewsItem& news = exostate.register_news_force(NI_PlanetClaimed);
                        news.player_0 = player_idx;

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

                // Resolve to attack a planet
                if (mp_state.mpai_substage == 0) {
                    mp_state.mpai_substage_player_idx = 0;
                    if (p && p->exists() && p->is_owned() && p->get_owner() != player_idx) {
                        // FIXME: Factor in lunar base here?
                        int p_army = p->get_army_size();
                        int army = player->get_fleet().freight.army_size();
                        if (p_army > army) {
                            // FIXME: I think this is pointless - orig sets back to 0 afterwards anyway
                            player->set_tactic(1);
                            mp_state.mpai_substage = 100;
                        } else {
                            // Bail if planet is aritificial and lord has artificial world
                            if ((p->get_class() == Artificial) && (exostate.has_artificial_planet(player_idx))) {
                                mp_state.mpai_substage = 100;
                            } else {
                                mp_state.mpai_substage = 1;
                            }
                        }
                    } else {
                        mp_state.mpai_substage = 100;
                    }
                }

                // Attack a planet
                if (mp_state.mpai_substage == 1) {
                    L.debug("[%s] PROCe_tact6 : ANNOUNCING ATTACK ON %s AT %s ", player->get_full_name(), p->get_name(), s->name);
                    exostate.set_active_flytarget(s);
                    exostate.set_active_planet(planet_idx);
                    // Unrest increases even before MODE_Arrive, if we're attacking a human planet
                    p->adjust_unrest(1);
                    int owner_idx = p->get_owner();
                    Player *owner = exostate.get_player(owner_idx);
                    if (owner->is_human()) {
                        mp_state.mpai_substage = 2;
                        // PROCenemyplayerattack from this point
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
                            mp_state.mpai_substage = 100;
                            break;
                        case CA_Attack:
                            comm_close();
                            mp_state.mpai_substage = 5;
                            break;
                        default:
                            L.fatal("Unexpected comm action on CPU attack: %d", comm_action_check());
                    }
                }

                // Attacking CPU, requesting support from war allies
                if (mp_state.mpai_substage == 4) {
                    int owner_idx = p->get_owner();
                    Player *owner = exostate.get_player(owner_idx);

                    int &i = mp_state.mpai_substage_player_idx;
                    for (; i < N_PLAYERS; ++i) {
                        if (i == owner_idx) {
                            continue;
                        }

                        Player *other = exostate.get_player(i);

                        // Defender (owner) doesn't request support from player attacking them
                        if (other == player) {
                            continue;
                        }

                        if (!(other && other->is_participating())) {
                            continue;
                        }

                        if (!other->is_human()) {
                            // CPUs never provide war ally support
                            continue;
                        }

                        if (exostate.has_alliance(owner_idx, i, ALLY_War)) {
                            Planet *army_planet = nullptr;
                            int army_sz = -1;
                            // Iterate defender's war ally's planets
                            for (PlanetIterator pi(i); !pi.complete(); ++pi) {
                                int this_army_sz = pi.get()->get_army_size();
                                if (this_army_sz > army_sz) {
                                    army_sz = this_army_sz;
                                    army_planet = pi.get();
                                }
                            }

                            if (army_sz > 1) {
                                int c = max(1, min(RND(4)*2, other->get_mc()));

                                bulletin_start_new(true);
                                bulletin_set_flag(flags[other->get_flag_idx()]);
                                bulletin_set_text_col(COL_TEXT3);
                                bulletin_set_next_text("%s is going to be attacked.", owner->get_full_name());
                                bulletin_set_next_text("");
                                const char* n = army_planet->get_name();
                                switch (owner->get_gender()) {
                                    case GENDER_Female:
                                        bulletin_set_next_text("As her war-ally, you should support");
                                        bulletin_set_next_text("her. Your strongest planet is %s.", n);
                                        break;
                                    case GENDER_Male:
                                        bulletin_set_next_text("As his war-ally, you should support");
                                        bulletin_set_next_text("him. Your strongest planet is %s.", n);
                                        break;
                                    case GENDER_Other:
                                        bulletin_set_next_text("As their war-ally, you should support");
                                        bulletin_set_next_text("them. Your strongest planet is %s.", n);
                                        break;
                                }
                                bulletin_set_next_text("Hyperspace transport costs %d MC.", c);
                                bulletin_set_next_text("How many units do you wish to launch?");
                                bulletin_set_war_ally(army_planet, c);
                                mp_state.mpai_substage = 8;
                                return ExodusMode::MODE_None;
                            }
                        }
                    }
                    mp_state.mpai_substage = 5;
                }

                // Launching attack
                if (mp_state.mpai_substage == 5) {
                    L.debug("[%s] PROCe_tact6 : LAUNCHING ATTACK ON %s AT %s ", player->get_full_name(), p->get_name(), s->name);

                    // SUGGEST: Does orig unset CPU<->CPU alliances? We will here.
                    int owner_idx = p->get_owner();
                    exostate.unset_alliances(owner_idx, player_idx);

                    mp_state.mpai_substage = 6;
                    ephstate.set_ephemeral_state(EPH_LunarBattlePrep);
                    ephstate.lunar_battle.aggressor_type = AGG_Player;
                    ephstate.lunar_battle.aggressor_idx = player_idx;
                    return ephstate.get_appropriate_mode();
                }

                // Handle outcome of battle
                if (mp_state.mpai_substage == 6) {
                    const LunarBattleReport &b = ephstate.lunar_battle_report;
                    Player* defender = exostate.get_player(b.defender_idx);
                    bool won = b.aggressor_won;
                    bool vis = exostate.any_human_has_visited(exostate.get_active_star_idx());
                    ephstate.clear_ephemeral_state();

                    if (vis) {
                        bulletin_start_new(false);
                        bulletin_set_bg(p->sprites()->bulletin_bg);
                        bulletin_set_active_player_flag();
                        bulletin_write_planet_info(s, p);
                        bulletin_set_next_text("%s is under attack.", p->get_name());
                        bulletin_set_next_text("The attacker is %s%s.", player->get_full_name(), player->get_guild_title_str_with_space());

                        // Bulletin part of PROCenemyattack
                        if (won) {
                            bulletin_set_next_text("%s has taken over", player->get_name());
                            bulletin_set_next_text("the planet of %s.", defender->get_full_name());
                            mp_state.mpai_substage = 100;
                        } else {
                            bulletin_set_next_text("%s could keep the planet.", defender->get_full_name());
                            bulletin_set_next_text("%s has lost the battle.", player->get_name());
                            mp_state.mpai_substage = 20;
                        }

                        return ExodusMode::MODE_None;
                    }

                    mp_state.mpai_substage = 100;
                }

                // Handle outcome of war ally bulletin
                if (mp_state.mpai_substage == 8) {
                    int owner_idx = p->get_owner();
                    int supporter_idx = mp_state.mpai_substage_player_idx;

                    Player *owner = exostate.get_player(owner_idx);
                    Player *supporter = exostate.get_player(supporter_idx);

                    // Supporter MC ONLY expended and units added to defender planet when total > 3 - but supporter still loses them
                    int inf, gli, art;
                    int cost = bulletin_get_war_ally_result(inf, gli, art);
                    if (inf + gli + art > 3 && supporter->attempt_spend(cost)) {
                        L.info("[%s] Received support from %s", owner->get_full_name(), supporter->get_full_name());
                        p->adjust_army(inf, gli, art);
                        supporter->add_trace(TRACE_AlliesHelped);
                    } else {
                        L.info("[%s] Did not receive sufficient support from %s", owner->get_full_name(), supporter->get_full_name());
                        if (onein(2)) {
                            // Player under attack calls off alliance
                            exostate.unset_alliances(owner_idx, supporter_idx);
                        }
                    }

                    mp_state.mpai_substage = 4;
                    // Ensure we resume substage 4's loop from the next iteration
                    mp_state.mpai_substage_player_idx++;
                    return ExodusMode::MODE_None;
                }

                if (mp_state.mpai_substage == 20) {
                    mp_state.mpai_substage = 100;
                    // Lord has failed invasion - do we start bombing?
                    int def = p->get_airdef_guns();
                    int bmb = player->get_fleet().bombers;
                    L.debug("BOMB DEF: %d", def);
                    L.debug("BOMB BMB: %d", bmb);
                    if (player->get_race() != RACE_Teri && def < bmb*2) {
                        mp_state.mpai_bombings_max = 2 + RND(3) + (def < 4 ? 3 : 0);
                        mp_state.mpai_bombings_remain = mp_state.mpai_bombings_max;
                        mp_state.mpai_original_bombers = bmb;
                        L.debug("MAX BOMBINGS: %d", mp_state.mpai_bombings_max);
                        mp_state.mpai_substage = 21;
                    } else {
                        L.debug("NO BOMBINGS");
                    }
                }

                if (mp_state.mpai_substage == 21) {
                    if (mp_state.mpai_bombings_remain <= 0) {
                        L.debug("BOMBINGS END");
                        mp_state.mpai_substage = 100;
                    } else {
                        mp_state.mpai_bombings_remain -= 1;
                        exostate.register_news(NI_BombAttack);

                        L.debug("BOMB (Remaining: %d)", mp_state.mpai_bombings_remain);

                        // PROCenemybomb

                        audio_manager.target_music(mpart2mus(8));

                        int owner_idx = p->get_owner();
                        Player *owner = exostate.get_player(owner_idx);

                        Star *s = (Star*)exostate.get_active_flytarget();

                        bulletin_start_new(false);
                        bulletin_set_bg(p->sprites()->bulletin_bg);
                        bulletin_set_flag(flags[owner->get_flag_idx()]);
                        bulletin_write_planet_info(s, p);
                        bulletin_set_next_text("");
                        bulletin_set_next_text("BOMB ATTACK");
                        bulletin_set_next_text("");
                        bulletin_set_next_text("%s's fleet is attacking the planet.", player->get_name());
                        bulletin_set_next_text("");

                        StoneSet targets;

                        StoneSet bset;
                        bset.add(STONE_Inf);
                        bset.add(STONE_Gli);
                        bset.add(STONE_Art);

                        int a = p->count_stones(bset);
                        int b = p->count_stones(STONE_Plu);

                        Stone d = a == 0 ? STONE_Agri : STONE_Plu;

                        if (onein(4)) {
                            if (p->has_stone(STONE_City)) {
                                if (player->get_flag(0) == AI_Hi) {
                                    d = STONE_City;
                                }
                            }
                        }

                        if (onein(3) && b > 0) {
                            targets.add(STONE_Inf);
                            targets.add(STONE_Gli);
                            targets.add(STONE_Art);
                        } else {
                            targets.add(d);
                        }

                        int def = p->get_airdef_guns();
                        int bmb = player->get_fleet().bombers;
                        // If air def guns remain, reconsider bombing
                        if (def > 0) {
                            if (bmb*3 < mp_state.mpai_original_bombers || bmb < mp_state.mpai_bombings_max) {
                                L.debug("Halt bombings early");
                                mp_state.mpai_bombings_remain = 0;
                            }
                        }

                        // N.B. Sets up ephstate
                        int hits, bombers_killed;
                        bomb_planet(targets, false, hits, bombers_killed);

                        if (def > 0) {
                            bulletin_set_next_text("AirDef guns destroyed %d bombers.", bombers_killed);
                            bulletin_set_next_text("");
                        }

                        bulletin_set_next_text("%d bombers have hit a target.", hits);

                        if (hits <= 0) {
                            // Don't show planet map if nothing happened
                            ephstate.clear_ephemeral_state();
                        }

                        mp_state.mpai_substage = 22;

                        return ExodusMode::MODE_None;
                    }
                }

                if (mp_state.mpai_substage == 22) {
                    mp_state.mpai_substage = 23;
                    return ephstate.get_appropriate_mode();
                }

                if (mp_state.mpai_substage == 23) {
                    ephstate.clear_ephemeral_state();
                    // Resume bomb loop
                    mp_state.mpai_substage = 21;
                    return ExodusMode::MODE_None;
                }

                // Always end with this
                if (mp_state.mpai_substage == 100) {
                    mp_state.mpai_substage = 0;
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
                for (PlanetIterator piter(player_idx); !piter.complete(); ++piter) {
                    Planet *p = piter.get();
                    if (p->get_total_reserves() > resources) {
                        resources = p->get_total_reserves();
                        most_resources_star = piter.get_star_idx();
                        most_resources_planet = piter.get_idx();
                        L.debug("[%s] PROCe_tact7 : Targeting", player->get_full_name(), p->get_name());
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
                for (PlanetIterator piter; !piter.complete(); ++piter) {
                    Planet *p = piter.get();
                    if (p->is_owned() && p->get_owner() != player_idx) {
                        L.debug("[%s] PROCe_tact9 : YES", player->get_full_name());
                        other_owned_planets = true;
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
                    for (PLANETITER piter; !piter.complete(); ++piter) {
                        Planet *p = piter.get();
                        if (p->is_owned() && p->get_owner() != player_idx) {
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
                                player->get_location().set_target(piter.get_star_idx(), 1);
                                player->get_location().set_planet_target(piter.get_idx());
                                L.debug("[%s] PROCe_tact10 : TARGETING %s at %s", player->get_full_name(), p->get_name(), piter.get_star()->name);
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
                                L.fatal("Unexpected comm action on CPU trade offer: %d", comm_action_check());
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

                        player->set_tactic(0);
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
                        player->set_tactic(0);
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
                for (PlanetIterator piter(player_idx); !piter.complete(); ++piter) {
                    Planet *p = piter.get();
                    int army = p->get_army_size();
                    if (army > quality) {
                        quality = army;
                        player->get_location().set_target(piter.get_star_idx(), 1);
                        player->get_location().set_planet_target(piter.get_idx());
                        L.debug("[%s] PROCe_tact12 : TARGETING %s AT %s", player->get_full_name(), p->get_name(), piter.get_star()->name);
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
                ephstate.destruction.show_n_strikes = true;
                ephstate.destruction.show_replay = true;
                ephstate.destruction.enable_explosions = true;
                ephstate.destruction.enable_explosion_anims = true;
                ephstate.destruction.irradiated = false;
                ephstate.destruction.show_target = false;
                ephstate.destruction.destroyer_idx = -1;
                ephstate.destruction.terror = false;
                ephstate.destruction.nuke = false;
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
                // SUGGEST: Should we tell the player how many have been destroyed?
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
                bulletin_set_next_text("%s%s could keep the planet.",
                    owner->get_full_name(),
                    owner->get_guild_title_str_with_space());
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
        if (exostate.get_orig_month() > 2) {
            if (p->count_stones(STONE_City) > 0) {
                // This is a nice touch - no species left if you've mined all resources!
                if (p->get_class() != Artificial && p->get_minerals() > 0) {
                    int threshold = 1;
                    int q = (int)owner->get_officer(OFF_Science);
                    threshold += 2*q - 2;
                    threshold += owner->get_tax() / 20;
                    if (RND(50) <= threshold) {
                        discover_species_bulletin(p);
                        next_mpp_stage();
                        return ExodusMode::MODE_None;
                    }
                }
            }
        }
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

                    if (owner) {
                        owner->add_trace(TRACE_RevolutionsCaused);
                    }

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

            int star_idx = exostate.get_active_star_idx();
            Player *new_owner = nullptr;
            int new_owner_idx = -1;
            int attempts = 0;
            while (attempts < 200 && !new_owner) {
                attempts++;
                int idx = rand() % N_PLAYERS;
                Player *pl = exostate.get_player(idx);
                if (!pl)
                    continue;
                if (pl == owner)
                    continue;
                if (!pl->is_participating())
                    continue;
                if (!pl->get_location().has_visited(star_idx))
                    continue;
                new_owner_idx = idx;
                new_owner = pl;
                break;
            }

            bulletin_set_next_text("The revolution of %s", p->get_name());
            bulletin_set_next_text("");
            if (rpt.aggressor_won) {
                bool republic = !new_owner;

#if FEATURE_REBEL_REPUBLIC_CHANCE
                republic = republic || onein(5);
#endif

                exostate.register_news(NI_SuccessfulRevolution);
                bulletin_set_next_text("The rebels have succeeded. So the");
                bulletin_set_next_text("planet needs a new leader. The people");
                if (!republic) {
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
                ephstate.destruction.show_n_strikes = false;
                ephstate.destruction.show_replay = false;
                ephstate.destruction.enable_explosions = true;
                ephstate.destruction.enable_explosion_anims = true;
                ephstate.destruction.irradiated = true;
                ephstate.destruction.show_target = true;
                ephstate.destruction.destroyer_idx = -1;
                ephstate.destruction.terror = false;
                ephstate.destruction.nuke = false;
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

    if (mp_state.mpp_stage == MPP_Income) {
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

    const int repair_cost = 5;

    if (mp_state.mpp_stage == MPP_MilitaryFacilityShutdown) {
        int &n = mp_state.mp_production_shutdown;
        n = 0;

        if (exostate.get_orig_month() > 10) {
            StoneSet produce_set;
            produce_set.add(STONE_Inf);
            produce_set.add(STONE_Gli);
            produce_set.add(STONE_Art);

            int production = p->count_stones(produce_set);

            for (int i = 0; i < production; ++i) {
                if (onein(50)) {
                    ++n;
                }
            }

            if (owner && (n > 0)) {
                int cost = n*repair_cost;

                // PROCcs_repair
                if (owner->is_human()) {
                    bulletin_start_new(true);
                    bulletin_set_bg(p->sprites()->bulletin_bg);
                    bulletin_set_active_player_flag();
                    bulletin_write_planet_info(s, p);
                    bulletin_set_next_text("%d production unit%s need%s to be repaired.", n, n>1?"s":"", n>1?"":"s");
                    bulletin_set_next_text("Cost: %dMC. Do you wish to pay?", cost);
                    bulletin_set_yesno();
                    next_mpp_stage();
                    return ExodusMode::MODE_None;
                } else {
                    owner->attempt_spend_cpuforce(cost);
                    n = 0;
                }
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_MilitaryFacilityShutdown2) {
        int& n = mp_state.mp_production_shutdown;

        if (n > 0) {
            if (owner && owner->is_human()) {
                if (bulletin_was_yesno_yes()) {
                    while (n > 0 && owner->attempt_spend(repair_cost)) {
                        n--;
                    }
                }
            }
        }

        if (n > 0) {
            report.add_line("%d production unit%s ha%s stopped.", n, n>1?"s":"", n>1?"ve":"s");

            while (n > 0) {
                StoneSet set;
                set.add(STONE_Inf);
                set.add(STONE_Gli);
                set.add(STONE_Art);

                Stone s;
                int x, y;
                if (p->find_random_stone(set, s, x, y)) {
                    p->set_stone(x, y, STONE_Rubble);
                    --n;
                } else {
                    L.error("Could not find production unit we verified must exist");
                    break;
                }
            }
        }

        n = 0;
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
        if (ephstate.research.cost > 0 && !owner->attempt_spend(ephstate.research.cost)) {
            L.fatal("Option was given to spend more on research than player's MC. Cost: %d", ephstate.research.cost);
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
                NewsItem& news = exostate.register_news(NI_NewInvention);
                news.player_0 = p->get_owner();
                news.inv = inv;
                bulletin_start_new(false);
                bulletin_set_bg(IMG_ME7_MENU);
                bulletin_set_active_player_flag();
                bulletin_set_text_col(COL_TEXT2);
                bulletin_set_next_text("New invention: %s", owner->get_invention_str(inv));
                bulletin_set_text_col(COL_TEXT_SPEECH);
                bulletin_set_next_text("(%s Research)", owner->get_invention_type_str(inv));

                bulletin_set_next_text("");

                const char *d0, *d1, *d2;
                owner->get_invention_description(inv, d0, d1, d2);
                bulletin_set_next_text(d0);
                bulletin_set_next_text(d1);
                bulletin_set_next_text(d2);

                bulletin_set_next_text("");

                bool unlocks = false;
                for (int i = 0; i < (int)INV_MAX; ++i) {
                    Invention _inv = (Invention)i;
                    if (_inv == inv) {
                        continue;
                    }

                    // FIXME: We report 'unlocks' even if these have other unmet dependencies
                    if (owner->get_invention_prerequisites(_inv) & (1 << (int)inv)) {
                        if (!unlocks) {
                            bulletin_set_next_text("This makes possible:");
                            unlocks = true;
                        }
                        bulletin_set_next_text("  %s", owner->get_invention_str(_inv));
                    }
                }
                if (inv >= INV_OrbitalBombs) {
                    // Changed phraseology slightly here - original "...Planet X earns therefore Y MC"
                    bulletin_set_next_text("The owner of Planet %s therefore", p->get_name());
                    bulletin_set_next_text("receives %dMC.", mc_reward);
                    owner->give_mc(mc_reward);
                }
                next_mpp_stage();
                return true;
            } else {
                // CPU players get MC for all inventions - not just the later ones
                owner->give_mc(mc_reward);
            }
        } else {
            // New species discovery if all inventions researched
            discover_species_bulletin(p);
            return true;
        }
    }

    return false;
}

void GalaxyMap::discover_species_bulletin(Planet* p) {
    // PROCnewexplored

    if (!p->is_owned()) {
        L.error("Discover species invalid on unowned planet %s", p->get_name());
        return;
    }

    Star* s = exostate.get_star_for_planet(p);

    Player* owner = exostate.get_player(p->get_owner());

    if (owner->is_human()) {
        audio_manager.target_music(mpart2mus(10));
    }

    bulletin_start_new(true);
    bulletin_set_bg(p->sprites()->bulletin_bg);
    bulletin_set_active_player_flag();
    bulletin_write_planet_info(s, p);

    if (onein(2)) {
        exostate.register_news(NI_NewAnimal);
        bulletin_set_next_text("NEW ANIMAL DISCOVERED");
        bulletin_set_next_text("");
        bulletin_set_next_text("The animal has been given the name");

        static const char* d0[] {
            "fu", "nuk", "tak", "kik", "neknek", "som", "rok", "dar",
                "gle", "ath", "um", "not", "pi", "si", "efith", "anith",
                "gnog", "sni", "mis", "ryk"};

        char name[64];
        int w = 0;
        int lim = (1 + RND(2));
        for (int i = 0; i < lim; ++i) {
            w += snprintf(name+w, sizeof(name)-w, d0[rand()%20]);
            if (onein(4) && (i+1 < lim)) {
                w += snprintf(name+w, sizeof(name)-w, "-");
            }
        }
        name[0] -= ('a'-'A');

        bulletin_set_next_text("'%s'.", name);
        bulletin_set_next_text("");

        static const char* d1[] {
            "fuzzy", "hairy", "slimy", "ugly",
                "strange", "funny", "ugly", "fearsome",
                "gigantic", "little", "small", "three-eyed",
                "two-legged", "five-legged", "fish-like", "dog-like",
                "horse-like", "frog-like", "stupid", "wicked"};

        static const char* d2[] {
            "red", "brown", "black", "green", "blue",
                "white", "grey", "violet", "yellow", "striped"};

        static const char* d3[] {
            "in water", "in swamps", "in the sky", "on the ground",
                "in woods", "in deserts", "in forests", "in the earth",
                "in caves", "on trees"};

        static const char* d4[] {
            "flesh", "wood", "earth", "plants", "grass"};

        const char* s1 = d1[rand() % (sizeof(d1)/sizeof(const char*))];
        const char* s2 = d2[rand() % (sizeof(d2)/sizeof(const char*))];
        const char* s3 = d3[rand() % (sizeof(d3)/sizeof(const char*))];
        const char* s4 = d4[rand() % (sizeof(d4)/sizeof(const char*))];

        bulletin_set_next_text("This is a %s %s creature that", s1, s2);
        bulletin_set_next_text("lives %s and eats %s.", s3, s4);
        bulletin_set_next_text("");
        int reward = RND(5)*5;
        bulletin_set_next_text("The owner of planet %s therefore", p->get_name());
        bulletin_set_next_text("receives %d Mega Credits.", reward);
        owner->give_mc(reward);
    } else {
        exostate.register_news(NI_NewPlant);
        bulletin_set_next_text("NEW PLANT DISCOVERED");
        bulletin_set_next_text("");
        bulletin_set_next_text("The plant has been given the name");

        static const char* d0[] {
            "som", "tor", "ras", "rak", "rek", "rik", "rok", "ruk",
                "pha", "sta", "ana", "kus", "nos", "ron", "dat", "abi",
                "niri", "deri", "ared", "nodo"};

        char name[64];
        int w = 0;
        int lim = (1 + RND(2));
        for (int i = 0; i < lim; ++i) {
            w += snprintf(name+w, sizeof(name)-w, d0[rand()%20]);
            if (onein(4) && (i+1 < lim)) {
                w += snprintf(name+w, sizeof(name)-w, "-");
            }
        }
        name[0] -= ('a'-'A');

        bulletin_set_next_text("'%s'.", name);
        bulletin_set_next_text("");

        static const char* d1[] {
            "slimy", "ugly", "strange", "nice", "intelligent",
                "gigantic", "little", "small", "beautiful", "frightening"};

        static const char* d2[] {
            "red", "brown", "black", "green", "blue",
                "white", "grey", "violet", "yellow", "striped"};

        static const char* d3[] {
            "water", "swamps", "tropical zones", "deserts", "woods",
                "dark zones", "forests", "large caves", "mountain areas", "rubble"};

        const char* s1 = d1[rand() % (sizeof(d1)/sizeof(const char*))];
        const char* s2 = d2[rand() % (sizeof(d2)/sizeof(const char*))];
        const char* s3 = d3[rand() % (sizeof(d3)/sizeof(const char*))];

        bulletin_set_next_text("This is a %s plant with %s", s1, s2);
        bulletin_set_next_text("flowers. It grows in %s.", s3);
        bulletin_set_next_text("");
        int reward = RND(4)*5;
        bulletin_set_next_text("The owner of planet %s therefore", p->get_name());
        bulletin_set_next_text("receives %d Mega Credits.", reward);
        owner->give_mc(reward);
    }
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
