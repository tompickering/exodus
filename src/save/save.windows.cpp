#ifdef WINDOWS

#include "save.windows.h"

#define MAX_PATH 255

const char* SaveManagerWindows::get_save_dir() {
    // TODO
    return nullptr;
}

bool SaveManagerWindows::save_data(int slot, const char *data) {
    // TODO
    return false;
}

bool SaveManagerWindows::load_data(int slot, char *data) {
    // TODO
    return false;
}

#endif
