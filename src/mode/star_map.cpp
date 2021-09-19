#include "star_map.h"

#include <cmath>

#include "galaxy/star.h"

#include "assetpaths.h"

const float PLANET_ROTATE_SPD = .1f;

const int FLEET_PANEL_W = 240;
const int FLEET_PANEL_H = 308;
const int FLEET_PANEL_X = RES_X/2 - FLEET_PANEL_W/2;
const int FLEET_PANEL_Y = RES_Y/2 - FLEET_PANEL_H/2 - 40;

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
    END,
};

StarMap::StarMap() : ModeBase("StarMap"), PanelDrawer(PNL_Star), CommPanelDrawer() {
    stage = SM_Idle;
    for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
        planet_progress[i] = 0;
    }

}

void StarMap::enter() {
    ModeBase::enter(ID::END);
    star = exostate.get_active_star();

    DrawTarget tgt = TGT_Primary;

    draw_manager.draw(tgt, IMG_MAP_SOLAR);
    draw_panel_bg(tgt);
    draw_planets(0.f);
    draw_manager.save_background();
    draw_manager.show_cursor(true);

    Planet *active_planet = exostate.get_active_planet();
    if (active_planet && active_planet->exists()) {
        // Ensures fleet button is updated etc
        select_planet(exostate.get_active_planet_idx());
    } else {
        for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
            if (select_planet(i))
                break;
        }
    }

    if (tgt != TGT_Primary) {
        draw_manager.fade_start(1.f, 12);
    }

    audio_manager.target_music(ephstate.default_music);

    stage = SM_Idle;
}

void StarMap::exit() {
    for (ID _id = FLEET_SCOUT; _id <= FLEET_EXIT; _id = (ID)((int)_id + 1)) {
        draw_manager.unset_selectable(id(_id));
    }

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

    switch(stage) {
        case SM_Idle:
            draw_planets(delta);

            update_panel_info_player(TGT_Primary, player);
            update_panel_info_planet(TGT_Primary, player, planet);

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
                        } else {
                            bool owned = planet->is_owned();

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
                        if (planet->is_owned()) {
                            if (planet->get_owner() == player_idx) {
                                // Comms with own planet
                                comm_open(DIA_S_PlanetComm);
                                stage = SM_PlanetComm;
                                return ExodusMode::MODE_None;
                            } else {
                                // Comms with enemy planet
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
                update_fleet_menu();

                // TODO

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
        case SM_PlanSettle:
            action = comm_update(delta);
            if (action == CA_Proceed) {
                draw_manager.fade_black(1.2f, 24);
                stage = SM_PlanSettleFade;
            } else if (action == CA_Abort) {
                comm_close();
                stage = SM_Idle;
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
                default:
                    L.fatal("Unexpected comm action in SM_PlanetComm: %d", (int)action);
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
                    ephstate.set_ephemeral_state(EPH_LunarBattlePrep);
                    ephstate.lunar_battle.aggressor_type = AGG_Player;
                    ephstate.lunar_battle.aggressor_idx = player_idx;
                    return ephstate.get_appropriate_mode();
                default:
                    L.fatal("Unexpected comm action in SM_PlanAttack: %d", (int)action);
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
            int draw_x = 140 + i*95;
            int draw_y = (RES_Y / 2) - 30 + ((i % 2) == 0 ? -30 : 30);
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

                // The original renders all planets at the same scale...
                if (planet->get_size() == PLANET_Small) planet_scale = 0.6;
                if (planet->get_size() == PLANET_Medium) planet_scale = 0.8;

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
                        Font::Tiny,
                        name,
                        Justify::Centre,
                        draw_x,
                        draw_y + 50,
                        col
                    );
                }
            }

            // TODO: Fleet markers
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

bool StarMap::select_planet(int index) {
    Planet *p = star->get_planet(index);
    if (p && p->exists()) {
        exostate.set_active_planet(index);
        set_fleet_button(!(p->is_owned() && exostate.get_active_player_idx() == p->get_owner()));
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
