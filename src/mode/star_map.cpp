#include "star_map.h"

#include <cmath>

#include "galaxy/star.h"

#include "assetpaths.h"

const float PLANET_ROTATE_SPD = .2f;

enum ID {
    PLANET1,
    PLANET2,
    PLANET3,
    PLANET4,
    PLANET5,
    END,
};

StarMap::StarMap() : ModeBase("StarMap"), PanelDrawer(PNL_Star) {
}

void StarMap::enter() {
    ModeBase::enter(ID::END);
    star = exostate.get_active_star();

    DrawTarget tgt = TGT_Primary;
    if (exodus.get_prev_mode() == ExodusMode::MODE_GalaxyMap) {
        tgt = TGT_Secondary;
    }

    for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
        planet_progress[i] = 0;
    }

    draw_manager.draw(tgt, IMG_MAP_SOLAR);
    draw_panel_bg(tgt);
    draw_planets(0.f);
    draw_manager.save_background();
    draw_manager.show_cursor(true);

    if (tgt != TGT_Primary) {
        draw_manager.fade_start(1.f, 12);
    }
}

ExodusMode StarMap::update(float delta) {

    draw_planets(delta);

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
            }
        }
    }
}
