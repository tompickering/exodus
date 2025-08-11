#include "save.h"

#include <cstring>

#include "state/exodus_state.h"

#include "shared.h"

static SaveMeta metadata[MAX_SLOTS];

extern ExodusState exostate;

const SaveMeta* SaveManager::get_all_meta(bool refresh) {
    if (refresh) {
        L.info("Reading save metadata");
        char *data = nullptr;
        for (int i = 0; i < MAX_SLOTS; ++i) {
            metadata[i].exists = false;
            if (load_data(EXODUS_VERSION, i, &data, nullptr)) {
                cJSON *j = cJSON_Parse(data);
                metadata[i].load(j);
            }

            if (data != nullptr)
            {
                free(data);
                data = nullptr;
            }
        }
    }

    return metadata;
}

bool SaveManager::load(int slot) {
    L.info("Loading slot %d", slot);
    char *data_meta = nullptr;
    char *data_game = nullptr;

    bool success = false;

    if (load_data(EXODUS_VERSION, slot, &data_meta, &data_game)) {
        success = true;
        cJSON *json_game = cJSON_Parse(data_game);
        exostate.load(json_game);
    }

    if (data_meta != nullptr)
    {
        free(data_meta);
        data_meta = nullptr;
    }

    if (data_game != nullptr)
    {
        free(data_game);
        data_game = nullptr;
    }

    return success;
}

bool SaveManager::save(int slot) {
    L.info("Saving slot %d", slot);

    SaveMeta meta;
    Player *player = exostate.get_player(0);
    strncpy(meta.name, player->get_name(), MAX_PLAYER_NAME);
    meta.exists = true;
    meta.name[MAX_PLAYER_NAME] = '\0';
    meta.month = exostate.get_month();
    meta.planets = exostate.get_n_planets(player);

    cJSON *json_meta = cJSON_CreateObject();
    meta.save(json_meta);
    char *meta_str = cJSON_Print(json_meta);

    cJSON *json_game = cJSON_CreateObject();
    exostate.save(json_game);
    char *game_str = cJSON_Print(json_game);

    return save_data(EXODUS_VERSION, slot, meta_str, game_str);
}
