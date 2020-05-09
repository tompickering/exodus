#include "planet_status.h"

#include "galaxy/planet.h"
#include "state/exodus_state.h"

#include "assetpaths.h"

enum ID {
    FLAG,
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

    char x_pad = 30;
    PlayerInfo *owner = p->is_owned() ? exostate.get_player(p->get_owner()) : nullptr;

    if (owner) {
        snprintf(heading, 12 + PLANET_MAX_NAME, "The planet %s", p->get_name());
        x_pad += 70;

        draw_manager.draw(
            id(ID::FLAG),
            flags[owner->flag_idx],
            {12, 100, 0, 0, 1, 1});
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
