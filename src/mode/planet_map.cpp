#include "planet_map.h"

#include "galaxy/planet.h"

#include "assetpaths.h"

const float ANIM_RATE = 0.8f;

enum ID {
    END,
};

PlanetMap::PlanetMap() : ModeBase("PlanetMap") {
    stage = PM_Idle;
}

void PlanetMap::enter() {
    ModeBase::enter(ID::END);
    planet = exostate.get_active_planet();

    if (!planet) {
        L.fatal("Entered PlanetMap mode with no active planet");
    }

    DrawTarget tgt = TGT_Primary;

    draw_manager.draw(tgt, planet->sprites()->map_bg);
    draw_manager.save_background();
    draw_manager.show_cursor(true);

    stage = PM_Idle;
}

ExodusMode PlanetMap::update(float delta) {
    SpriteClick click;

    PlayerInfo *player = exostate.get_active_player();

    switch(stage) {
        case PM_Idle:
            if (draw_manager.clicked()) {
                return ExodusMode::MODE_Pop;
            }
            break;
    }

    return ExodusMode::MODE_None;
}
