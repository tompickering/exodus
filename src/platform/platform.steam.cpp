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
    internals->load_achievements();
    return init_result;
}

void PlatformSteam::poll() {
    SteamAPI_RunCallbacks();
}

bool PlatformSteam::shutdown() {
    SteamAPI_Shutdown();
    return true;
}

void PlatformSteam::unlock_achievement(Achievement achievement) {
    if (internals != nullptr) {
        internals->unlock_achievement(achievement);
    }
}

// SteamInternals

extern ACHIEVEMENTMANAGER achievement_manager;

SteamInternals::SteamInternals() {
    user = SteamUser();
    user_stats = SteamUserStats();
}

bool SteamInternals::load_achievements() {
    if (user_stats == nullptr) {
        return false;
    }

    for (int i = (int)ACH_None + 1; i < ACH_MAX; ++i) {
        bool achieved = false;
        user_stats->GetAchievement(ACHIEVEMENT_IDS[i], &achieved);

        if (achieved) {
            achievement_manager.set_unlocked((Achievement)i);
        }
    }

    return true;
}

void SteamInternals::unlock_achievement(Achievement achievement) {
    if (user_stats != nullptr) {
        user_stats->SetAchievement(ACHIEVEMENT_IDS[(int)achievement]);
    }
}

#endif
