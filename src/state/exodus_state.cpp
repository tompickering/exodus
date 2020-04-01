#include "exodus_state.h"

#include <cstring>

#include "assetpaths.h"
#include "shared.h"

const char* flags[] = {
    IMG_TS1_FLAG13,
    IMG_TS1_FLAG14,
    IMG_TS1_FLAG15,
    IMG_TS1_FLAG10,
    IMG_TS1_FLAG11,
    IMG_TS1_FLAG12,
    IMG_TS1_FLAG7,
    IMG_TS1_FLAG8,
    IMG_TS1_FLAG9,
    IMG_TS1_FLAG4,
    IMG_TS1_FLAG5,
    IMG_TS1_FLAG6,
    IMG_TS1_FLAG1,
    IMG_TS1_FLAG2,
    IMG_TS1_FLAG3,
};

ExodusState::ExodusState() {
    galaxy_finalised = false;
}

void ExodusState::init(GameConfig config) {
    galaxy_finalised = false;
    size = config.size;
    n_players = config.n_players;
    month = 1;
    active_player = 0;
    unsigned int i;

    // PLAYER INIT: Human
    for (i = 0; i < n_players; ++i) {
        memcpy(&player_info[i], &config.info[i], sizeof(PlayerInfo));
        player_info[i].human = true;
        player_info[i].intro_seen = false;
        switch(i) {
            case 0:
                player_info[i].fleet_marker = IMG_TS1_ICON1;
                break;
            case 1:
                player_info[i].fleet_marker = IMG_TS1_ICON2;
                break;
            case 2:
                player_info[i].fleet_marker = IMG_TS1_ICON3;
                break;
            case 3:
                player_info[i].fleet_marker = IMG_TS1_ICON4;
                break;
            case 4:
                player_info[i].fleet_marker = IMG_TS1_ICON5;
                break;
        }
    }

    // PLAYER INIT: CPU
    for (; i < N_PLAYERS; ++i) {
        player_info[i].human = false;
        player_info[i].fleet_marker = nullptr;
        sprintf(player_info[i].name, "%d", i);
        strcpy(player_info[i].title, "CPU");
    }

    // PLAYER INIT: Everyone
    for (i = 0; i < N_PLAYERS; ++i) {
        snprintf(
            player_info[i].full_name,
            MAX_PLAYER_TITLE + 1 + MAX_PLAYER_NAME + 1,
            "%s %s",
            player_info[i].title,
            player_info[i].name);
    }

    aim = config.aim;
    enemy_start = config.enemy_start;
    L.debug("ExodusState init");
    L.debug("      Size: %d", size);
    L.debug("   Players: %d", n_players);
    for (unsigned int i = 0; i < n_players; ++i) {
        PlayerInfo *inf = &player_info[i];
        L.debug("       %s %s : Flag %d", inf->title, inf->name, inf->flag_idx);
    }
    L.debug("       Aim: %d", aim);
    L.debug("   Enemies: %d", enemy_start);
}

void ExodusState::generate_galaxy() {
    if (galaxy_finalised) {
        L.error("Cannot regenerate galaxy after finalisation");
    }

    unsigned int n_stars = 15;
    if (size == GAL_Medium)
        n_stars = 25;
    if (size == GAL_Large)
        n_stars = 40;

    new(&galaxy) Galaxy(n_stars);
}

void ExodusState::finalise_galaxy() {
    galaxy_finalised = true;
}

Galaxy* ExodusState::get_galaxy() {
    return &galaxy;
}

unsigned int ExodusState::get_month() {
    return month;
}

PlayerInfo* ExodusState::get_active_player() {
    return &player_info[active_player];
}

void ExodusState::set_active_flytarget(FlyTarget* new_target) {
    L.debug("Active target: %s", new_target->name);
    active_flytarget = new_target;
}

int ExodusState::tgt2loc(FlyTarget* tgt) {
    if (tgt == get_galaxy()->get_guild())
        return -1;
    for (int i = 0; i < GALAXY_MAX_STARS; ++i)
        if (&get_galaxy()->get_stars()[i] == tgt)
            return i;
    L.fatal("Invalid location");
    return -1;
}

FlyTarget* ExodusState::loc2tgt(int loc) {
    if (loc < 0)
        return get_galaxy()->get_guild();
    return &get_galaxy()->get_stars()[loc];
}
