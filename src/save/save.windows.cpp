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

#endif
