#include "finance.h"

#include "assetpaths.h"

#include "util/value.h"

Finance::Finance() : ModeBase("Finance") {
}

void Finance::enter() {
    ModeBase::enter();

    draw_manager.draw(IMG_BG_NEWS);

    char heading[32];
    snprintf(heading, sizeof(heading), "Finances");
    draw_manager.draw_text(
        Font::Large,
        heading,
        Justify::Centre,
        RES_X/2, 21,
        COL_TEXT);

    draw_finance();
}

void Finance::draw_finance() {
    Player *p = exostate().get_active_player();

    char t[128];

    int m = max(1, exostate().get_month() - 1);

    snprintf(t, sizeof(t), "MC at end of Month %d: %d", m-1, p->get_mc_month_end());
    draw_manager.draw_text(
        t,
        Justify::Left,
        10, 40,
        COL_TEXT);

    snprintf(t, sizeof(t), "MC at start of Month %d: %d", m, p->get_mc_month_start());
    draw_manager.draw_text(
        t,
        Justify::Left,
        10, 60,
        COL_TEXT);
}

ExodusMode Finance::update(float delta) {
    if (draw_manager.clicked() || input_manager.consume(K_F)) {
        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}
