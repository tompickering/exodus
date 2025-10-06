#include "achievements.h"

#include "state/exodus_state.h"

extern ExodusState exostate;

bool AchievementManager::achievements_enabled() {
    if (exostate.multiplayer()) {
        return false;
    }

    return true;
}

void AchievementManager::unlock(Achievement achievement) {
    if (!achievements_enabled()) {
        return;
    }

    enact_unlock(achievement);
}
