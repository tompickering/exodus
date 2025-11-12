#include "finance.h"

#include "assetpaths.h"

#include "util/value.h"

Finance::Finance() : ModeBase("Finance") {
}

void Finance::enter() {
    ModeBase::enter();
    draw_finance(FR_LastMonth);
}

static const char* cat2header(int idx) {
    switch ((MCReason)idx) {
        case MC_None:
        case MC_Debug:
        case MC_CPUWriteOffDebt:
        case MC_ReturnToGalaxyReset:
        case MC_MAX:
            L.error("Category should not be possible for players: %d", idx);
            return "<UNKNOWN>";
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

void Finance::draw_finance(FinanceReport report) {
    current_report = report;

    draw_manager.draw(IMG_BG_NEWS);

    char heading[32];

    if (report == FR_LastMonth) {
        snprintf(heading, sizeof(heading), "Finances: Last Month");
    } else {
        snprintf(heading, sizeof(heading), "Finances: This Month");
    }

    draw_manager.draw_text(
        Font::Large,
        heading,
        Justify::Centre,
        RES_X/2, 8,
        COL_TEXT);

    Player *p = exostate().get_active_player();

    char t[128];

    int m = max(1, exostate().get_month() - 1);

    draw_manager.draw_text(
        "Gains",
        Justify::Centre,
        RES_X/4 - 12, 60,
        COL_TEXT2);

    draw_manager.draw_text(
        "Losses",
        Justify::Centre,
        (3*RES_X)/4 + 12, 60,
        COL_TEXT2);

    if (current_report == FR_ThisMonth) {
        snprintf(t, sizeof(t), "Month %d End: %dMC", m-1, p->get_mc_month_end());
        draw_manager.draw_text(
            t,
            Justify::Left,
            10, RES_Y-24,
            COL_TEXT);

        snprintf(t, sizeof(t), "Month %d Start: %dMC", m, p->get_mc_month_start());
        draw_manager.draw_text(
            t,
            Justify::Right,
            RES_X-10, RES_Y-24,
            COL_TEXT);
    }

    const int items_base_y = 90;
    const int items_spacing_v = 16;

    const int col0 = 10;
    const int col7 = RES_X - 10;
    const int col6 = col7 - 120;
    const int col2 = (col0 + col6) / 3;
    const int col4 = (2*(col0 + col6)) / 3;
    const int col1 = col0 + 120;
    const int col3 = col2 + 120;
    const int col5 = col4 + 120;

    const int max_items_per_col = 20;

    const int* gains = nullptr;
    const int* losses = nullptr;

    if (report == FR_LastMonth) {
        gains = p->get_gains_last_month();
        losses = p->get_losses_last_month();
    } else {
        gains = p->get_gains_this_month();
        losses = p->get_losses_this_month();
    }

    int gains_total = 0;
    int losses_total = 0;

    int x_header = col0;
    int x_mc = col1;
    int y = items_base_y;

    int n = 0;

    for (int i = 0; i < MC_MAX; ++i) {
        if (n == max_items_per_col) {
            x_header = col2;
            x_mc = col3;
            y = items_base_y;
        }

        if (gains[i] == 0) {
            continue;
        }

        gains_total += gains[i];

        draw_manager.draw_text(
            Font::Tiny,
            cat2header(i),
            Justify::Left,
            x_header, y,
            {0x00, 0xFF, 0x00});

        snprintf(t, sizeof(t), "%dMC", gains[i]);
        draw_manager.draw_text(
            Font::Tiny,
            t,
            Justify::Right,
            x_mc, y,
            {0x00, 0xFF, 0x00});

        y += items_spacing_v;
        ++n;
    }

    x_header = col4;
    x_mc = col5;
    y = items_base_y;
    n = 0;
    for (int i = 0; i < MC_MAX; ++i) {
        if (n == max_items_per_col) {
            x_header = col6;
            x_mc = col7;
            y = items_base_y;
        }

        if (losses[i] == 0) {
            continue;
        }

        losses_total += losses[i];

        draw_manager.draw_text(
            Font::Tiny,
            cat2header(i),
            Justify::Left,
            x_header, y,
            {0xFF, 0x00, 0x00});

        snprintf(t, sizeof(t), "%dMC", losses[i]);
        draw_manager.draw_text(
            Font::Tiny,
            t,
            Justify::Right,
            x_mc, y,
            {0xFF, 0x00, 0x00});

        y += items_spacing_v;
        ++n;
    }

    draw_manager.draw_line(RES_X/2, items_base_y-10, RES_X/2, items_base_y+items_spacing_v*max_items_per_col, COL_TEXT2);

    snprintf(t, sizeof(t), "Total: %dMC", gains_total);
    draw_manager.draw_text(
        t,
        Justify::Centre,
        RES_X/4 - 12, items_base_y+items_spacing_v*max_items_per_col,
        COL_TEXT2);

    snprintf(t, sizeof(t), "Total: %dMC", losses_total);
    draw_manager.draw_text(
        t,
        Justify::Centre,
        (3*RES_X)/4 + 12, items_base_y+items_spacing_v*max_items_per_col,
        COL_TEXT2);

    int total = gains_total - losses_total;

    RGB col = {0xFF, 0x00, 0x00};

    if (total > 0) {
        col = {0x00, 0xFF, 0x00};
    }

    if (total == 0) {
        col = COL_TEXT2;
    }

    snprintf(t, sizeof(t), "NET: %s%dMC", (total > 0 ? "+" : ""), total);
    draw_manager.draw_text(
        Font::Large,
        t,
        Justify::Centre,
        RES_X/2, RES_Y-74,
        col);
}

ExodusMode Finance::update(float delta) {
    if (draw_manager.clicked() || input_manager.consume(K_F)) {
        if (current_report == FR_LastMonth) {
            draw_finance(FR_ThisMonth);
            return ExodusMode::MODE_None;
        }

        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}
