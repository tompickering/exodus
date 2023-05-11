#include "news.h"

#include "assetpaths.h"

#include "util/value.h"

#define HEADER_HEIGHT 78

News::News() : ModeBase("News") {
}

void News::enter() {
    ModeBase::enter();

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

    bool any = false;

    char text[256];

    for (int i = 0; i < MAX_NEWSITEMS; ++i) {
        const NewsItem& news = exostate.get_news(i);
        if (news.type == NI_None) {
            break;
        }

        if (news.star_idx < 0 || news.planet_idx < 0) {
            L.error("Bad indices in newsitem: %d / %d", news.star_idx, news.planet_idx);
            continue;
        }

        Galaxy *gal = exostate.get_galaxy();
        Star *s = &(gal->get_stars()[news.star_idx]);
        Planet *p = s->get_planet(news.planet_idx);

        any = true;

        if (news.type == NI_LeftGalaxy) {
            snprintf(text, sizeof(text), "%s", news.get_string());
        } else {
            snprintf(text, sizeof(text), "%s at %s: %s", p->get_name(), s->name, news.get_string());
        }

        RGB col = COL_TEXT;
        if (news.player_owned) {
            col = COL_TEXT2;
        }

        draw_manager.draw_text(
            text,
            Justify::Left,
            4, y,
            col);

        y += 20;
    }

    if (!any) {
        draw_manager.draw_text(
            "There is no information.",
            Justify::Left,
            4, y,
            COL_TEXT);
    }
}

ExodusMode News::update(float delta) {
    if (draw_manager.clicked() || input_manager.consume(K_R) || input_manager.consume(K_N)) {
        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}
