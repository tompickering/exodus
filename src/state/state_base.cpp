#include "state_base.h"

#include "../shared.h"

StateBase::StateBase(const char *name) : name(name) {
}

StateBase::~StateBase() {
}

void StateBase::enter() {
    L.debug("Entering state: %s", name);
    draw_manager.clear();
}

void StateBase::exit() {
    L.debug("Exiting state: %s", name);
    draw_manager.show_cursor(false);
}

ExodusState StateBase::update(float delta) {
    return ExodusState::ST_None;
}
