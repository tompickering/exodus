#include "news.h"

#include "assetpaths.h"

#include "util/value.h"

#define HEADER_HEIGHT 78

News::News() : ModeBase("News") {
}

void News::enter() {
    ModeBase::enter();
    Player *player = exostate.get_active_player();

    draw_manager.draw(IMG_BG_NEWS);
    draw_manager.fill({0, 0, RES_X, HEADER_HEIGHT}, COL_BORDERS);
    draw_manager.fill_pattern({4, 4, RES_X-8, HEADER_HEIGHT - 8});

    char heading[32];
    int m = max(1, exostate.get_month() - 1);
    snprintf(heading, sizeof(heading), "News of month %d", m);
    draw_manager.draw_text(
        Font::Large,
        heading,
        Justify::Centre,
        RES_X/2, 21,
        COL_TEXT);

    draw_news();
}

void News::draw_news() {
    int y = HEADER_HEIGHT + 16;

    // TODO

    draw_manager.draw_text(
        "There is no information.",
        Justify::Left,
        4, y,
        COL_TEXT);
}

ExodusMode News::update(float delta) {
    if (draw_manager.clicked() || input_manager.consume(K_R) || input_manager.consume(K_N)) {
        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}
