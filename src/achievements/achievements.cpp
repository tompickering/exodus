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

    if (unlocked_this_session & (1 << achievement)) {
        return;
    }

    unlocked_this_session = unlocked_this_session | (1 << achievement);

    enact_unlock(achievement);
}
