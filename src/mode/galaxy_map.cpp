#include "galaxy_map.h"

#include "assetpaths.h"

#include "util/str.h"
#include "util/value.h"

#define BLINK_TIME 0.5

#ifdef DBG
extern ExodusDebug exodebug;
#endif

enum ID {
    SELECTED,
    FLEET_MARKER,
    MONTH_PASSING,
    FRAMED_IMG,
    END,
};

GalaxyMap::GalaxyMap() : ModeBase("GalaxyMap"), GalaxyDrawer(), PanelDrawer(PNL_Galaxy), CommPanelDrawer(), BulletinDrawer(), FrameDrawer() {
    stage = GM_SwapIn;
    selected_ft = nullptr;
    selected_ft_blink = 0;

    mp_state.mp_stage = MP_None;
    mp_state.mpai_stage = (MonthPassAIStage)0;
    mp_state.mpp_stage = (MonthPassPlanetStage)0;
    mp_state.month_pass_time = 0;

    do_first_city = false;
    do_meteor = false;
    do_meltdown = false;
    do_lunar_battle = false;
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

    if (mp_state.mp_stage == MP_None) {
        stage = GM_SwapIn;
    } else {
        stage = GM_MonthPassing;
    }

    selected_ft = nullptr;
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
                    if (!player->get_location().in_flight() && selected_ft != exostate.loc2tgt(player->get_location().get_target())) {
                        comm_open(DIA_S_PlanFly);
                        stage = GM_FlyConfirm;
                        return ExodusMode::MODE_None;
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
                        exostate.set_active_flytarget(selected_ft);
                        exostate.set_active_planet(-1);
                        draw_manager.show_cursor(false);
                        draw_manager.fade_black(1.2f, 24);
                        if (selected_ft == gal->get_guild()) {
                            stage = GM_Zoom2Guild;
                        } else {
                            stage = GM_Zoom2Star;
                        }
                    } else {
                        L.debug("Can't zoom - not visited");
                    }
                }
            }

            // Hotkeys
            if (input_manager.consume(K_Space)) {
                stage = GM_MonthPassing;
            }

            break;
        case GM_Zoom2Guild:
            return ExodusMode::MODE_GuildExterior;
        case GM_Zoom2Star:
            return ExodusMode::MODE_StarMap;
        case GM_FlyConfirm:
            action = comm_update(delta);
            if (action == CA_Proceed) {
                // TODO: Vary number of months
                player->get_location().set_target(exostate.tgt2loc(selected_ft), 1);
                return ExodusMode::MODE_Fly;
            } else if (action == CA_Abort) {
                comm_close();
                stage = GM_Idle;
            }
            break;
        case GM_MonthPassing:
            {
                update_panel_info_player(TGT_Primary, exostate.get_player(0));

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
                    }
                }

                if (comm_is_open()) {
                    if (comm_update(delta) == CA_None) {
                        break;
                    }
                }

                ExodusMode next_mode = month_pass_update();
                mp_state.month_pass_time += delta;
                if (mp_state.mp_stage == MP_None) {
                    bulletin_ensure_closed();
                    // The only place we emerge from month-pass-specific stages...
                    stage = GM_Idle;
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
        case GM_Menu:
            if (menu_is_open()) {
                menu_update(delta);
                switch (menu_get_action()) {
                    case MA_Close:
                        menu_close();
                        stage = GM_Idle;
                    case MA_EquipShip:
                        menu_close();
                        return ExodusMode::MODE_ShipEquip;
                    default:
                        break;
                }
            } else {
                stage = GM_Idle;
            }
        default:
            break;
    }

    return ExodusMode::MODE_None;
}

void GalaxyMap::exit() {
    comm_ensure_closed();
    ModeBase::exit();
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
    L.debug("[%s] MPAI stage %d", p->get_full_name(), mp_state.mpai_stage);
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
    L.debug("[%s/%s] MPP stage %d", s->name, p->get_name(), mp_state.mpp_stage);

}

void GalaxyMap::month_pass_start() {
    L.info("Month passing...");
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

    draw_manager.fill(
        id(ID::MONTH_PASSING),
        {160, 150, RES_X - 320, 30},
        COL_BORDERS
    );

    draw_manager.draw_text(
        "One month is passing by",
        Justify::Centre,
        RES_X / 2, 154,
        COL_TEXT2
    );

    reset_planet_report();
    exostate.advance_month();
}

void GalaxyMap::month_pass_end() {
    mp_state.mp_stage = MP_None;
    mp_state.mpai_stage = (MonthPassAIStage)0;
    mp_state.mpp_stage = (MonthPassPlanetStage)0;

    L.info("Month passed");

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
        // TODO - PROCsunexpand
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

    if (mp_state.mp_stage == MP_UpdatePirateProbabilities) {
        // TODO SunP
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

    if (mp_state.mp_stage == MP_AlienMissions) {
        // TODO - PROCdomission etc
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_PayOfficers) {
        // TODO - PROCpayoff etc
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_GuildCommendations) {
        // TODO - PROCnewtitle
        next_mp_stage();
    }

    if (mp_state.mp_stage == MP_AlienExile) {
        // TODO - PROCcheckalive
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
        // TODO
        next_mpai_stage();
    }

    if (mp_state.mpai_stage == MPAI_Die) {
        // TODO
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
                // PROCe_tact1
                L.debug("[%s] PROCe_tact1", player->get_full_name());
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
                            }
                        }
                    }
                }
                if (player->get_location().in_flight()) {
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
            }
            if (!p->get_location().in_flight() && (p->get_tactic() == 8 || p->get_tactic() == 2)) {
                // PROCe_tact2
                L.debug("[%s] PROCe_tact2", player->get_full_name());
                int star_idx = player->get_location().get_target();
                int planet_idx = player->get_location().get_planet_target();
                Star *star = &stars[star_idx];
                Planet *planet = star->get_planet(planet_idx);
                if (planet && planet->exists() && planet->get_owner() == player_idx) {
                    int planet_inf, planet_gli, planet_art;
                    planet->get_army(planet_inf, planet_gli, planet_art);
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

                    int n = exostate.get_n_planets(player);
                    if ((n > 1 && onein(2)) || (n == 1 && onein(5))) {
                        player->prev_tactic();
                    } else {
                        player->next_tactic();
                    }
                } else {
                    player->prev_tactic();
                }
            }
            if (!p->get_location().in_flight() && p->get_tactic() == 9) {
                // PROCe_tact3
                L.debug("[%s] PROCe_tact3", player->get_full_name());
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
            }
            if (!p->get_location().in_flight() && p->get_tactic() == 3) {
                // PROCe_tact4
                L.debug("[%s] PROCe_tact4", player->get_full_name());
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
                            player->get_location().set_target(weak_star, 1);
                            player->get_location().set_planet_target(weak_planet);
                        }
                        if (nodef) {
                            player->get_location().set_target(nodef_star, 1);
                            player->get_location().set_planet_target(nodef_planet);
                        }
                        if (onein(3) || weak_star == -1) {
                            if (mostcities_star >= 0 && mostcities_planet >= 0) {
                                player->get_location().set_target(mostcities_star, 1);
                                player->get_location().set_planet_target(mostcities_planet);
                            }
                        }
                        if (onein(3) || weak_star == -1) {
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
            }
            if (!p->get_location().in_flight() && p->get_tactic() == 10) {
                // PROCe_tact5
                L.debug("[%s] PROCe_tact5", player->get_full_name());
                int star_idx = player->get_location().get_target();
                int planet_idx = player->get_location().get_planet_target();
                Star *s = &stars[star_idx];
                Planet *p = s->get_planet(planet_idx);
                if (p && p->exists() && !p->is_owned()) {
                    // TODO: Orig doesn't check affordability here, allowing -ve MC
                    if (player->attempt_spend(190)) {
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

                        player->set_tactic(0);

                        // PROCenemytactics call
                        ai_planet_update(p);
                    } else {
                        L.warn("[%s] Prevented planet claim due to funds");
                    }
                }
                player->set_tactic(0);
                player->get_location().unset_target();
            }
            if (!p->get_location().in_flight() && p->get_tactic() == 5) {
                // TODO: PROCe_tact6
                L.debug("[%s] PROCe_tact6", player->get_full_name());
            }
            if (p->get_tactic() == 20) {
                // PROCe_tact7
                L.debug("[%s] PROCe_tact7", player->get_full_name());
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
            }
            if (!p->get_location().in_flight() && p->get_tactic() == 21) {
                // PROCe_tact8
                L.debug("[%s] PROCe_tact8", player->get_full_name());
                int star_idx = player->get_location().get_target();
                int planet_idx = player->get_location().get_planet_target();
                Star *star = &stars[star_idx];
                Planet *planet = star->get_planet(planet_idx);
                if (planet && planet->exists() && planet->get_owner() == player_idx) {
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
            }
            if (p->get_tactic() == 22) {
                // PROCe_tact9
                L.debug("[%s] PROCe_tact9", player->get_full_name());
                bool other_owned_planets = false;
                for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
                    Star *s = &stars[star_idx];
                    for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                        Planet *p = s->get_planet(planet_idx);
                        if (p && p->exists() && p->is_owned() && p->get_owner() != player_idx) {
                            other_owned_planets = true;
                            break;
                        }
                    }
                    if (other_owned_planets) {
                        break;
                    }
                }
                if (!other_owned_planets) {
                    player->set_tactic(0);
                }
            }
            if (!p->get_location().in_flight() && p->get_tactic() == 22) {
                // PROCe_tact10
                L.debug("[%s] PROCe_tact10", player->get_full_name());
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
                                }
                            }
                        }
                    }
                }
                if (player->get_location().in_flight()) {
                    if (player->has_invention(INV_OrbitalMassThrust) && onein(2)) {
                        player->next_tactic();
                        player->get_location().complete();
                    }
                } else {
                    player->next_tactic();
                }
            }
            if (!p->get_location().in_flight() && p->get_tactic() == 23) {
                // PROCe_tact11
                L.debug("[%s] PROCe_tact11", player->get_full_name());
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
                            case CA_Proceed:
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
                                // FIXME: Orig allows CPU to pay price they can't afford (-ve MC) here
                                if (!player->attempt_spend(n)) {
                                    L.warn("[%s] Preventing trade that was not affordable",
                                            player->get_full_name());
                                    player->set_tactic(22);
                                    return ExodusMode::MODE_None;
                                }
                            }
                        }

                        const Freight &f = player->get_fleet().freight;
                        player->give_mc(a*player->transfer_min(-f.minerals));
                        player->give_mc(b*player->transfer_fd(-f.food));
                        player->give_mc(c*player->transfer_plu(-f.plutonium));
                        player->set_tactic(0);
                        // TODO: PROClordbuy
                    } else {
                        player->set_tactic(22);
                    }
                }
            }
            if (p->get_tactic() == 0) {
                // TODO: PROCe_tact12
                L.debug("[%s] PROCe_tact12", player->get_full_name());
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
                // TODO - Music...
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
                exostate.first_spaceport_done = true;
                // TODO
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
                        return ExodusMode::MODE_None;
                    }
                } else {
                    // CPU players will always get the research free of charge
                    ephstate.research.cost = 0;
                    ephstate.research.done = true;
                }
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_Research) {
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
                    return ExodusMode::MODE_None;
                } else {
                    // CPU players get MC for all inventions - not just the later ones
                    owner->give_mc(mc_reward);
                }
            } else {
                // TODO: New species discovery if all inventions researched
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_Meteors) {
        if (onein(200)) {
            // TODO PROCdonotice
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
                ephstate.destruction.n_strikes = RND(7) + 1; // 2-8 hits
                ephstate.destruction.enable_explosions = true;
                ephstate.destruction.irradiated = false;
                ephstate.destruction.show_target = false;
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
                // TODO: News item (PROCdonotice)
                next_mpp_stage();
                return ExodusMode::MODE_None;
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_ClimateChange) {
        if (onein(250)) {
            const char* before = p->get_class_str_lower();
            p->surfchange();
            const char* after = p->get_class_str_lower();
            bulletin_start_new(true);
            bulletin_set_bg(p->sprites()->bulletin_bg);
            bulletin_set_active_player_flag();
            bulletin_write_planet_info(s, p);
            // TODO: Music
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
                // TODO: Music, PROCdonotice
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
            // TODO: PROCdonotice, music
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
        // TODO
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_AlienResearch) {
        // TODO
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_DiscoverSpecies) {
        // TODO
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_CityEpidemic) {
        // TODO
        next_mpp_stage();
    }

    // Can cause the owner to change - we should return to ensure
    // that the 'owner' variable is updated when we resume processing.
    if (mp_state.mpp_stage == MPP_RebelAttack) {
        // TODO
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
                // TODO: Music
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
                ephstate.destruction.tgt_stone = STONE_Plu;
                ephstate.destruction.n_strikes = 1;
                ephstate.destruction.enable_explosions = true;
                ephstate.destruction.irradiated = true;
                ephstate.destruction.show_target = true;
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
                    // TODO: Bulletin
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
                    // TODO: Bulletin
                }
            }
        }
        next_mpp_stage();
    }

    if (mp_state.mpp_stage == MPP_LosePlanetControl) {
        if (p->count_stones(STONE_Base) == 0) {
            // TODO: music
            // No PROCdonotice here?
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
        if (p->count_stones(STONE_Trade)) {
            TradeReport rpt = p->monthly_trade();
            report.add_line("Trading Centre sold:");
            report.add_line("Mi: %d / Fd: %d / Pl: %d => %d MC",
                             rpt.mi,  rpt.fd,  rpt.pl, rpt.mc);
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
            // TODO: PROCdonotice
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
                    // TODO: PROCdonotice
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
            // TODO: PROCdonotice for human players
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
                // TODO: PROCdonotice
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
