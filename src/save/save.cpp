#include "save.h"

#include <string>
#include <fstream>
#include <cstring>

#include "state/exodus_state.h"

#include "shared.h"

#include <sys/stat.h>

#include "shared.h"

using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;

static SaveMeta metadata[MAX_SLOTS];

static uint8_t obfuscation_byte = 0xFA;

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

bool SaveManager::save_data(uint32_t version, int slot, char *data_meta, char* data_game) {
    const char *dir = get_save_dir();

    if (!dir) {
        L.error("Could not determine save directory");
        return false;
    }

    char file[MAX_PATH];
    snprintf(file, sizeof(file), "%s/%d", dir, slot);

    ofstream out(string(file), ios::out | ios::binary);

    if (out.fail()) {
        L.error("Could not open file %s for save", file);
        return false;
    }

    char buf[64];
    int bytes;

    bytes = snprintf(buf, sizeof(buf), "%u\n", version);
    for (size_t i = 0; i < bytes; ++i) buf[i] ^= obfuscation_byte;
    out.write(buf, bytes);

    uint64_t size_meta = (uint64_t)strlen(data_meta);
    bytes = snprintf(buf, sizeof(buf), "%lu\n", size_meta);
    for (size_t i = 0; i < bytes; ++i) buf[i] ^= obfuscation_byte;
    out.write(buf, bytes);

    uint64_t size_game = (uint64_t)strlen(data_game);
    bytes = snprintf(buf, sizeof(buf), "%lu\n", size_game);
    for (size_t i = 0; i < bytes; ++i) buf[i] ^= obfuscation_byte;
    out.write(buf, bytes);

    for (size_t i = 0; i < size_meta; ++i) data_meta[i] ^= obfuscation_byte;
    for (size_t i = 0; i < size_game; ++i) data_game[i] ^= obfuscation_byte;

    out.write(data_meta, size_meta);
    out.write(data_game, size_game);

    if (out.fail()) {
        L.error("Could not write to file %s", file);
        return false;
    }

    out.close();

    if (out.fail()) {
        L.warn("Could not close file after save: %s", file);
    }

    return true;
}

bool SaveManager::load_data(uint32_t expected_version, int slot, char** data_meta, char** data_game) {
    const char *dir = get_save_dir();

    if (!dir) {
        L.error("Could not determine load directory");
        return false;
    }

    char file[MAX_PATH+1];
    snprintf(file, sizeof(file), "%s/%d", dir, slot);

    ifstream in(string(file), ios::in | ios::binary);

    if (in.fail()) {
        // File most likely does not exist
        return false;
    }

    char buf[64];

    size_t head = 0;

    do {
        in.read(buf+head, 1);
        buf[head] ^= obfuscation_byte;
        ++head;
    } while(buf[head-1] != '\n');

    uint32_t version = strtoul(buf, NULL, 10);

    if (version != expected_version) {
        L.error("SAVE DATA IS INCORRECT VERSION: %d (expected %d)", version, expected_version);
        return false;
    }

    head = 0;

    do {
        in.read(buf+head, 1);
        buf[head] ^= obfuscation_byte;
        ++head;
    } while(buf[head-1] != '\n');

    uint64_t size_meta = strtoul(buf, NULL, 10);

    head = 0;

    do {
        in.read(buf+head, 1);
        buf[head] ^= obfuscation_byte;
        ++head;
    } while(buf[head-1] != '\n');

    uint64_t size_game = strtoul(buf, NULL, 10);

    (*data_meta) = (char*)malloc(size_meta);
    in.read(*data_meta, size_meta);
    for (size_t i = 0; i < size_meta; ++i) (*data_meta)[i] ^= obfuscation_byte;

    if (data_game != nullptr)
    {
        (*data_game) = (char*)malloc(size_game);
        in.read(*data_game, size_game);
        for (size_t i = 0; i < size_game; ++i) (*data_game)[i] ^= obfuscation_byte;
    }

    if (in.fail()) {
        L.error("Could not read from file %s", file);
        return false;
    }

    in.close();

    if (in.fail()) {
        L.warn("Could not close file after load: %s", file);
    }

    return true;
}
