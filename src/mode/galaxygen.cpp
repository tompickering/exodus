#include "galaxygen.h"

GalaxyGen::GalaxyGen() : ModeBase("GalaxyGen"), GalaxyDrawer() {
}

void GalaxyGen::enter() {
    gen_required = true;
}

ExodusMode GalaxyGen::update(float delta) {
    if (gen_required) {
        exostate.generate_galaxy();
        draw_galaxy(false);
        gen_required = false;
    }

    return ExodusMode::MODE_None;
}
