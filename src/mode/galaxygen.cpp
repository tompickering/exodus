#include "galaxygen.h"

GalaxyGen::GalaxyGen() : ModeBase("GalaxyGen") {
}

void GalaxyGen::enter() {
}

ExodusMode GalaxyGen::update(float delta) {
    return ExodusMode::MODE_None;
}
