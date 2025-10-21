#include "achievements.h"

#include "state/exodus_state.h"



const char* ACHIEVEMENT_IDS[] = {
    "ACH_None",
    "ACH_WinMight",
    "ACH_WinMoney",
    "ACH_WinCiv",
    "ACH_WinMedium",
    "ACH_WinHard",
    "ACH_WinSmallGalaxy",
    "ACH_WinMediumGalaxy",
    "ACH_WinLargeGalaxy",
    "ACH_GuildMember",
    "ACH_FirstCity",
    "ACH_FirstStarport",
    "ACH_ShipEquipped",
    "ACH_FullCrew",
    "ACH_RepairsComplete",
    "ACH_AllAlliances",
    "ACH_ManyAllies",
    "ACH_ManyEnemies",
    "ACH_AllianceNoMC",
    "ACH_AllianceForced",
    "ACH_Ceasefire",
    "ACH_SecretMission",
    "ACH_BombingMission",
    "ACH_AllGoodOfficers",
    "ACH_DefenseWon",
    "ACH_AttackWon",
    "ACH_OneInfantry",
    "ACH_WinBadOdds",
    "ACH_WinVBadOdds",
    "ACH_AllInventions",
    "ACH_PlanetMined",
    "ACH_AllExplored",
    "ACH_SheriffHighScore",
    "ACH_GuildWarrior",
    "ACH_GuildStrong",
    "ACH_GuildConqueror",
    "ACH_GuildMighty",
    "ACH_GuildGreat",
    "ACH_GuildSovereign",
    "ACH_GuildEmperor",
    "ACH_VeryGoodRep",
    "ACH_NukeAPlanet",
    "ACH_WinNoInfractions",
    "ACH_AliensDefeated",
    "ACH_RebelsSuppressed",
    "ACH_RebelsAppeased",
    "ACH_BestStartWorld",
    "ACH_ArtPlanetP1",
    "ACH_ArtPlanetP2",
    "ACH_ArtPlanetP3",
    "ACH_ParadiseLost",
    "ACH_ParadiseGained",
    "ACH_NativesDestroyed",
    "ACH_TradeGood",
    "ACH_TradeVGood",
    "ACH_TradeExcellent",
    "ACH_TradeClean",
    "ACH_IllicitTrade",
    "ACH_PopulationAppeased",
    "ACH_DefeatHostileFleet",
    "ACH_PlanetFried"
};

bool AchievementManager::achievements_enabled() {
    if (exostate().multiplayer()) {
        return false;
    }

    return true;
}

bool AchievementManager::is_unlocked(Achievement achievement) {
    return (unlocked_achievements & (1 << achievement));
}

void AchievementManager::set_unlocked(Achievement achievement) {
    unlocked_achievements = unlocked_achievements | (1 << achievement);
}

void AchievementManager::unlock(Achievement achievement) {
    if (!achievements_enabled()) {
        return;
    }

    if (is_unlocked(achievement)) {
        return;
    }

    set_unlocked(achievement);

    L.info("[ACHIEVEMENT UNLOCKED] %d", (int)achievement);

#ifdef DBG
    L.info("(Skipping achievement unlock in debug build)");
#else
    enact_unlock(achievement);
#endif
}
