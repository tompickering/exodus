#include "mc_categories.h"

#include "shared.h"

const char* cat2header(int idx) {
    switch ((MCReason)idx) {
        case MC_None:
            L.error("Category should not be possible for players: %d", idx);
            return "<MC_None>";
        case MC_Debug:
            return "<MC_Debug>";
        case MC_CPUWriteOffDebt:
            return "<MC_CPUWriteOffDebt>";
        case MC_ReturnToGalaxyReset:
            return "<MC_ReturnToGalaxyReset>";
        case MC_MAX:
            L.error("Category should not be possible for players: %d", idx);
            return "<MC_MAX>";
        case MC_StartCredits:
            return "From Earth";
        case MC_Colonise:
            return "Colonisation";
        case MC_Planets:
            return "Planets";
        case MC_Cities:
            return "Cities";
        case MC_VillageGifts:
            return "Gifts";
        case MC_Army:
            return "Army";
        case MC_ScienceFunding:
            return "Science";
        case MC_Building:
            return "Construction";
        case MC_BuildingLunarBase:
            return "Lunar Base";
        case MC_Festival:
            return "Festivals";
        case MC_FactoryRepair:
            return "Factory Repair";
        case MC_ExtraTaxes:
            return "Extra Taxes";
        case MC_TradingCentre:
            return "Trade Centre";
        case MC_FleetProd:
            return "Fleet Prod.";
        case MC_ShipEquip:
            return "Ship Equip";
        case MC_Research:
            return "Research";
        case MC_Discovery:
            return "Discovery";
        case MC_ChangeGlobalClimate:
            return "Climate";
        case MC_ArtPlanet:
            return "Art. Planet";
        case MC_SecretService:
            return "Sec. Service";
        case MC_Missions:
            return "Missions";
        case MC_NewOfficer:
            return "Enlisting";
        case MC_OldOfficer:
            return "Severance";
        case MC_OfficerSalary:
            return "Salary";
        case MC_TempOfficer:
            return "Temp. Recruit";
        case MC_PeaceDeal:
            return "Peace Deal";
        case MC_PeaceDealRebels:
            return "Rebel Peace";
        case MC_Alliance:
            return "Alliance";
        case MC_TradeFee:
            return "Trade Fees";
        case MC_Trade:
            return "Trade";
        case MC_TradeBuy:
            return "Trade Buy";
        case MC_WarSupport:
            return "War Support";
        case MC_Mines:
            return "Mines";
        case MC_GuildMembership:
            return "Guild Membership";
        case MC_Fine:
            return "Fines";
        case MC_SpaceBattleLoot:
            return "Loot";
        case MC_SpaceBattleSurrendered:
            return "Piracy";
        case MC_DiplomaticReparations:
            return "Diplomacy";
    }

    return "<NONE>";
}
