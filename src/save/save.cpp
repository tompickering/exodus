#include "save.h"

#include <cstring>

#include "state/exodus_state.h"

#include "shared.h"

static SaveMeta metadata[MAX_SLOTS];

extern ExodusState exostate;

const SaveMeta* SaveManager::get_all_meta(bool refresh) {
    if (refresh) {
        L.info("Reading save metadata");
        char data[SAVE_SIZE];
        for (int i = 0; i < MAX_SLOTS; ++i) {
            metadata[i].version = 0;
            metadata[i].exists = false;
            if (load_data(i, data)) {
                memcpy(&metadata[i], data, sizeof(SaveMeta));
            }
        }
    }
    return metadata;
}

bool SaveManager::load(int slot) {
    L.info("Loading slot %d", slot);
    char data[SAVE_SIZE];
    if (!load_data(slot, data)) {
        return false;
    }

    SaveMeta meta;
    memcpy(&meta, data, sizeof(SaveMeta));
    if (meta.version != EXODUS_VERSION) {
        L.error("SAVE DATA IS INCORRECT VERSION: %d vs %d", meta.version, EXODUS_VERSION);
        return false;
    }

    memcpy(&exostate, data+sizeof(SaveMeta), sizeof(ExodusState));
    return true;
}

bool SaveManager::save(int slot) {
    L.info("Saving slot %d", slot);
    SaveMeta meta;
    Player *player = exostate.get_player(0);
    strncpy(meta.name, player->get_name(), MAX_PLAYER_NAME);
    meta.version = EXODUS_VERSION;
    meta.exists = true;
    meta.name[MAX_PLAYER_NAME] = '\0';
    meta.month = exostate.get_month();
    meta.planets = exostate.get_n_planets(player);

    char data[SAVE_SIZE];
    memcpy(data, &meta, sizeof(SaveMeta));
    memcpy(data+sizeof(SaveMeta), &exostate, sizeof(ExodusState));
    return save_data(slot, data);
}
