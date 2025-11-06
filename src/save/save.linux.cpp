#if defined(LINUX) || defined(MAC)

#include "save.linux.h"

#include <unistd.h> // For getuid()
#include <pwd.h>

#include <sys/stat.h>

#include "shared.h"

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

        snprintf(save_dir, sizeof(save_dir), "%s/.local", home);
        mkdir(save_dir, S_IRUSR | S_IWUSR | S_IXUSR);

        snprintf(save_dir, sizeof(save_dir), "%s/.local/share", home);
        mkdir(save_dir, S_IRUSR | S_IWUSR | S_IXUSR);

        snprintf(save_dir, sizeof(save_dir), "%s/.local/share/exodus", home);
        mkdir(save_dir, S_IRUSR | S_IWUSR | S_IXUSR);

        snprintf(save_dir, sizeof(save_dir), "%s/.local/share/exodus/enhanced", home);
        mkdir(save_dir, S_IRUSR | S_IWUSR | S_IXUSR);

        snprintf(save_dir, sizeof(save_dir), "%s/.local/share/exodus/classic", home);
        mkdir(save_dir, S_IRUSR | S_IWUSR | S_IXUSR);

        const char* mode = "classic";

        if (ENHANCED()) {
            mode = "enhanced";
        }

        snprintf(save_dir, sizeof(save_dir), "%s/.local/share/exodus/%s", home, mode);
    }

    mkdir(save_dir, S_IRUSR | S_IWUSR | S_IXUSR);

    return save_dir;
}

#endif
