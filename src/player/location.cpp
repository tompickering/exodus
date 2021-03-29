#include "location.h"

const int TARGET_NONE = -2;

PlayerLocation::PlayerLocation() {
    months_to_arrive = 0;
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
    if (just_arrived && target >= 0) {
        visited |= (uint64_t)1 << target;
    }
    return just_arrived;
}

bool PlayerLocation::in_flight() {
    return months_to_arrive > 0;
}

void PlayerLocation::set(int nt) {
    target = nt;
    months_to_arrive = 1;
    advance();
}

void PlayerLocation::set_target(int nt, int m) {
    target = nt;
    months_to_arrive = m;
}

int PlayerLocation::get_target() {
    return target;
}

void PlayerLocation::set_planet_target(int nt) {
    planet_target = nt;
}

int PlayerLocation::get_planet_target() {
    return planet_target;
}

bool PlayerLocation::has_visited(int query_target) {
    // Guild (-1) is always true
    if (query_target < 0)
        return true;
    return (bool)(visited & ((uint64_t)1 << query_target));
}

void PlayerLocation::unset_target() {
    target = TARGET_NONE;
}

bool PlayerLocation::is_target_set() {
    return target != TARGET_NONE;
}
