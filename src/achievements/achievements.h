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
    ACH_GuildMember,
    ACH_FirstCity,
    ACH_FirstStarport,
    ACH_ShipEquipped,
    ACH_FullCrew,
    ACH_RepairsComplete,
    ACH_AllAlliances,
    ACH_ManyAllies,
    ACH_ManyEnemies,
    ACH_AllianceNoMC,
    ACH_AllianceForced,
    ACH_Ceasefire,
    ACH_SecretMission,
    ACH_BombingMission,
    ACH_AllGoodOfficers,
    ACH_DefenseWon,
    ACH_AttackWon,
    ACH_OneInfantry,
    ACH_WinBadOdds,
    ACH_WinVBadOdds,
    ACH_AllInventions,
    ACH_PlanetMined,
    ACH_AllExplored,
    ACH_SheriffHighScore,
    ACH_GuildWarrior,
    ACH_GuildStrong,
    ACH_GuildConqueror,
    ACH_GuildMighty,
    ACH_GuildGreat,
    ACH_GuildWarlord,
    ACH_GuildEmperor,
    ACH_VeryGoodRep,
    ACH_NukeAPlanet,
    ACH_WinNoInfractions,
    ACH_AliensDefeated,
    ACH_RebelsSuppressed,
    ACH_RebelsAppeased,
    ACH_BestStartWorld,
    ACH_ArtPlanetP1,
    ACH_ArtPlanetP2,
    ACH_ArtPlanetP3,
    ACH_ParadiseLost,
    ACH_ParadiseGained,
    ACH_NativesDestroyed,
    ACH_TradeGood,
    ACH_TradeVGood,
    ACH_TradeExcellent,
    ACH_TradeClean,
    ACH_IllicitTrade,
    ACH_PopulationAppeased,
    ACH_DefeatHostileFleet,
    ACH_PlanetFried,
    ACH_MAX
};

static_assert(ACH_MAX <= 64);

extern const char* ACHIEVEMENT_IDS[];

class AchievementManager {
    public:
        AchievementManager() : unlocked_achievements(0) {}
        virtual bool init() = 0;
        virtual bool achievements_enabled() final;
        virtual bool is_unlocked(Achievement) final;
        virtual void set_unlocked(Achievement) final;
        virtual void unlock(Achievement) final;
        virtual void enact_unlock(Achievement) = 0;

    private:
        uint64_t unlocked_achievements;
};

#ifdef STEAM
#include "achievements.steam.h"
#else
#include "achievements.null.h"
#endif

#endif
