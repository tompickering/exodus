#include "mode_base.h"

#include "shared.h"

ModeBase::ModeBase(const char *name) : name(name) {
}

ModeBase::~ModeBase() {
}

void ModeBase::enter(unsigned int max_id) {
    ModeBase::enter();
    register_ids(max_id);
}

void ModeBase::enter() {
    id_max = 0;
    L.debug("Entering mode: %s", name);
    draw_manager.clear();
}

void ModeBase::exit() {
    L.debug("Exiting mode: %s", name);
    unregister_ids();
    draw_manager.show_cursor(false);
}

SprID ModeBase::id(unsigned int enum_id) {
    return ids[enum_id];
}

void ModeBase::register_ids(unsigned int max_id) {
    if (max_id > MAX_STATIC_IDS) {
        L.fatal("Max ID count surpassed: %d", max_id);
    }

    id_max = max_id;
    for (unsigned int i = 0; i < max_id; ++i) {
        ids[i] = draw_manager.new_sprite_id();
    }
}

void ModeBase::unregister_ids() {
    for (unsigned int i = 0; i < id_max; ++i) {
        draw_manager.release_sprite_id(ids[i]);
    }
}

ExodusMode ModeBase::update(float delta) {
    return ExodusMode::MODE_None;
}
