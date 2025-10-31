#include "location.h"

#ifdef DBG
#include "state/exodus_debug.h"
extern ExodusDebug exodebug;
#endif

PlayerLocation::PlayerLocation() {
    months_to_arrive = 0;
    location = -1;
    target = -1;
    planet_target = 0;
    just_arrived = false;
    visited = 0;
}

bool PlayerLocation::advance() {
    just_arrived = false;
    if (months_to_arrive > 0) {
        just_arrived = (--months_to_arrive == 0);
    }
    if (just_arrived) {
        location = target;
        if (target >= 0) {
            visited |= (uint64_t)1 << target;
        }
    }
    return just_arrived;
}

void PlayerLocation::complete() {
    while (in_flight()) {
        advance();
    }
}

bool PlayerLocation::in_flight() {
    return months_to_arrive > 0;
}

int PlayerLocation::get() {
    return location;
}

void PlayerLocation::set(int nt) {
    target = nt;
    months_to_arrive = 1;
    advance();
}

void PlayerLocation::set_target(int nt, int m) {
    if (target != nt) {
        target = nt;
        months_to_arrive = m;
    }
}

int PlayerLocation::get_target() const {
    return target;
}

void PlayerLocation::set_planet_target(int nt) {
    planet_target = nt;
}

int PlayerLocation::get_planet_target() {
    return planet_target;
}

bool PlayerLocation::has_visited(int query_target) {
#ifdef DBG
    if (exodebug.full_visibility)
        return true;
#endif
    // Guild (-1) is always true
    if (query_target < 0)
        return true;
    return (bool)(visited & ((uint64_t)1 << query_target));
}

void PlayerLocation::unset_target() {
    target = location;
    months_to_arrive = 0;
}

bool PlayerLocation::is_target_set() {
    return target != location;
}

int PlayerLocation::get_months_to_arrive() {
    return months_to_arrive;
}

void PlayerLocation::save(cJSON* j) const {
    SAVE_NUM(j, nopirates);
    SAVE_NUM(j, location);
    SAVE_NUM(j, target);
    SAVE_NUM(j, planet_target);
    SAVE_NUM(j, months_to_arrive);
    SAVE_BOOL(j, just_arrived);
    SAVE_NUM(j, visited);
}

void PlayerLocation::load(cJSON* j) {
    LOAD_NUM(j, nopirates);
    LOAD_NUM(j, location);
    LOAD_NUM(j, target);
    LOAD_NUM(j, planet_target);
    LOAD_NUM(j, months_to_arrive);
    LOAD_BOOL(j, just_arrived);
    LOAD_NUM(j, visited);
}
