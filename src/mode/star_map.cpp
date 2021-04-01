#include "star_map.h"

#include <cmath>

#include "galaxy/star.h"

#include "assetpaths.h"

const float PLANET_ROTATE_SPD = .1f;

enum ID {
    PLANET1,
    PLANET2,
    PLANET3,
    PLANET4,
    PLANET5,
    FLEET_BUTTON,
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

    stage = SM_Idle;
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
                    // Map
                    if (exostate.get_active_planet()) {
                        return ExodusMode::MODE_PlanetMap;
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
        case SM_PlanSettle:
            action = comm_update(delta);
            if (action == CA_Proceed) {
                return ExodusMode::MODE_PlanetColonise;
            } else if (action == CA_Abort) {
                comm_close();
                stage = SM_Idle;
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
                    // TODO
                    comm_close();
                    stage = SM_Idle;
                    break;
                case CA_StartProduction:
                    // TODO
                    comm_close();
                    stage = SM_Idle;
                    return ephstate.get_appropriate_mode();
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
