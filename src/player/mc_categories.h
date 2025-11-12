#ifndef GUARD_MC_CATEGORIES_H
#define GUARD_MC_CATEGORIES_H

#define MAX_MC_CATEGORIES 50

const char* cat2header(int);

enum MCReason {
    MC_None,
    MC_Debug,
    MC_StartCredits,
    MC_Colonise,
    MC_Planets,
    MC_Cities,
    MC_VillageGifts,
    MC_Army,
    MC_ScienceFunding,
    MC_Building,
    MC_BuildingLunarBase,
    MC_Festival,
    MC_FactoryRepair,
    MC_ExtraTaxes,
    MC_TradingCentre,
    MC_FleetProd,
    MC_ShipEquip,
    MC_Research,
    MC_Discovery,
    MC_ChangeGlobalClimate,
    MC_ArtPlanet,
    MC_SecretService,
    MC_Missions,
    MC_NewOfficer,
    MC_OldOfficer,
    MC_OfficerSalary,
    MC_TempOfficer,
    MC_PeaceDeal,
    MC_PeaceDealRebels,
    MC_Alliance,
    MC_TradeFee,
    MC_Trade,
    MC_TradeBuy,
    MC_WarSupport,
    MC_Mines,
    MC_GuildMembership,
    MC_Fine,
    MC_SpaceBattleLoot,
    MC_SpaceBattleSurrendered,
    MC_DiplomaticReparations,
    MC_CPUWriteOffDebt,
    MC_ReturnToGalaxyReset,
    MC_MAX,
};

static_assert(MC_MAX <= MAX_MC_CATEGORIES);

#endif
