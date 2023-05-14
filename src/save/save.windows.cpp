#ifdef WINDOWS

#include "save.windows.h"

#include <string>
#include <fstream>

#include <windows.h>
#include <shlobj.h>

static char save_dir[MAX_PATH];

using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;

const char* SaveManagerWindows::get_save_dir() {
    save_dir[0] = '\0';

    const char* profile = getenv("USERPROFILE");
    snprintf(save_dir, sizeof(save_dir), "%s\\ExodusSaves", profile);

    if (CreateDirectory(save_dir, nullptr) || GetLastError() == ERROR_ALREADY_EXISTS) {
        return save_dir;
    }

    return nullptr;
}

bool SaveManagerWindows::save_data(int slot, const char *data) {
    const char* dir = get_save_dir();

    if (!dir) {
        L.error("Could not determine save directory");
        return false;
    }

    char file[MAX_PATH];
    snprintf(file, sizeof(file), "%s\\%d", dir, slot);

    ofstream out(string(file), ios::out | ios::binary);

    if (out.fail()) {
        L.error("Could not open file %s for save", file);
        return false;
    }

    out.write(data, SAVE_SIZE);

    if (out.fail()) {
        L.error("Could not write to file %s", file);
        return false;
    }

    out.close();

    if (out.fail()) {
        L.warn("Could not close file after save: %s", file);
        return false;
    }

    return true;
}

bool SaveManagerWindows::load_data(int slot, char *data) {
    const char *dir = get_save_dir();

    if (!dir) {
        L.error("Could not determine load directory");
        return false;
    }

    char file[MAX_PATH];
    snprintf(file, sizeof(file), "%s\\%d", dir, slot);

    ifstream in(string(file), ios::in | ios::binary);

    if (in.fail()) {
        // File most likely does not exist
        return false;
    }

    in.read(data, SAVE_SIZE);

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

#endif
