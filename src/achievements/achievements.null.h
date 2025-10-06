#ifndef GUARD_ACHIEVEMENTS_NULL_H
#define GUARD_ACHIEVEMENTS_NULL_H

#include "achievements.h"

class AchievementManagerNull : public AchievementManager {
    public:
        virtual bool init() override;
        virtual void enact_unlock(Achievement) override;
};

#endif
