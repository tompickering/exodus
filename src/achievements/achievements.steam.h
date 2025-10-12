#ifndef GUARD_ACHIEVEMENTS_STEAM_H
#define GUARD_ACHIEVEMENTS_STEAM_H

#include "achievements.h"

class AchievementManagerSteam : public AchievementManager {
    public:
        virtual bool init() override;
        virtual void enact_unlock(Achievement) override;
};

#endif
