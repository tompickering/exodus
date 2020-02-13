#include "mode_base.h"

#include "../shared.h"

ModeBase::ModeBase(const char *name) : name(name) {
}

ModeBase::~ModeBase() {
}

void ModeBase::enter() {
    L.debug("Entering mode: %s", name);
    draw_manager.clear();
}

void ModeBase::exit() {
    L.debug("Exiting mode: %s", name);
    draw_manager.show_cursor(false);
}

ExodusMode ModeBase::update(float delta) {
    return ExodusMode::ST_None;
}
