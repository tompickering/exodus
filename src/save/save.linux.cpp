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

    // TODO: Error checking for failed write
    ofstream out(string(file), ios::out | ios::binary);
    out.write(data, SAVE_SIZE);
    out.close();
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

    // TODO: Error checking for failed read or no such file
    ifstream in(string(file), ios::in | ios::binary);
    in.read(data, SAVE_SIZE);
    in.close();
    return true;
}
