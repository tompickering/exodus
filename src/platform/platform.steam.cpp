#include "platform.steam.h"

#ifdef STEAM

PlatformSteam::~PlatformSteam() {
    if (internals != nullptr) {
        delete internals;
        internals = nullptr;
    }
}

bool PlatformSteam::init() {
    const bool init_result = SteamAPI_Init();
    internals = new SteamInternals();
    return init_result;
}

void PlatformSteam::poll() {
    SteamAPI_RunCallbacks();
}

bool PlatformSteam::shutdown() {
    SteamAPI_Shutdown();
    return true;
}

SteamInternals::SteamInternals() {
    user = SteamUser();
    user_stats = SteamUserStats();
}

#endif
