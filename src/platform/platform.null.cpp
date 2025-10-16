#include "platform.null.h"

bool PlatformNull::init() {
    return true;
}

void PlatformNull::poll() {
}

bool PlatformNull::shutdown() {
    return true;
}

