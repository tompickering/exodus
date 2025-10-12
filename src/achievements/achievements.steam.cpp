#include "achievements.steam.h"

#include "shared.h"

bool AchievementManagerSteam::init() {
    // TODO
    return true;
}

void AchievementManagerSteam::enact_unlock(Achievement achievement) {
    // TODO
    L.info("[STEAM ACHIEVEMENT UNLOCKED] %d", (int)achievement);
}
