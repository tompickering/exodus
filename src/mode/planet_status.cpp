#include "planet_status.h"

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

    draw_manager.draw(p->sprites()->landscape);
    draw_manager.save_background();
    draw_manager.show_cursor(true);
}

ExodusMode PlanetStatus::update(float delta) {
    if (draw_manager.clicked()) {
        return exodus.get_prev_mode();
    }

    return ExodusMode::MODE_None;
}
