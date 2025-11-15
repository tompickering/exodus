#include "finance.h"

#include "player/mc_categories.h"

#include "assetpaths.h"

#include "util/value.h"

Finance::Finance() : ModeBase("Finance") {
}

void Finance::enter() {
    ModeBase::enter();
    draw_finance(FR_LastMonth);
}

void Finance::draw_finance(FinanceReport report) {
    current_report = report;

    draw_manager.draw(IMG_BG_NEWS);

    char heading[32];

    if (report == FR_LastMonth) {
        snprintf(heading, sizeof(heading), "Finances: Last Month");
    } else if (report == FR_OverLastMonth) {
        snprintf(heading, sizeof(heading), "Finances: Over Last Month");
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

    if (current_report == FR_OverLastMonth) {
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
    const int items_spacing_v_big = 22;

    const int col0 = 10;
    const int col7 = RES_X - 10;
    const int col6 = col7 - 120;
    const int col2 = (col0 + col6) / 3;
    const int col4 = (2*(col0 + col6)) / 3;
    const int col1 = col0 + 120;
    const int col3 = col2 + 120;
    const int col5 = col4 + 120;

    const int max_items_per_col = 20;
    const int max_items_per_col_big = 14;

    const int* gains = nullptr;
    const int* losses = nullptr;

    if (report == FR_LastMonth) {
        gains = p->get_gains_last_month();
        losses = p->get_losses_last_month();
    } else if (report == FR_OverLastMonth) {
        gains = p->get_gains_over_last_month();
        losses = p->get_losses_over_last_month();
    } else {
        gains = p->get_gains_this_month();
        losses = p->get_losses_this_month();
    }

    int gains_total = 0;
    int losses_total = 0;

    int gains_lines = 0;
    int losses_lines = 0;

    for (int i = 0; i < MC_MAX; ++i) {
        if (gains[i] == 0) { continue; }
        ++gains_lines;
    }

    for (int i = 0; i < MC_MAX; ++i) {
        if (losses[i] == 0) { continue; }
        ++losses_lines;
    }

    bool draw_big = false;

    if (gains_lines <= max_items_per_col_big && losses_lines <= max_items_per_col_big) {
        draw_big = true;
    }

    int x_header = col0;
    int x_mc = col1;
    int y = items_base_y;

    int n = 0;

    if (draw_big) {
        x_mc = col3;
    }

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
            draw_big ? Font::Default : Font::Tiny,
            cat2header(i),
            Justify::Left,
            x_header, y,
            {0x00, 0xFF, 0x00});

        snprintf(t, sizeof(t), "%dMC", gains[i]);
        draw_manager.draw_text(
            draw_big ? Font::Default : Font::Tiny,
            t,
            Justify::Right,
            x_mc, y,
            {0x00, 0xFF, 0x00});

        y += draw_big ? items_spacing_v_big : items_spacing_v;
        ++n;
    }

    x_header = col4;
    x_mc = col5;
    y = items_base_y;

    if (draw_big) {
        x_mc = col7;
    }

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
            draw_big ? Font::Default : Font::Tiny,
            cat2header(i),
            Justify::Left,
            x_header, y,
            {0xFF, 0x00, 0x00});

        snprintf(t, sizeof(t), "%dMC", losses[i]);
        draw_manager.draw_text(
            draw_big ? Font::Default : Font::Tiny,
            t,
            Justify::Right,
            x_mc, y,
            {0xFF, 0x00, 0x00});

        y += draw_big ? items_spacing_v_big : items_spacing_v;
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
            draw_finance(FR_OverLastMonth);
            return ExodusMode::MODE_None;
        }

        if (current_report == FR_OverLastMonth) {
            draw_finance(FR_ThisMonth);
            return ExodusMode::MODE_None;
        }

        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}
