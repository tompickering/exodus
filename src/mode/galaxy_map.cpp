#include "galaxy_map.h"

#include "assetpaths.h"

#define BLINK_TIME 0.5

enum ID {
    SELECTED,
    FLEET_MARKER,
    MONTH_PASSING,
    END,
};

GalaxyMap::GalaxyMap() : ModeBase("GalaxyMap"), GalaxyDrawer(), PanelDrawer(PNL_Galaxy), CommPanelDrawer() {
    stage = GM_SwapIn;
    selected_ft = nullptr;
    selected_ft_blink = 0;
    mp_stage = MP_None;
    month_pass_time = 0;
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

    if (mp_stage == MP_None) {
        stage = GM_SwapIn;
    } else {
        stage = GM_MonthPassing;
    }

    selected_ft = nullptr;
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
            break;
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
                const char* marker_icon;
                get_draw_position(fleet_pos, draw_x, draw_y);

                if (!player->get_location().in_flight()) {
                    marker_icon = player->get_fleet_marker();
                } else {
                    marker_icon = IMG_TS1_DEST;
                }

                draw_manager.draw(
                    id(ID::FLEET_MARKER),
                    marker_icon,
                    {draw_x - 10, draw_y + 10, 0.5, 0.5, 1, 1});
            }

            click = draw_manager.query_click(id_panel);
            if (click.id) {
                if (click.x < 0.25) {
                    // Fly
                    if (!player->get_location().in_flight() && selected_ft != exostate.loc2tgt(player->get_location().get_target())) {
                        comm_set_title("Message from counsellor");
                        comm_set_img_caption("COUNSELLOR");
                        if (selected_ft == gal->get_guild()) {
                            comm_set_text(0, "For our flight to the space");
                            comm_vset_text(1, "guild, we need X months.", selected_ft->name);
                        } else {
                            comm_set_text(0, "For our flight to the star");
                            comm_vset_text(1, "%s, we need X months.", selected_ft->name);
                        }
                        comm_set_text(2, "A pirate attack is unlikely.");
                        comm_set_text(4, "Do you wish to start?");
                        comm_open(6);
                        stage = GM_FlyConfirm;
                        return ExodusMode::MODE_None;
                    }
                } else if (click.x < 0.5) {
                    L.debug("Panel 1");
                } else if (click.x < 0.75) {
                    L.debug("Panel 2");
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
            action = comm_check_action();
            if (action == CA_Proceed) {
                // TODO: Vary number of months
                player->get_location().set_target(exostate.tgt2loc(selected_ft), 1);
                return ExodusMode::MODE_Fly;
            } else if (action == CA_Abort) {
                comm_close();
                return ExodusMode::MODE_Reload;
            }
            break;
        case GM_MonthPassing:
            {
                ExodusMode next_mode = month_pass_update();
                month_pass_time += delta;
                if (mp_stage == MP_None) {
                    // The only place we emerge from month-pass-specific stages...
                    stage = GM_Report;
                }
                return next_mode;
            }
        case GM_Report:
            // TODO: Planet / status reports etc
            L.debug("End of month report...");
            stage = GM_Idle;
            return ExodusMode::MODE_None;
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
    mp_stage = (MonthPassStage)((int)mp_stage + 1);
    L.debug("MP stage %d", mp_stage);
    // Reset all mp-stage state tracking
    mp_player_idx = 0;
    mp_star_idx = 0;
    mp_planet_idx = 0;
}

void GalaxyMap::month_pass_start() {
    L.info("Month passing...");
    month_pass_time = 0;

    if (mp_stage != MP_None) {
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

    exostate.advance_month();
}

void GalaxyMap::month_pass_end() {
    draw_manager.draw(id(ID::MONTH_PASSING), nullptr);
    mp_stage = MP_None;

    L.info("Month passed");

    Player *p;

    // Reset to the first active player
    bool reset;
    for (int i = 0; i < N_PLAYERS; ++i) {
        p = exostate.set_active_player(i);
        if (p && p->is_active() && p->is_human()) {
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

    if (mp_stage == MP_None) {
        month_pass_start();
        next_mp_stage();
        // Allow an update to display the 'waiting' dialogue...
        return ExodusMode::MODE_None;
    }

    if (mp_stage == MP_TimeDelay) {
        // Time delay here so the "Month passing" notification doesn't flicker.
        if (month_pass_time > 1.f) {
            next_mp_stage();
        } else {
            return ExodusMode::MODE_None;
        }
    }

    if (mp_stage == MP_CheckMissionFail) {
        // TODO: Check if we've run out of time for our mission
        next_mp_stage();
    }

    if (mp_stage == MP_StarshipRepairs) {
        for (; mp_player_idx < N_PLAYERS; ++mp_player_idx) {
            Player *p = exostate.set_active_player(mp_player_idx);
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

    if (mp_stage == MP_UpdateReputation) {
        for (; mp_player_idx < N_PLAYERS; ++mp_player_idx) {
            Player *p = exostate.set_active_player(mp_player_idx);
            if (p->get_reputation() < 3 && onein(20)) {
                p->adjust_reputation(1);
            }
        }
        next_mp_stage();
    }

    if (mp_stage == MP_SunExpansion) {
        // TODO - PROCsunexpand
        next_mp_stage();
    }

    if (mp_stage == MP_MoveArtificialPlanets) {
        // TODO - PROCarrivewp
        next_mp_stage();
    }

    if (mp_stage == MP_EnemyTactics) {
        // TODO - PROCenemythinks
        next_mp_stage();
    }

    if (mp_stage == MP_ArtificialWorldNews) {
        // TODO - PROCwpc
        next_mp_stage();
    }

    if (mp_stage == MP_EnemyActions) {
        // TODO - PROCenemytactics / run PROCeta functions
        next_mp_stage();
    }

    if (mp_stage == MP_UpdatePirateProbabilities) {
        // TODO SunP
        next_mp_stage();
    }

    if (mp_stage == MP_PlanetMaintenance) {
        unsigned int n_stars;
        Galaxy *gal = exostate.get_galaxy();
        Star *stars = gal->get_stars(n_stars);
        for (; mp_star_idx < n_stars; ++mp_star_idx) {
            for (; mp_planet_idx < STAR_MAX_PLANETS; ++mp_planet_idx) {
                Planet *p = stars[mp_star_idx].get_planet(mp_planet_idx);
                if (!(p && p->exists()))
                    continue;

                // TODO: SIt and SIu updates

                if (onein(7)) p->adjust_unrest(-1);
                if (onein(250)) p->surfchange();

                // TODO: Owned planet maintenance
            }
            mp_planet_idx = 0;
        }
        next_mp_stage();
    }

    if (mp_stage == MP_UpdateAlienFly) {
        // TODO
        next_mp_stage();
    }

    if (mp_stage == MP_AlienMissions) {
        // TODO - PROCdomission etc
        next_mp_stage();
    }

    if (mp_stage == MP_PayOfficers) {
        // TODO - PROCpayoff etc
        next_mp_stage();
    }

    if (mp_stage == MP_GuildCommendations) {
        // TODO - PROCnewtitle
        next_mp_stage();
    }

    if (mp_stage == MP_AlienExile) {
        // TODO - PROCcheckalive
        next_mp_stage();
    }

    if (mp_stage == MP_UpdateHumanFly) {
        for (; mp_player_idx < N_PLAYERS; ++mp_player_idx) {
            Player *p = exostate.set_active_player(mp_player_idx);
            if (p && p->is_active() && p->is_human()) {
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

    if (mp_stage == MP_End) {
        // When we decide we're done with updates...
        month_pass_end();
        return ExodusMode::MODE_None;
    }

    return ExodusMode::MODE_None;
}
