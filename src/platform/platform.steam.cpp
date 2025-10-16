#include "platform.steam.h"

#ifdef STEAM

#include "steam/steam_api.h"

bool PlatformSteam::init() {
    return SteamAPI_Init();
}

void PlatformSteam::poll() {
    SteamAPI_RunCallbacks();
}

bool PlatformSteam::shutdown() {
    SteamAPI_Shutdown();
    return true;
}

#endif
