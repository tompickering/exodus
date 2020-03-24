#include "exodus_state.h"

#include <cstring>

#include "../assetpaths.h"
#include "../shared.h"

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
    for (unsigned int i = 0; i < n_players; ++i) {
        memcpy(&player_info[i], &config.info[i], sizeof(PlayerInfo));
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
