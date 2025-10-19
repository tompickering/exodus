#include "star_map.h"

#include <cmath>

#include "galaxy/star.h"
#include "util/value.h"

#include "assetpaths.h"
#include "exodus_features.h"

const float PLANET_ROTATE_SPD = .1f;

const int FLEET_PANEL_W = 240;
const int FLEET_PANEL_H = 308;
const int FLEET_PANEL_X = RES_X/2 - FLEET_PANEL_W/2;
const int FLEET_PANEL_Y = RES_Y/2 - FLEET_PANEL_H/2 - 40;

const int RENAME_W = 120;
const int RENAME_H = 20;

enum ID {
    PLANET1,
    PLANET2,
    PLANET3,
    PLANET4,
    PLANET5,
    FLEET_BUTTON,
    FLEET_PANEL,
    FLEET_PANEL_PATTERN,
    FLEET_HEAD_SCOUT,
    FLEET_HEAD_BMB,
    FLEET_SCOUT,
    FLEET_COVERED_SCOUT,
    FLEET_BMB_CULT,
    FLEET_BMB_MINE,
    FLEET_BMB_PLU,
    FLEET_BMB_CITY,
    FLEET_BMB_ARMY,
    FLEET_BMB_PORT,
    FLEET_BMB_TRADE,
    FLEET_BMB_AIRDEF,
    FLEET_EXIT,
    FLEET_MISSIONBG,
    FESTIVAL,
    RENAME,
    RENAME_BORDER,
    RENAME_NEWNAME,
    END,
};

StarMap::StarMap() : ModeBase("StarMap"), PanelDrawer(PNL_Star), CommPanelDrawer() {
    stage = SM_Idle;
    for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
        planet_progress[i] = 0;
    }
    festival_delay = 0;
}

void StarMap::enter() {
    ModeBase::enter(ID::END);

    // If we're returning here having fought a battle, clear ephemeral state
    if (ephstate.get_ephemeral_state() == EPH_LunarBattleReport) {
        ephstate.clear_ephemeral_state();
    }

    star = exostate.get_active_star();

    DrawTarget tgt = TGT_Primary;

    const char *bg = IMG_MAP_SOLAR;

    switch (star->get_size()) {
        case STAR_Expand1:
            bg = IMG_MAP_SOLAR_SN1;
            break;
        case STAR_Expand2:
            bg = IMG_MAP_SOLAR_SN2;
            break;
        case STAR_Expand3:
            bg = IMG_MAP_SOLAR_SN3;
            break;
        case STAR_Expand4:
            bg = IMG_MAP_SOLAR_SN4;
            break;
        case STAR_Dwarf:
            bg = IMG_MAP_SOLAR_SN5;
            break;
        default:
            break;
    }

    draw_manager.draw(tgt, bg);

    draw_panel_bg(tgt);
    draw_planets(0.f);
    draw_manager.save_background();
    draw_manager.show_cursor(true);

    bool planet_set = false;
    Planet *active_planet = exostate.get_active_planet();
    if (active_planet && active_planet->exists()) {
        // Ensures fleet button is updated etc
        planet_set = select_planet(exostate.get_active_planet_idx());
    }
    if (!planet_set) {
        for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
            if (select_planet(i))
                break;
        }
    }

    festival_delay = 0;

    lookagain = false;

    for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
        id_planet_names[i] = draw_manager.new_sprite_id();
        id_fleet_markers[i] = draw_manager.new_sprite_id();
    }

    if (tgt != TGT_Primary) {
        draw_manager.fade_start(1.f, 12);
    }

    DrawArea a = {FLEET_PANEL_X, FLEET_PANEL_Y, FLEET_PANEL_W, FLEET_PANEL_H};
    draw_manager.set_source_region(id(ID::FLEET_MISSIONBG), &a);

    audio_manager.target_music(ephstate.default_music);

    if (ephstate.get_ephemeral_state() == EPH_SelectPlanet) {
        stage = SM_SelectPlanet;
    } else {
        stage = SM_Idle;
    }
}

void StarMap::exit() {
    draw_manager.set_source_region(id(ID::FLEET_MISSIONBG), nullptr);
    for (ID _id = FLEET_SCOUT; _id <= FLEET_EXIT; _id = (ID)((int)_id + 1)) {
        draw_manager.unset_selectable(id(_id));
    }

    for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
        draw_manager.draw(id_planet_names[i], nullptr);
        draw_manager.release_sprite_id(id_planet_names[i]);
        draw_manager.draw(id_fleet_markers[i], nullptr);
        draw_manager.release_sprite_id(id_fleet_markers[i]);
    }

    comm_ensure_closed();

    ModeBase::exit();
}

ExodusMode StarMap::update(float delta) {
    SpriteClick click;
    CommAction action;

    if (draw_manager.fade_active()) {
        return ExodusMode::MODE_None;
    }

    Player *player = exostate.get_active_player();
    Planet *planet = exostate.get_active_planet();
    int player_idx = exostate.get_active_player_idx();

    switch (stage) {
        case SM_Idle:
            if (input_manager.consume(K_Space)) {
                ephstate.set_ephemeral_state(EPH_MonthPass);
                return ephstate.get_appropriate_mode();
            }

            draw_planets(delta);

            update_panel_info_player(TGT_Primary, player);

            if (ephstate.get_ephemeral_state() == EPH_PostPlanet) {
                stage = SM_HandlePostPlanet;
                return ExodusMode::MODE_None;
            }

            // We have returned from planet probe view - resume dialogue
            if (ephstate.get_ephemeral_state() == EPH_ProbePlanet) {
                ephstate.clear_ephemeral_state();
                comm_open(DIA_S_PlanProbe);
                stage = SM_PlanSettle;
                return ExodusMode::MODE_None;
            }

            for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
                if (draw_manager.query_click(id_planet_names[i]).id) {
                    Planet *p = star->get_planet(i);
                    if (p->exists() && p->is_owned() && p->get_owner() == player_idx) {
                        rename_planet = i;
                        int x, y;
                        get_planet_draw_pos(i, x, y);
                        draw_manager.fill(
                            id(ID::RENAME_BORDER),
                            {x-RENAME_W/2-BORDER, y+50-BORDER,
                             RENAME_W+2*BORDER, RENAME_H+2*BORDER},
                            COL_BORDERS);
                        draw_manager.fill(
                            id(ID::RENAME),
                            {x-RENAME_W/2, y+50,
                             RENAME_W, RENAME_H},
                            {0, 0, 0});
                        input_manager.start_text_input();
                        input_manager.set_input_text(p->get_name());
                        stage = SM_PlanetRename;
                        return ExodusMode::MODE_None;
                    }
                }
            }

            for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
                if (draw_manager.query_click(id(ID::PLANET1 + i)).id) {
                    select_planet(i);
                }
            }

            click = draw_manager.query_click(id_panel);
            if (click.id) {
                if (click.x < 0.25) {
                    // Fleet / Map
                    if (planet) {
                        if (planet->get_owner() == player_idx) {
                            return ExodusMode::MODE_PlanetMap;
                        } else if (exostate.get_active_star_idx() != player->get_location().get_target()) {
                            comm_open(DIA_S_FleetNotInSystem);
                            stage = SM_Counsellor;
                            return ExodusMode::MODE_None;
                        } else {
                            bool owned = planet->is_owned();

#if FEATURE_BOMBING_LIMIT_HUMAN
                            if (exostate.bombing_prevented(planet)) {
                                comm_open(DIA_S_AlreadyBombed);
                                stage = SM_Counsellor;
                                return ExodusMode::MODE_None;
                            }
#endif

                            draw_manager.fill(
                                id(ID::FLEET_PANEL),
                                {FLEET_PANEL_X-BORDER, FLEET_PANEL_Y-BORDER,
                                FLEET_PANEL_W+2*BORDER, FLEET_PANEL_H+2*BORDER},
                                COL_BORDERS);

                            draw_manager.fill_pattern(
                                id(ID::FLEET_PANEL_PATTERN),
                                {FLEET_PANEL_X, FLEET_PANEL_Y, FLEET_PANEL_W, FLEET_PANEL_H});

                            for (ID _id = FLEET_SCOUT; _id <= FLEET_EXIT; _id = (ID)((int)_id + 1)) {
                                if (owned || _id < FLEET_BMB_CULT || _id > FLEET_BMB_AIRDEF) {
                                    draw_manager.set_selectable(id(_id));
                                }
                            }

                            stage = SM_Fleet;
                            return ExodusMode::MODE_None;
                        }
                    }
                } else if (click.x < 0.5) {
                    // Info
                    if (planet && planet->exists()) {
                        return ExodusMode::MODE_PlanetStatus;
                    }
                } else if (click.x < 0.75) {
                    // Comm
                    if (planet && planet->exists()) {
                        PlayerLocation &loc = player->get_location();
                        if (loc.in_flight() || loc.get_target() != exostate.get_active_star_idx()) {
                            panel_set_text("Your fleet is not in this system.");
                            return ExodusMode::MODE_None;
                        }


                        if (player->get_starship().pct_damage_comms > 50) {
                            comm_open(DIA_S_CommsBroken);
                            stage = SM_PlanetComm;
                            return ExodusMode::MODE_None;
                        }

                        if (planet->is_owned()) {
                            int owner_idx = planet->get_owner();
                            Player *owner = exostate.get_player(owner_idx);
                            if (owner_idx == player_idx) {
                                // Comms with own planet
                                comm_open(DIA_S_PlanetComm);
                                stage = SM_PlanetComm;
                                return ExodusMode::MODE_None;
                            } else {
                                // Comms with enemy planet
                                if (exostate.is_allied(player_idx, owner_idx)) {
                                    audio_manager.target_music(mpart2mus(13));
                                } else if (owner->is_hostile_to(player_idx)) {
                                    audio_manager.target_music(mpart2mus(8));
                                } else {
                                    audio_manager.target_music(mpart2mus(4));
                                }
                                comm_open(DIA_S_HailPlanet);
                                stage = SM_EnemyComm;
                                return ExodusMode::MODE_None;
                            }
                        } else {
                            comm_open(DIA_S_PlanSettle);
                            stage = SM_PlanSettle;
                            return ExodusMode::MODE_None;
                        }
                    }
                } else {
                    // Back
                    draw_manager.fade_black(1.2f, 24);
                    stage = SM_Back2Gal;
                }
            }
            break;
        case SM_Fleet:
            {
                if (comm_is_open()) {
                    action = comm_update(delta);
                    if (action == CA_Abort) {
                        comm_close();
                    }
                    return ExodusMode::MODE_None;
                }

                update_fleet_menu();

                Fleet &fleet = player->get_fleet_nonconst();
                const char* mission_bg = planet->sprites()->landscape;

                bool owned = planet->is_owned();

                bool scout = false;
                bool scout_covered = false;

                if (draw_manager.query_click(id(ID::FLEET_SCOUT)).id) {
                    scout = true;
                    scout_covered = false;
                }

                if (draw_manager.query_click(id(ID::FLEET_COVERED_SCOUT)).id) {
                    scout = true;
                    scout_covered = true;
                }

                if (scout) {
                    if (fleet.scouts <= 0) {
                        comm_open(DIA_S_NoScouts);
                        return ExodusMode::MODE_None;
                    } else if (scout_covered && fleet.bombers <= 0) {
                        // This dialogue not present in orig
                        comm_open(DIA_S_NoCoverBombers);
                        return ExodusMode::MODE_None;
                    } else {
                        if (!owned) {
                            ephstate.set_ephemeral_state(EPH_ScoutPlanet);
                            return ExodusMode::MODE_PlanetMap;
                        }

                        int owner = planet->get_owner();
                        exostate.unset_alliances(player_idx, owner);

                        int guns = planet->get_airdef_guns();
                        int def = 0;
                        for (int i = 0; i < guns; ++i) {
                            if (onein(4)) ++def;
                        }

                        L.debug("%d guns provide %d defence", guns, def);

                        int scouts_killed = 0;
                        int bombers_killed = 0;

                        int att = 0;
                        if (scout_covered) {
                            for (int i = 0; i < fleet.bombers; ++i) {
                                if (onein(25)) ++att;
                            }

                            int d = def/2;
                            scouts_killed = min(d, 20);
                            scouts_killed = min(scouts_killed, fleet.scouts);
                            bombers_killed = min(fleet.bombers, d);
                            att = min(att, guns);
                        } else {
                            scouts_killed = min(def, fleet.scouts);
                        }

                        planet->adjust_airdef_guns(-att);
                        fleet.scouts = max(fleet.scouts - scouts_killed, 0);
                        fleet.bombers = max(fleet.bombers - bombers_killed, 0);

                        draw_manager.draw(
                            id(ID::FLEET_MISSIONBG),
                            mission_bg,
                            {FLEET_PANEL_X, FLEET_PANEL_Y,
                             0, 0, 1, 1});

                        draw_manager.draw_text(
                            "SCOUT FLIGHT",
                            Justify::Left,
                            FLEET_PANEL_X+4, FLEET_PANEL_Y+4,
                            COL_TEXT2);

                        draw_manager.draw_text(
                            "The AirDef guns have",
                            Justify::Left,
                            FLEET_PANEL_X+4, FLEET_PANEL_Y+44,
                            COL_TEXT);

                        char text[64];

                        snprintf(text, sizeof(text), "hit %d scouts.", scouts_killed);
                        draw_manager.draw_text(
                            text,
                            Justify::Left,
                            FLEET_PANEL_X+4, FLEET_PANEL_Y+64,
                            COL_TEXT);

                        if (scout_covered) {
                            snprintf(text, sizeof(text), "%d bombers have hit", att);
                            draw_manager.draw_text(
                                text,
                                Justify::Left,
                                FLEET_PANEL_X+4, FLEET_PANEL_Y+104,
                                COL_TEXT);
                            draw_manager.draw_text(
                                "a target.",
                                Justify::Left,
                                FLEET_PANEL_X+4, FLEET_PANEL_Y+124,
                                COL_TEXT);
                            snprintf(text, sizeof(text), "%d bombers have been", bombers_killed);
                            draw_manager.draw_text(
                                text,
                                Justify::Left,
                                FLEET_PANEL_X+4, FLEET_PANEL_Y+164,
                                COL_TEXT);
                            draw_manager.draw_text(
                                "destroyed.",
                                Justify::Left,
                                FLEET_PANEL_X+4, FLEET_PANEL_Y+184,
                                COL_TEXT);
                        }

                        const char* t = "No scouts survived.";
                        if (fleet.scouts > 0) {
                            t = "Visual data follows.";
                        }
                        draw_manager.draw_text(
                            t,
                            Justify::Left,
                            FLEET_PANEL_X+4, FLEET_PANEL_Y+224,
                            COL_TEXT);

                        stage = SM_MissionScout;
                        return ExodusMode::MODE_None;
                    }
                }

                bool bmb = false;
                bool bmb_guns = false;
                StoneSet tgt;
                if (draw_manager.query_click(id(ID::FLEET_BMB_CULT)).id)   { bmb=true; tgt.add(STONE_Agri); }
                if (draw_manager.query_click(id(ID::FLEET_BMB_MINE)).id)   { bmb=true; tgt.add(STONE_Mine); }
                if (draw_manager.query_click(id(ID::FLEET_BMB_PLU)).id)    { bmb=true; tgt.add(STONE_Plu); }
                if (draw_manager.query_click(id(ID::FLEET_BMB_CITY)).id)   { bmb=true; tgt.add(STONE_City); }
                if (draw_manager.query_click(id(ID::FLEET_BMB_ARMY)).id)   { bmb=true; tgt.add(STONE_Inf);
                                                                                       tgt.add(STONE_Gli);
                                                                                       tgt.add(STONE_Art); }
                if (draw_manager.query_click(id(ID::FLEET_BMB_PORT)).id)   { bmb=true; tgt.add(STONE_Port0);
                                                                                       tgt.add(STONE_Port1);
                                                                                       tgt.add(STONE_Port2); }
                if (draw_manager.query_click(id(ID::FLEET_BMB_TRADE)).id)  { bmb=true; tgt.add(STONE_Trade); }
                if (draw_manager.query_click(id(ID::FLEET_BMB_AIRDEF)).id) { bmb=true; bmb_guns=true; }

                if (bmb) {
                    if (fleet.bombers <= 0) {
                        comm_open(DIA_S_NoBombers);
                        return ExodusMode::MODE_None;
                    } else {
                        /*
                         * This is a little out of order compared to the original,
                         * which finishes PROCflybomb first (from PROCbombplan) and
                         * then adjusts unrest, sets punish and trace etc.
                         */
                        planet->adjust_unrest(1);

                        // punish%(1,2)=1
                        player->commit_infraction(INF_BombAttack);

                        player->add_trace(TRACE_PlanetsBombed);

                        // N.B. Sets up ephstate
                        int hits, bombers_killed;
                        bomb_planet(tgt, bmb_guns, hits, bombers_killed);

                        draw_manager.draw(
                            id(ID::FLEET_MISSIONBG),
                            mission_bg,
                            {FLEET_PANEL_X, FLEET_PANEL_Y,
                             0, 0, 1, 1});

                        draw_manager.draw_text(
                            "The attack has begun.",
                            Justify::Left,
                            FLEET_PANEL_X+4, FLEET_PANEL_Y+4,
                            COL_TEXT2);

                        draw_manager.draw_text(
                            "The AirDef guns have",
                            Justify::Left,
                            FLEET_PANEL_X+4, FLEET_PANEL_Y+44,
                            COL_TEXT);

                        char text[64];

                        snprintf(text, sizeof(text), "hit %d bombers.", bombers_killed);
                        draw_manager.draw_text(
                            text,
                            Justify::Left,
                            FLEET_PANEL_X+4, FLEET_PANEL_Y+64,
                            COL_TEXT);

                        snprintf(text, sizeof(text), "%d bombers have hit", hits);
                        draw_manager.draw_text(
                            text,
                            Justify::Left,
                            FLEET_PANEL_X+4, FLEET_PANEL_Y+104,
                            COL_TEXT);
                        draw_manager.draw_text(
                            "a target.",
                            Justify::Left,
                            FLEET_PANEL_X+4, FLEET_PANEL_Y+124,
                            COL_TEXT);

                        draw_manager.draw_text(
                            "Visual data follows.",
                            Justify::Left,
                            FLEET_PANEL_X+4, FLEET_PANEL_Y+224,
                            COL_TEXT);

                        if (hits > 0) {
                            achievement_manager.unlock(ACH_BombingMission);
                        }

                        stage = SM_MissionBomb;
                        return ExodusMode::MODE_None;
                    }
                }

                if (draw_manager.query_click(id(ID::FLEET_EXIT)).id) {
                    for (ID _id = FLEET_SCOUT; _id <= FLEET_EXIT; _id = (ID)((int)_id + 1)) {
                        draw_manager.unset_selectable(id(_id));
                    }
                    for (ID _id = FLEET_SCOUT; _id <= FLEET_EXIT; _id = (ID)((int)_id + 1)) {
                        draw_manager.draw(id(_id), nullptr);
                        draw_manager.draw(id(ID::FLEET_PANEL_PATTERN), nullptr);
                        draw_manager.draw(id(ID::FLEET_PANEL), nullptr);
                    }
                    stage = SM_Idle;
                }
            }
            break;
        case SM_MissionScout:
            {
                if (draw_manager.clicked()) {
                    if (planet->is_owned()) {
                        exostate.prevent_bombing(planet);
                    }
                    const Fleet &fleet = player->get_fleet();
                    if (fleet.scouts) {
                        ephstate.set_ephemeral_state(EPH_ScoutPlanet);
                        return ExodusMode::MODE_PlanetMap;
                    } else {
                        draw_manager.draw(id(ID::FLEET_MISSIONBG), nullptr);
                        stage = SM_Fleet;
                        return ExodusMode::MODE_None;
                    }
                }
            }
            break;
        case SM_MissionBomb:
            {
                if (draw_manager.clicked()) {
                    if (planet->is_owned()) {
                        exostate.prevent_bombing(planet);
                    }
                    return ephstate.get_appropriate_mode();
                }
            }
            break;
        case SM_PlanSettle:
            action = comm_update(delta);
            if (action == CA_Proceed) {
                draw_manager.fade_black(1.2f, 24);
                stage = SM_PlanSettleFade;
            } else if (action == CA_Abort) {
                comm_close();
                stage = SM_Idle;
            } else if (action == CA_BionicProbe) {
                comm_close();
                ephstate.set_ephemeral_state(EPH_ProbePlanet);
                return ephstate.get_appropriate_mode();
            }
            break;
        case SM_PlanSettleFade:
            if (!draw_manager.fade_active()) {
                return ExodusMode::MODE_PlanetColonise;
            }
            break;
        case SM_PlanetComm:
            action = comm_update(delta);
            switch (action) {
                case CA_None:
                    break;
                case CA_Abort:
                    comm_close();
                    stage = SM_Idle;
                    break;
                case CA_GoodsTransfer:
                    comm_close();
                    return ExodusMode::MODE_PlanetTransfer;
                case CA_StartProduction:
                    comm_close();
                    return ExodusMode::MODE_FleetProduction;
                case CA_ChangeClimate:
                    comm_close();
                    planet->changeclimate();
                    return ExodusMode::MODE_Reload;
                case CA_MovePlanet:
                    comm_close();
                    ephstate.set_ephemeral_state(EPH_MovePlanet);
                    return ephstate.get_appropriate_mode();
                default:
                    L.error("Unexpected comm action in SM_PlanetComm: %d", (int)action);
            }
            break;
        case SM_EnemyComm:
            action = comm_update(delta);
            switch (action) {
                case CA_None:
                    break;
                case CA_Abort:
                    comm_close();
                    stage = SM_Idle;
                    break;
                case CA_PlanAttack:
                    comm_close();
                    comm_open(DIA_S_PlanAttack);
                    stage = SM_PlanAttack;
                    break;
                case CA_Trade:
                    comm_close();
                    return ExodusMode::MODE_Trade;
                default:
                    L.fatal("Unexpected comm action in SM_EnemyComm: %d", (int)action);
            }
            break;
        case SM_PlanAttack:
            action = comm_update(delta);
            switch (action) {
                case CA_None:
                    break;
                case CA_Abort:
                    comm_close();
                    stage = SM_Idle;
                    break;
                case CA_Proceed:
                    comm_close();
                    player->add_trace(TRACE_BattlesStarted);
                    ephstate.set_ephemeral_state(EPH_LunarBattlePrep);
                    ephstate.lunar_battle.aggressor_type = AGG_Player;
                    ephstate.lunar_battle.aggressor_idx = player_idx;
                    ephstate.lunar_battle.aggressor_odds = comm_ctx.att_odds;
                    return ephstate.get_appropriate_mode();
                default:
                    L.fatal("Unexpected comm action in SM_PlanAttack: %d", (int)action);
            }
            break;
        case SM_FestivalDelay:
            {
                if (festival_delay > .8f) {
                    festival_delay = 0;
                    draw_manager.draw(
                        id(ID::FESTIVAL),
                        IMG_CT4_EXPORT,
                        {5, 7, 0, 0, 1, 1});
                    frame_draw();
                    char text[24 + PLANET_MAX_NAME];
                    snprintf(text, sizeof(text), "The celebrations of %s", planet->get_name());
                    draw_manager.draw_text(
                        text,
                        Justify::Centre,
                        RES_X/2, 10,
                        COL_TEXT2);
                    audio_manager.target_music(MUS_CELEBRATE);
                    stage = SM_Festival;
                    return ExodusMode::MODE_None;
                }

                festival_delay += delta;
            }
            break;
        case SM_Festival:
            {
                if (draw_manager.clicked()) {
                    draw_manager.draw(id(ID::FESTIVAL), nullptr);
                    frame_remove();
                    audio_manager.target_music(ephstate.default_music);
                    stage = SM_Idle;
                }
            }
            break;
        case SM_SelectPlanet:
            {
                if (input_manager.consume(K_Escape) || draw_manager.clicked_r()) {
                    // Go back to galaxy map - still in planet select mode
                    return ExodusMode::MODE_Pop;
                }

                panel_set_text("SELECT A PLANET");

                // TODO: Should we allow you to select unowned planets? Likely needs ephstate config
                for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
                    if (draw_manager.query_click(id(ID::PLANET1 + i)).id) {
                        if (ephstate.selectplanet_mc <= 0 || player->attempt_spend(ephstate.selectplanet_mc)) {
                            if (ephstate.selectplanet_trace != TRACE_None) {
                                player->add_trace(ephstate.selectplanet_trace);
                            }
                            if (ephstate.selectplanet_mission != MT_None) {
                                player->set_mission_type(ephstate.selectplanet_mission);
                            }
                            *(ephstate.selectplanet_planet) = i;
                            draw_manager.fade_black(1.2f, 24);
                            stage = SM_SelectPlanetFadeOut;
                            return ExodusMode::MODE_None;
                        } else {
                            L.error("Player cannot afford selectplanet MC - should not have gotten this far");
                        }
                    }
                }
            }
            break;
        case SM_SelectPlanetFadeOut:
            {
                if (!draw_manager.fade_active()) {
                    // Clear EPH_SelectPlanet and set up further ephemeral state if necessary
                    return ephstate.selectplanet_resolve();
                }
            }
            break;
        case SM_HandlePostPlanet:
            if (ephstate.consume_postplanet(PPA_BadLaws)) {
                comm_open(DIA_S_LookAgainBadLaws);
                stage = SM_HandlePostPlanetComms;
                return ExodusMode::MODE_None;
            }
            if (ephstate.consume_postplanet(PPA_NoEssentials)) {
                comm_open(DIA_S_LookAgainNoEssentials);
                stage = SM_HandlePostPlanetComms;
                return ExodusMode::MODE_None;
            }

            if (lookagain) {
                return ExodusMode::MODE_PlanetMap;
            }

            if (ephstate.consume_postplanet(PPA_Festival)) {
                stage = SM_FestivalDelay;
                return ExodusMode::MODE_None;
            }

            // All postplanet actions consumed - we can clear state now
            ephstate.clear_ephemeral_state();

            stage = SM_Idle;
            return ExodusMode::MODE_None;
        case SM_HandlePostPlanetComms:
            switch (comm_update(delta)) {
                case CA_Proceed:
                    lookagain = true;
                    // Fallthrough...
                case CA_Abort:
                    comm_close();
                    stage = SM_HandlePostPlanet;
                    break;
                default:
                    break;

            }
            break;
        case SM_Counsellor:
            if (comm_update(delta) != CA_None) {
                comm_close();
                stage = SM_Idle;
            }
            break;
        case SM_PlanetRename:
            {
                const char* newname = input_manager.get_input_text(PLANET_MAX_NAME);

                int x, y;
                get_planet_draw_pos(rename_planet, x, y);

                draw_manager.draw_text(
                    id(ID::RENAME_NEWNAME),
                    newname,
                    Justify::Left,
                    x - RENAME_W/2 + 4,
                    y + 50,
                    COL_TEXT
                );

                if (input_manager.consume(K_Backspace)) {
                    input_manager.backspace();
                }

                if (input_manager.consume(K_Enter)) {
                    Planet *p = star->get_planet(rename_planet);
                    p->set_name(newname);
                    input_manager.stop_text_input();
                    draw_manager.draw(id(ID::RENAME_NEWNAME), nullptr);
                    draw_manager.draw(id(ID::RENAME), nullptr);
                    draw_manager.draw(id(ID::RENAME_BORDER), nullptr);
                    return ExodusMode::MODE_Reload;
                }
            }
            break;
        case SM_Back2Gal:
            return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}

void StarMap::draw_planets(float delta) {
    for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
        Planet *planet = star->get_planet_nocheck(i);
        if (planet && planet->exists()) {
            bool active = planet == exostate.get_active_planet();
            int draw_x, draw_y;
            get_planet_draw_pos(i, draw_x, draw_y);
            if (delta == 0 || active) {
                if (active) {
                    int p = exostate.get_active_planet_idx();
                    planet_progress[p] += delta * PLANET_ROTATE_SPD;
                    planet_progress[p] = fmod(planet_progress[p], 1.f);
                }

                int x_off = 76 * (int)(90 * planet_progress[i]);
                int y_off = 74 * (int)planet->get_class();
                DrawArea a = {x_off, y_off, 76, 74};
                draw_manager.set_source_region(id(ID::PLANET1 + i), &a);

                float planet_scale = 1.f;

#if FEATURE_MULTISIZE_PLANETS
                // The original renders all planets at the same scale...
                if (planet->get_size() == PLANET_Small) planet_scale = 0.6;
                if (planet->get_size() == PLANET_Medium) planet_scale = 0.8;
#endif

                draw_manager.draw(
                    id(ID::PLANET1 + i),
                    IMG_PA_ROT,
                    {draw_x,
                     draw_y,
                     .5f, .5f,
                     planet_scale, planet_scale});
            }

            if (delta == 0) {
                const char *name = planet->get_name();
                if (strnlen(name, 1)) {
                    // The original doesn't show names of named planets if they have
                    // no owner at the time - but the name is still visible in the
                    // status screen etc - we show the name in grey for clarity.
                    RGB col = COL_TEXT_GREYED;
                    if (planet->is_owned()) {
                        bool enemy = planet->is_owned() &&
                            planet->get_owner() != exostate.get_active_player_idx();
                        if (enemy) {
                            col = COL_TEXT_BAD;
                        } else {
                            col = COL_TEXT;
                        }
                    }
                    draw_manager.draw_text(
                        id_planet_names[i],
                        Font::Tiny,
                        name,
                        Justify::Centre,
                        draw_x,
                        draw_y + 50,
                        col
                    );
                }
            }

            // Fleet markers
            int s_idx = exostate.tgt2loc(exostate.get_active_star());
            if (planet->is_owned()) {
                uint32_t drawn_fleets = 0;

                int owner_idx = planet->get_owner();
                Player *owner = exostate.get_player(owner_idx);

                if (!(drawn_fleets & (1 << owner_idx))) {
                    PlayerLocation &owner_loc = owner->get_location();
                    if (!owner_loc.in_flight() && owner_loc.get_target() == s_idx) {
                        drawn_fleets |= (1 << owner_idx);

                        draw_manager.draw(
                            id_fleet_markers[i],
                            IMG_TS1_SHICON,
                            {draw_x,
                             draw_y + 100,
                             .5f, .5f, 1, 1});
                    }
                }
            }
        } else if (planet && planet->get_construction_phase() > 0) {
            int draw_x = 140 + i*95;
            int draw_y = (RES_Y / 2) - 30 + ((i % 2) == 0 ? -30 : 30);
            const char *img = IMG_PP1_CONST;
            if (planet->get_construction_phase() > 1) {
                img = IMG_PP2_CONST;
            }
            draw_manager.draw(
                id(ID::PLANET1 + i),
                img,
                {draw_x,
                 draw_y,
                 .5f, .5f,
                 .5f, .5f});
        }
    }
}

void StarMap::get_planet_draw_pos(int i, int& x, int& y) {
    x = 140 + i*95;
    y = (RES_Y / 2) - 30 + ((i % 2) == 0 ? -30 : 30);
}

bool StarMap::select_planet(int index) {
    Planet *p = star->get_planet(index);
    if (p && p->exists()) {
        exostate.set_active_planet(index);
        set_fleet_button(!(p->is_owned() && exostate.get_active_player_idx() == p->get_owner()));
        update_panel_info_planet(TGT_Primary, exostate.get_active_player(), p);
        return true;
    }
    return false;
}

void StarMap::set_fleet_button(bool on) {
    // FIXME: The panel doesn't look like the original game
    // All 4 buttons look drawn independently in the original,
    // but I can't see any alternative sprites for the other 3
    // (which look more faded in this version).
    draw_manager.draw(
        id(ID::FLEET_BUTTON),
        on ? IMG_TS2_M2_1 : IMG_TS2_M2_2,
        {206, RES_Y - 6,
         0, 1, 1, 1});
}

void StarMap::update_fleet_menu() {
    Planet *planet = exostate.get_active_planet();
    bool owned = planet->is_owned();

    draw_manager.draw_text(
        id(ID::FLEET_HEAD_SCOUT),
        "Scout flights:",
        Justify::Left,
        FLEET_PANEL_X+4, FLEET_PANEL_Y+4,
        COL_TEXT2);
    draw_manager.draw_text(
        id(ID::FLEET_SCOUT),
        "Scout flight",
        Justify::Left,
        FLEET_PANEL_X+4, FLEET_PANEL_Y+24,
        COL_TEXT);
    draw_manager.draw_text(
        id(ID::FLEET_COVERED_SCOUT),
        "Covered scout flight",
        Justify::Left,
        FLEET_PANEL_X+4, FLEET_PANEL_Y+44,
        COL_TEXT);

    RGB col = COL_TEXT;
    if (!owned) {
        col = COL_TEXT_GREYED;
    }

    draw_manager.draw_text(
        id(ID::FLEET_HEAD_BMB),
        "Bomber attack:",
        Justify::Left,
        FLEET_PANEL_X+4, FLEET_PANEL_Y+84,
        COL_TEXT2);
    draw_manager.draw_text(
        id(ID::FLEET_BMB_CULT),
        "Cultivated area",
        Justify::Left,
        FLEET_PANEL_X+4, FLEET_PANEL_Y+104,
        col);
    draw_manager.draw_text(
        id(ID::FLEET_BMB_MINE),
        "Mining",
        Justify::Left,
        FLEET_PANEL_X+4, FLEET_PANEL_Y+124,
        col);
    draw_manager.draw_text(
        id(ID::FLEET_BMB_PLU),
        "Plutonium production",
        Justify::Left,
        FLEET_PANEL_X+4, FLEET_PANEL_Y+144,
        col);
    draw_manager.draw_text(
        id(ID::FLEET_BMB_CITY),
        "Cities",
        Justify::Left,
        FLEET_PANEL_X+4, FLEET_PANEL_Y+164,
        col);
    draw_manager.draw_text(
        id(ID::FLEET_BMB_ARMY),
        "Army production",
        Justify::Left,
        FLEET_PANEL_X+4, FLEET_PANEL_Y+184,
        col);
    draw_manager.draw_text(
        id(ID::FLEET_BMB_PORT),
        "Spaceport",
        Justify::Left,
        FLEET_PANEL_X+4, FLEET_PANEL_Y+204,
        col);
    draw_manager.draw_text(
        id(ID::FLEET_BMB_TRADE),
        "Trading Centre",
        Justify::Left,
        FLEET_PANEL_X+4, FLEET_PANEL_Y+224,
        col);
    draw_manager.draw_text(
        id(ID::FLEET_BMB_AIRDEF),
        "AirDef Guns",
        Justify::Left,
        FLEET_PANEL_X+4, FLEET_PANEL_Y+244,
        col);

    draw_manager.draw_text(
        id(ID::FLEET_EXIT),
        "Exit",
        Justify::Left,
        FLEET_PANEL_X+4, FLEET_PANEL_Y+284,
        COL_TEXT);
}

bool StarMap::should_push_to_stack() {
    return ephstate.get_ephemeral_state() != EPH_SelectPlanet;
}
