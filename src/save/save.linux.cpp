#if defined(LINUX) || defined(MAC)

#include "save.linux.h"

#include <string>
#include <fstream>
#include <unistd.h> // For getuid()
#include <pwd.h>

#include <sys/stat.h>

#include "shared.h"

using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;

#define MAX_PATH 255

static char save_dir[MAX_PATH];

const char* SaveManagerLinux::get_save_dir() {
    const char *data_home = getenv("XDG_DATA_HOME");

    if (data_home) {
        snprintf(save_dir, sizeof(save_dir), data_home);
    } else {
        const char *home = getenv("XDG_DATA_HOME");

        if (!home) {
            home = getpwuid(getuid())->pw_dir;
        }

        snprintf(save_dir, sizeof(save_dir), "%s/.local/share/%s", home, "exodus");
    }

    mkdir(save_dir, S_IRUSR | S_IWUSR | S_IXUSR);

    return save_dir;
}

bool SaveManagerLinux::save_data(int slot, const char *data) {
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
    snprintf(file, sizeof(file), "%s/%d", dir, slot);

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
