#include "planet_colonise.h"

#include "shared.h"

PlanetColonise::PlanetColonise() : ModeBase("PlanetColonise") {
    stackable = false;
    planet = nullptr;
}

void PlanetColonise::enter() {
    planet = exostate.get_active_planet();

    if (!planet || !planet->exists()) {
        L.fatal("Entered PlanetColonise mode with invalid planet!");
    }

    stage = PlanetOverview;
}

ExodusMode PlanetColonise::update(float dt) {
    time += dt;
    return ExodusMode::MODE_PlanetMap;
}
