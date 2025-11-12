#include "finance.h"

#include "assetpaths.h"

#include "util/value.h"

Finance::Finance() : ModeBase("Finance") {
}

void Finance::enter() {
    ModeBase::enter();

    draw_manager.draw(IMG_BG_NEWS);

    char heading[32];
    snprintf(heading, sizeof(heading), "Finances: Last Month");
    draw_manager.draw_text(
        Font::Large,
        heading,
        Justify::Centre,
        RES_X/2, 8,
        COL_TEXT);

    draw_finance();
}

void Finance::draw_finance() {
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

    const int items_base_y = 90;
    const int items_spacing_v = 16;

    const int col0 = 10;
    const int col7 = RES_X - 10;
    const int col6 = col7 - 110;
    const int col2 = (col0 + col6) / 3;
    const int col4 = (2*(col0 + col6)) / 3;
    const int col1 = col0 + 110;
    const int col3 = col2 + 110;
    const int col5 = col4 + 110;

    const int max_items_per_col = 20;

    for (int i = 0; i < max_items_per_col; ++i) {
        draw_manager.draw_text(
            Font::Tiny,
            "Diplomacy",
            Justify::Left,
            col0, items_base_y+items_spacing_v*i,
            COL_TEXT);
        draw_manager.draw_text(
            Font::Tiny,
            "10MC",
            Justify::Right,
            col1, items_base_y+items_spacing_v*i,
            COL_TEXT);
        draw_manager.draw_text(
            Font::Tiny,
            "Diplomacy",
            Justify::Left,
            col2, items_base_y+items_spacing_v*i,
            COL_TEXT);
        draw_manager.draw_text(
            Font::Tiny,
            "10MC",
            Justify::Right,
            col3, items_base_y+items_spacing_v*i,
            COL_TEXT);
        draw_manager.draw_text(
            Font::Tiny,
            "Diplomacy",
            Justify::Left,
            col4, items_base_y+items_spacing_v*i,
            COL_TEXT);
        draw_manager.draw_text(
            Font::Tiny,
            "10MC",
            Justify::Right,
            col5, items_base_y+items_spacing_v*i,
            COL_TEXT);
        draw_manager.draw_text(
            Font::Tiny,
            "Diplomacy",
            Justify::Left,
            col6, items_base_y+items_spacing_v*i,
            COL_TEXT);
        draw_manager.draw_text(
            Font::Tiny,
            "10MC",
            Justify::Right,
            col7, items_base_y+items_spacing_v*i,
            COL_TEXT);
    }

    draw_manager.draw_line(RES_X/2, items_base_y-10, RES_X/2, items_base_y+items_spacing_v*max_items_per_col, COL_TEXT2);

    // TODO
    draw_manager.draw_text(
        "Total: XMC",
        Justify::Centre,
        RES_X/4 - 12, items_base_y+items_spacing_v*max_items_per_col,
        COL_TEXT2);

    // TODO
    draw_manager.draw_text(
        "Total: XMC",
        Justify::Centre,
        (3*RES_X)/4 + 12, items_base_y+items_spacing_v*max_items_per_col,
        COL_TEXT2);

    // TODO
    draw_manager.draw_text(
        Font::Large,
        "NET: +XMC",
        Justify::Centre,
        RES_X/2, RES_Y-74,
        COL_TEXT2);
}

ExodusMode Finance::update(float delta) {
    if (draw_manager.clicked() || input_manager.consume(K_F)) {
        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}
