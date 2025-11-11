#include "finance.h"

#include "assetpaths.h"

#include "util/value.h"

Finance::Finance() : ModeBase("Finance") {
}

void Finance::enter() {
    ModeBase::enter();

    draw_manager.draw(IMG_BG_NEWS);

    char heading[32];
    int m = max(1, exostate().get_month() - 1);
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
}

ExodusMode Finance::update(float delta) {
    if (draw_manager.clicked() || input_manager.consume(K_F)) {
        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}
