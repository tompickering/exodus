#ifndef GUARD_ACHIEVEMENTS_H
#define GUARD_ACHIEVEMENTS_H

#include <stdint.h>

enum Achievement : uint8_t {
    ACH_None,
    ACH_WinMight,
    ACH_WinMoney,
    ACH_WinCiv,
    ACH_WinMedium,
    ACH_WinHard,
    ACH_WinSmallGalaxy,
    ACH_WinMediumGalaxy,
    ACH_WinLargeGalaxy,
    ACH_GuildWarrior,
    ACH_GuildStrong,
    ACH_GuildConqueror,
    ACH_GuildMighty,
    ACH_GuildGreat,
    ACH_GuildWarlord,
    ACH_GuildEmperor,
    ACH_GuildMember,
    ACH_VeryGoodRep,
    ACH_WinNoInfractions,
    ACH_RepairsComplete,
    ACH_FullCrew,
    ACH_ShipEquipped,
    ACH_SheriffHighScore,
    ACH_NukeAPlanet,
    ACH_BestStartWorld,
    ACH_FirstCity,
    ACH_FirstStarport,
    ACH_PlanetMined,
    ACH_AllExplored,
    ACH_ParadiseLost,
    ACH_ParadiseGained,
    ACH_NativesDestroyed,
    ACH_DefeatHostileFleet,
    ACH_AllAlliances,
    ACH_AllGoodOfficers,
    ACH_AttackWon,
    ACH_DefenseWon,
    ACH_RebelsAppeased,
    ACH_RebelsSuppressed,
    ACH_AliensDefeated,
    ACH_AllInventions,
    ACH_ArtPlanetP1,
    ACH_ArtPlanetP2,
    ACH_ArtPlanetP3,
    ACH_MAX
};

static_assert(ACH_MAX <= 64);

class AchievementManager {
    public:
        AchievementManager() : unlocked_this_session(0) {}
        virtual bool init() = 0;
        virtual bool achievements_enabled() final;
        virtual void unlock(Achievement) final;
        virtual void enact_unlock(Achievement) = 0;

    private:
        uint64_t unlocked_this_session;
};

#ifdef STEAM
#include "achievements.steam.h"
#else
#include "achievements.null.h"
#endif

#endif
