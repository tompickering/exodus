#include "planet_status.h"

#include "galaxy/planet.h"

#include "assetpaths.h"

enum ID {
    END,
};

PlanetStatus::PlanetStatus() : ModeBase("PlanetStatus") {
}

void PlanetStatus::enter() {
    ModeBase::enter(ID::END);
    Planet *p = exostate.get_active_planet();

    if (!p) {
        L.fatal("Entered PlanetStatus mode with no active planet!");
    }

    // Orig: PROCstatus
    draw_manager.draw(p->sprites()->landscape);
    draw_manager.fill({0, 0, RES_X, 70}, COL_BORDERS);
    draw_manager.pattern_fill({4, 4, RES_X - 8, 62});

    char heading[12 + PLANET_MAX_NAME];

    if (p->is_owned()) {
        snprintf(heading, 12 + PLANET_MAX_NAME, "The planet %s", p->get_name());
    } else {
        strncpy(heading, "Unexplored planet", 12 + PLANET_MAX_NAME);
    }

    draw_manager.draw_text(
        Font::Large,
        heading,
        Justify::Centre,
        RES_X/2, 16,
        COL_TEXT);

    draw_manager.save_background();
    draw_manager.show_cursor(true);
}

ExodusMode PlanetStatus::update(float delta) {
    if (draw_manager.clicked()) {
        return exodus.get_prev_mode();
    }

    return ExodusMode::MODE_None;
}
