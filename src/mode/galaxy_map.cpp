#include "galaxy_map.h"

#include "assetpaths.h"

#include "util/str.h"

#define BLINK_TIME 0.5

enum ID {
    SELECTED,
    FLEET_MARKER,
    MONTH_PASSING,
    END,
};

GalaxyMap::GalaxyMap() : ModeBase("GalaxyMap"), GalaxyDrawer(), PanelDrawer(PNL_Galaxy), CommPanelDrawer(), BulletinDrawer() {
    stage = GM_SwapIn;
    selected_ft = nullptr;
    selected_ft_blink = 0;
    mp_stage = MP_None;
    mpp_stage = (MonthPassPlanetStage)0;
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
                if (bulletin_is_open()) {
                    bulletin_update(delta);
                    if (!bulletin_acknowledged()) {
                        break;
                    }
                }
                ExodusMode next_mode = month_pass_update();
                month_pass_time += delta;
                if (mp_stage == MP_None) {
                    bulletin_ensure_closed();
                    // The only place we emerge from month-pass-specific stages...
                    stage = GM_Idle;
                    // This ensures that the screen is redrawn after the bulletin closes
                    return ExodusMode::MODE_Reload;
                }
                return next_mode;
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
    mp_stage = (MonthPassStage)((int)mp_stage + 1);
    L.debug("MP stage %d", mp_stage);
    // Reset all mp-stage state tracking
    mp_player_idx = 0;
    mp_star_idx = 0;
    mp_planet_idx = 0;
}

void GalaxyMap::next_mpp_stage() {
    mpp_stage = (MonthPassPlanetStage)((int)mpp_stage + 1);
    Star *s = exostate.get_active_star();
    Planet *p = exostate.get_active_planet();
    L.debug("[%s/%s] MPP stage %d", s->name, p->get_name(), mpp_stage);

}

void GalaxyMap::month_pass_start() {
    L.info("Month passing...");
    month_pass_time = 0;
    mpp_stage = (MonthPassPlanetStage)0;

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
    mpp_stage = (MonthPassPlanetStage)0;

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

    unsigned int n_stars;
    Galaxy *gal = exostate.get_galaxy();
    Star *stars = gal->get_stars(n_stars);

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

    if (mp_stage == MP_PlanetBackgroundUpdate) {
        for (; mp_star_idx < n_stars; ++mp_star_idx) {
            for (; mp_planet_idx < STAR_MAX_PLANETS; ++mp_planet_idx) {
                Planet *p = stars[mp_star_idx].get_planet(mp_planet_idx);
                if (!(p && p->exists()))
                    continue;

                p->month_reset();

                if (onein(7)) p->adjust_unrest(-1);

                if (p->is_owned()) {
                    if (onein(250)) p->surfchange();
                }

                // TODO: Owned planet maintenance
            }
            mp_planet_idx = 0;
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
    if (mp_stage == MP_PlanetMainUpdate) {
        for (; mp_star_idx < n_stars; ++mp_star_idx) {
            exostate.set_active_flytarget(&stars[mp_star_idx]);
            for (; mp_planet_idx < STAR_MAX_PLANETS; ++mp_planet_idx) {
                Planet *p = stars[mp_star_idx].get_planet(mp_planet_idx);
                // This only applies to owned planets
                // FIXME: Ensure that if we exit into another state, the planet *still
                // passes* this check next time around to finish the PROCcal_plan process!
                if (!(p && p->exists() && p->is_owned()))
                    continue;

                exostate.set_active_planet(mp_planet_idx);
                ExodusMode next_mode = month_pass_planet_update();

                if (mpp_stage != MPP_End) {
                    return next_mode;
                }

                mpp_stage = (MonthPassPlanetStage)0;
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

ExodusMode GalaxyMap::month_pass_planet_update() {
    Star *s = (Star*)exostate.get_active_flytarget();
    Planet *p  = exostate.get_active_planet();
    exostate.set_active_player(p->get_owner());
    Player *owner = exostate.get_player(p->get_owner());

    if (mpp_stage == MPP_ShuffleTrade) {
        if (onein(7)) p->randomise_trade_quality();
        next_mpp_stage();
    }

    if (mpp_stage == MPP_FirstCity) {
        if (!exostate.first_city_done) {
            if (owner->is_human() && p->count_stones(STONE_City) > 0) {
                // TODO - Bulletin image, music...
                exostate.first_city_done = true;
                bulletin_start_new(false);
                bulletin_set_flag(IMG_TS1_FLAG16);
                bulletin_set_text_col(COL_TEXT3);
                bulletin_vset_next_text("%s HAS BUILT THE FIRST CITY", tmp_caps(owner->get_name()));
                bulletin_vset_next_text("");
                bulletin_vset_next_text("Words cannot describe what has been");
                bulletin_vset_next_text("achieved by building the first city for the");
                bulletin_vset_next_text("homeless colonists.");
                bulletin_vset_next_text("");
                bulletin_vset_next_text("This might be the beginning of a new era.");
                bulletin_vset_next_text("");
                bulletin_vset_next_text("But beware. A city can also be a center");
                bulletin_vset_next_text("for system enemies.");
                bulletin_vset_next_text("");
                bulletin_vset_next_text("A picture of the new city follows.");
                next_mpp_stage();
                return ExodusMode::MODE_None;
            }
        }
        next_mpp_stage();
    }

    if (mpp_stage == MPP_FirstSpaceport) {
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

    if (mpp_stage == MPP_Research1) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_Meteors) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_KillArmies) {
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
                bulletin_vset_next_text("Defects at %s have caused", p->get_name());
                bulletin_set_next_text("some battle machines to be destructed.");
                // TODO: Should we tell the player how many have been destroyed?
                // TODO: News item (PROCdonotice)
                next_mpp_stage();
                return ExodusMode::MODE_None;
            }
        }
        next_mpp_stage();
    }

    if (mpp_stage == MPP_ClimateChange) {
        if (onein(250)) {
            const char* before = p->get_class_str_lower();
            p->surfchange();
            const char* after = p->get_class_str_lower();
            bulletin_start_new(true);
            bulletin_set_bg(p->sprites()->bulletin_bg);
            bulletin_set_active_player_flag();
            bulletin_write_planet_info(s, p);
            // TODO: Music
            bulletin_vset_next_text("GEOLOGICAL CHANGE AT %s", tmp_caps(p->get_name()));
            bulletin_vset_next_text("");
            bulletin_vset_next_text("Due to a global climate effect,");
            bulletin_vset_next_text("the planet's surface has changed from");
            bulletin_vset_next_text("%s into %s.", before, after);
            next_mpp_stage();
            return ExodusMode::MODE_None;
        }
        next_mpp_stage();
    }

    if (mpp_stage == MPP_Epidemic) {
        // PROCepidemic(1) case
        if (onein(150)) {
            if (exostate.get_month() >= 10 && !owner->has_invention(INV_Acid)) {
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
                bulletin_vset_next_text("PLAGUE AT %s", tmp_caps(p->get_name()));
                bulletin_set_next_text("");
                bulletin_set_next_text("Large parts of cultivated area");
                bulletin_set_next_text("have collapsed.");
                next_mpp_stage();
                return ExodusMode::MODE_None;
            }
        }
        next_mpp_stage();
    }

    if (mpp_stage == MPP_AlienAttack) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_Research2) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_DiscoverSpecies) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_CityEpidemic) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_RebelAttack) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_LawsIncreaseUnrest) {
        if (p->laws_cause_unrest()) p->adjust_unrest(1);
        next_mpp_stage();
    }

    if (mpp_stage == MPP_ParksReduceUnrest) {
        if (onein(2)) {
            int parks = p->count_stones(STONE_Park);
            int cities = p->count_stones(STONE_City);
            if (parks > cities/3) p->adjust_unrest(-1);
        }
        next_mpp_stage();
    }

    if (mpp_stage == MPP_ClearRadiation) {
        p->clear_radiation();
        next_mpp_stage();
    }

    if (mpp_stage == MPP_ReactorPollution) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_ClimateChangeDueToCultivation) {
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

    if (mpp_stage == MPP_ClimateChangeDueToDeforestation) {
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

    if (mpp_stage == MPP_LosePlanetControl) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_Income) {
        // TODO - Check the weird case in PROCcal_plan based on the t% DIM
        owner->give_mc(p->get_net_income());
        next_mpp_stage();
    }

    if (mpp_stage == MPP_ProducePlutonium) {
        p->produce_plutonium();
        next_mpp_stage();
    }

    if (mpp_stage == MPP_ProduceMilitary) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_Mining) {
        p->mine();
        next_mpp_stage();
    }

    if (mpp_stage == MPP_MilitaryFacilityShutdown) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_Trade) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_VillageGifts) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_FoodPerishes) {
        p->perish_food();
        next_mpp_stage();
    }

    if (mpp_stage == MPP_FoodProduction) {
        p->produce_food();
        next_mpp_stage();
    }

    if (mpp_stage == MPP_AgriCollapse) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_CityExpansion) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_VillageExpansion) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_ConsumeFood) {
        // TODO
        next_mpp_stage();
    }

    if (mpp_stage == MPP_AdvanceUnrest) {
        p->update_unrest_history();
        next_mpp_stage();
    }

    if (mpp_stage == MPP_AmendMilitaryFunding) {
        p->validate_army_funding();
        next_mpp_stage();
    }

    return ExodusMode::MODE_None;
}
