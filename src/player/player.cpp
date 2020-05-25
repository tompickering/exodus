#include "player.h"

#include <cstring>

#include "alien_names.h"
#include "shared.h"

static int alien_names_offset = -1;

const char* get_alien_name(Race race, int idx) {
    if (idx >= N_ALIEN_NAMES) {
        L.fatal("Request name with invalid index %d", idx);
    }

    if (race == RACE_Yokon)   return ALIEN_NAMES_YOKON[idx];
    if (race == RACE_Teri)    return ALIEN_NAMES_TERI[idx];
    if (race == RACE_Urkash)  return ALIEN_NAMES_URKASH[idx];
    if (race == RACE_Gordoon) return ALIEN_NAMES_GORDOON[idx];
    L.fatal("Request alien name for invalid race %d", (int)race);
    return "";
}

Player::Player() {
    dead = false;
    participating_in_game = false;
    _intro_seen = false;
}

void Player::init_alien_name(int idx) {
    if (alien_names_offset == -1) {
        alien_names_offset = rand() % N_ALIEN_NAMES;
    }
    int name_idx = (idx + alien_names_offset) % N_ALIEN_NAMES;
    set_name(get_alien_name(race, name_idx));
    int gender_boundary = 5;
    if (race == RACE_Yokon) gender_boundary = 6;
    if (race == RACE_Urkash) gender_boundary = 6;
    set_gender(name_idx <= gender_boundary ? GENDER_Female : GENDER_Male);
    if (get_gender() == GENDER_Female) {
        set_title("Lady");
        set_ref("Milady");
    } else {
        set_title("Lord");
        set_ref("Milord");
    }
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

Gender Player::get_gender() {
    return gender;
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

void Player::set_gender(Gender new_gender) {
    gender = new_gender;
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

bool Player::is_active() {
    // Indicate whether this player should be 'taking part' in the game
    // TODO: Check for has-left-the-galaxy etc
    return participating_in_game && is_alive();
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

bool Player::can_afford(unsigned int cost) {
    return mc >= (int)cost;
}

bool Player::attempt_spend(unsigned int cost) {
    if (can_afford(cost)) {
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

Starship& Player::get_starship() {
    return starship;
}
