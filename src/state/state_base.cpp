#include "state_base.h"

#include "../shared.h"

StateBase::StateBase(const char *name) : draw_cursor(false) {
}

StateBase::StateBase(const char *name, bool draw_cursor)
    : draw_cursor(draw_cursor), name(name) {
}

StateBase::~StateBase() {
}

void StateBase::enter() {
    L.debug("Entering state: %s", name);
    draw_manager.clear();
}

void StateBase::exit() {
    L.debug("Exiting state: %s", name);
}

ExodusState StateBase::update(float delta) {
}
