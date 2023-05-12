#ifdef LINUX

#include "save.linux.h"

#include <string>
#include <fstream>
#include <unistd.h> // For getuid()
#include <pwd.h>

#include "shared.h"

using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;

#define MAX_PATH 255

const char* SaveManagerLinux::get_save_dir() {
    const char *savedir = getenv("HOME");

    if (!savedir) {
        savedir = getpwuid(getuid())->pw_dir;
    }

    // Safe to return this; getenv result already in our memory space
    return savedir;
}

bool SaveManagerLinux::save_data(int slot, const char *data) {
    const char *dir = get_save_dir();

    if (!dir) {
        L.error("Could not determine save directory");
        return false;
    }

    char file[MAX_PATH+1];
    snprintf(file, MAX_PATH, "%s/%d", dir, slot);
    file[MAX_PATH] = '\0';

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
    }

    return true;
}

bool SaveManagerLinux::load_data(int slot, char *data) {
    const char *dir = get_save_dir();

    if (!dir) {
        L.error("Could not determine load directory");
        return false;
    }

    char file[MAX_PATH+1];
    snprintf(file, MAX_PATH, "%s/%d", dir, slot);
    file[MAX_PATH] = '\0';

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
