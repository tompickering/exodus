#include "player.h"

#include <cstring>

#include "shared.h"

Player::Player() {
    dead = false;
    _intro_seen = false;
}

const char* Player::get_name() {
    return name;
}

const char* Player::get_title() {
    return title;
}

const char* Player::get_ref() {
    return ref;
}

const char* Player::get_full_name() {
    return full_name;
}

void Player::set_name(const char *new_name) {
    strncpy(name, new_name, MAX_PLAYER_NAME);
    refresh_full_name();
}

void Player::set_title(const char *new_title) {
    strncpy(title, new_title, MAX_PLAYER_TITLE);
    refresh_full_name();
}

void Player::set_ref(const char *new_ref) {
    strncpy(ref, new_ref, MAX_PLAYER_REFERENCE);
}

void Player::refresh_full_name() {
    snprintf(
        full_name,
        MAX_PLAYER_TITLE + 1 + MAX_PLAYER_NAME + 1,
        "%s %s",
        title,
        name);
}

Race Player::get_race() {
    return race;
}

bool Player::is_human() {
    return race == RACE_Human;
}

bool Player::is_alive() {
    return !dead;
}

int Player::get_flag_idx() {
    return flag_idx;
}

void Player::set_flag_idx(int new_idx) {
    flag_idx = new_idx;
}

int Player::get_mc() {
    return mc;
}

bool Player::intro_seen() {
    return _intro_seen;
}

void Player::set_intro_seen() {
    _intro_seen = true;
}

bool Player::attempt_spend(unsigned int cost) {
    if (mc >= cost) {
        mc -= cost;
        return true;
    }

    return false;
}

const char* Player::get_fleet_marker() {
    return fleet_marker;
}

void Player::set_fleet_marker(const char* new_fleet_marker) {
    fleet_marker = new_fleet_marker;
}

PlayerLocation& Player::get_location() {
    return location;
}
