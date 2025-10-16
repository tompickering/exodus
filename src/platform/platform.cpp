#include "platform.h"

#ifdef STEAM

#include "steam/steam_api.h"

bool platform_init() {
    return SteamAPI_Init();
}

void platform_poll() {
    SteamAPI_RunCallbacks();
}

bool platform_shutdown() {
    SteamAPI_Shutdown();
    return true;
}

#else

bool platform_init() {
    return true;
}

void platform_poll() {
}

bool platform_shutdown() {
    return true;
}

#endif
