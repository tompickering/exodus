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
}

void StarMap::enter() {
    ModeBase::enter(ID::END);
    star = exostate.get_active_star();

    DrawTarget tgt = TGT_Primary;

    for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
        planet_progress[i] = 0;
    }

    draw_manager.draw(tgt, IMG_MAP_SOLAR);
    draw_panel_bg(tgt);
    draw_planets(0.f);
    draw_manager.save_background();
    draw_manager.show_cursor(true);

    for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
        if (select_planet(i))
            break;
    }

    if (tgt != TGT_Primary) {
        draw_manager.fade_start(1.f, 12);
    }

    stage = SM_Idle;
}

ExodusMode StarMap::update(float delta) {
    SpriteClick click;

    if (draw_manager.fade_active()) {
        return ExodusMode::MODE_None;
    }

    Player *player = exostate.get_active_player();
    Planet *planet = exostate.get_active_planet();

    draw_planets(delta);

    switch(stage) {
        case SM_Idle:
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
                            if (planet->get_owner() == exostate.get_active_player_idx()) {
                                // TODO: Comms with own planet
                            } else {
                                // TODO: Comms with enemy planet
                            }
                        } else {
                            // TODO: Comms to claim planet
                            //comm_set_title("Message from counsellor");
                            //comm_set_img_caption("COUNSELLOR");
                        }
                    }
                } else {
                    // Back
                    draw_manager.fade_black(1.2f, 24);
                    stage = SM_Back2Gal;
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
        Planet *planet = star->get_planet(i);
        if (planet && planet->exists()) {
            bool active = planet == exostate.get_active_planet();
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

                draw_manager.draw(
                    id(ID::PLANET1 + i),
                    IMG_PA_ROT,
                    {140 + i*95,
                     (RES_Y / 2) - 30 + ((i % 2) == 0 ? -30 : 30),
                     .5f, .5f,
                     1, 1});
            }
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
