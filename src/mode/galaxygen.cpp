#include "galaxygen.h"

GalaxyGen::GalaxyGen() : ModeBase("GalaxyGen") {
}

void GalaxyGen::enter() {
    gen_required = true;
}

ExodusMode GalaxyGen::update(float delta) {
    if (gen_required) {
        exostate.generate_galaxy();
        gen_required = false;
    }

    return ExodusMode::MODE_None;
}
