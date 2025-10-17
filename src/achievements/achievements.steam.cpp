#include "achievements.steam.h"

#include "platform/platform.steam.h"

#include "shared.h"

extern PLATFORM platform;

bool AchievementManagerSteam::init() {
    return true;
}

void AchievementManagerSteam::enact_unlock(Achievement achievement) {
    platform.unlock_achievement(achievement);
}
