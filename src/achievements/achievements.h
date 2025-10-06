#ifndef GUARD_ACHIEVEMENTS_H
#define GUARD_ACHIEVEMENTS_H

#include <stdint.h>

enum Achievement : uint8_t {
    ACH_MightEasy,
    ACH_MightMedium,
    ACH_MightHard,
    ACH_MoneyEasy,
    ACH_MoneyMedium,
    ACH_MoneyHard,
    ACH_TechEasy,
    ACH_TechMedium,
    ACH_TechHard,
    ACH_GuildWarrior,
    ACH_GuildStrong,
    ACH_GuildConqueror,
    ACH_GuildMighty,
    ACH_GuildGreat,
    ACH_GuildWarlord,
    ACH_GuildEmperor
};

class AchievementManager {
    public:
        AchievementManager() {}
        virtual bool init() = 0;
        virtual void unlock(Achievement) = 0;
};

#ifdef STEAM
#include "achievements.steam.h"
#else
#include "achievements.null.h"
#endif

#endif
