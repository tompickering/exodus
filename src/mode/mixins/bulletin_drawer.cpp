#include "bulletin_drawer.h"

#include "assetpaths.h"
#include "shared.h"

#define BULLETIN_TRANSITION_RATE 1.4f

const int BULLETIN_BORDER = 6;
const int BULLETIN_W = 436 + (BULLETIN_BORDER) * 2;
const int BULLETIN_H = 306 + (BULLETIN_BORDER) * 2;
const int BULLETIN_X = (RES_X / 2) - (BULLETIN_W / 2);
const int BULLETIN_Y = (RES_Y / 2) - (BULLETIN_H / 2) - 12;
const int BULLETIN_TEXT_X = BULLETIN_X + BULLETIN_BORDER * 2 + 4;
const int BULLETIN_FLAG_BG_X = (RES_X / 2) - 48 - BULLETIN_BORDER;
const int BULLETIN_FLAG_BG_Y = BULLETIN_Y - 2 - 56 - BULLETIN_BORDER*2;
const int BULLETIN_FLAG_BG_W = 96 + BULLETIN_BORDER*2;
const int BULLETIN_FLAG_BG_H = 56 + BULLETIN_BORDER*2;
const int BULLETIN_BG_X = BULLETIN_X + BULLETIN_BORDER;
const int BULLETIN_BG_Y = BULLETIN_Y + BULLETIN_BORDER;

BulletinDrawer::BulletinDrawer() {
    for (int i = 0; i < BULLETIN_LINES; ++i) {
        strncpy(bulletin_text[i], "", 1);
    }
    bulletin_reset_text_cols();
    bulletin_has_been_acknowledged = false;
    bulletin_text_idx = 0;
    bulletin_transition = 0;
    bulletin_redraws_needed = true;
    bulletin_is_yesno = false;
    bulletin_yesno_was_yes = false;
    _bulletin_is_open = false;

    bulletin_bg_preserve = nullptr;
}

void BulletinDrawer::bulletin_start_new(bool transition) {
    for (int i = 0; i < bulletin_text_idx; ++i) {
        draw_manager.draw(id_bulletin_text[i], nullptr);
    }

    if (!bulletin_is_open())
        bulletin_open();

    bulletin_reset();

    bulletin_transition = transition ? 0 : 1;

    bulletin_set_bg(nullptr);

    if (!transition) {
        bulletin_bg_preserve = nullptr;
    }

    bulletin_set_active_player_flag();
}

void BulletinDrawer::bulletin_update(float dt) {
    bulletin_transition += dt * BULLETIN_TRANSITION_RATE;
    if (bulletin_transition >= 1) {
        bulletin_transition = 1;
        draw_manager.draw(
            id_bulletin_bg_scan,
            nullptr);
    }

    if (bulletin_redraws_needed && !bulletin_has_been_acknowledged) {
        bulletin_update_bg();
        bulletin_draw_text();
    }

    bulletin_redraws_needed = bulletin_transition < 1;

    if (bulletin_is_yesno) {
        SpriteClick clk = draw_manager.query_click(id_bulletin_yesno);
        if (clk.id) {
            if (clk.x < 0.5) {
                L.debug("YESNO: YES");
                bulletin_yesno_was_yes = true;
            } else {
                L.debug("YESNO: NO");
            }
            bulletin_has_been_acknowledged = true;
        }
    } else if (bulletin_is_war_ally) {
        bulletin_war_ally_update();
    } else if (draw_manager.clicked()) {
        if (bulletin_transition < 1) {
            bulletin_transition = 1;
        } else {
            bulletin_has_been_acknowledged = true;
        }
    }
}

void BulletinDrawer::bulletin_draw_text() {
    for (int i = 0; i < bulletin_text_idx; ++i) {
        if (bulletin_transition < 1) {
            draw_manager.draw(
                id_bulletin_text[i],
                nullptr);
        } else {
            draw_manager.draw_text(
                id_bulletin_text[i],
                bulletin_text[i],
                Justify::Left,
                BULLETIN_TEXT_X,
                bulletin_text_y(i),
                bulletin_text_col[i]);
        }
    }

    if (bulletin_transition >= 1) {
        if (bulletin_is_yesno) {
            draw_manager.draw(
                id_bulletin_yesno,
                IMG_BR14_EXPORT,
                {RES_X / 2,
                 BULLETIN_Y + BULLETIN_H - BULLETIN_BORDER - 2,
                 0.5f, 1, 1, 1});
        }
        if (bulletin_is_war_ally) {
            bulletin_war_ally_init();
        }
    }
}

void BulletinDrawer::bulletin_war_ally_init() {
    for (int i = 0; i < sizeof(bulletin_war_ally_ids_army); ++i) {
        bulletin_war_ally_ids_army[i] = draw_manager.new_sprite_id();
    }
    bulletin_war_ally_id_exit = draw_manager.new_sprite_id();

    for (int i = 0; i < 3; ++i) {
        const char* img = IMG_TD2_TR4;
        if (i == 1) {
            img =  IMG_TD2_TR5;
        } else if (i == 2) {
            img =  IMG_TD2_TR6;
        }

        draw_manager.draw(
            bulletin_war_ally_ids_army[i*5],
            img,
            {BULLETIN_X + 20,
             BULLETIN_Y + 164 + i*44,
             0, 0.5f, 1, 1});

        draw_manager.draw(
            bulletin_war_ally_ids_army[i*5 + 3],
            IMG_BR4_EXPORT3,
            {BULLETIN_X + BULLETIN_W - 20,
             BULLETIN_Y + 164 + i*44,
             1, 0.5f, 1, 1});
    }

    draw_manager.draw(
        bulletin_war_ally_id_exit,
        IMG_BR5_EXPORT4,
        {RES_X / 2,
         BULLETIN_Y + BULLETIN_H - BULLETIN_BORDER - 2,
         0.5f, 1, 1, 1});
}

void BulletinDrawer::bulletin_war_ally_update() {
    if (draw_manager.query_click(bulletin_war_ally_id_exit).id) {
        for (int i = 0; i < sizeof(bulletin_war_ally_ids_army); ++i) {
            draw_manager.draw(bulletin_war_ally_ids_army[i], nullptr);
            draw_manager.release_sprite_id(bulletin_war_ally_ids_army[i]);
        }
        draw_manager.draw(bulletin_war_ally_id_exit, nullptr);
        draw_manager.release_sprite_id(bulletin_war_ally_id_exit);
        bulletin_has_been_acknowledged = true;
        return;
    }

    int inf, gli, art;
    bulletin_war_ally_planet->get_army(inf, gli, art);

    char n[8];
    for (int i = 0; i < 3; ++i) {
        int stock = inf;
        int *to_send = &bulletin_war_ally_inf;
        if (i == 1) {
            stock = gli;
            to_send = &bulletin_war_ally_gli;
        } else if (i == 2) {
            stock = art;
            to_send = &bulletin_war_ally_art;
        }

        snprintf(n, sizeof(n), "%d", stock);
        draw_manager.draw_text(
            bulletin_war_ally_ids_army[i*5 + 1],
            n,
            Justify::Left,
            BULLETIN_X + 164,
            BULLETIN_Y + 152 + i*44,
            COL_TEXT);

        snprintf(n, sizeof(n), "%d", *to_send);
        draw_manager.draw_text(
            bulletin_war_ally_ids_army[i*5 + 2],
            n,
            Justify::Left,
            BULLETIN_X + 270,
            BULLETIN_Y + 152 + i*44,
            COL_TEXT);

        SpriteClick clk = draw_manager.query_click(bulletin_war_ally_ids_army[i*5 + 3]);

        if (clk.id) {
            if (clk.x < 0.5) {
                if (*to_send > 0) {
                    (*to_send)--;
                    switch (i) {
                        case 0:
                            bulletin_war_ally_planet->adjust_army(1, 0, 0);
                            break;
                        case 1:
                            bulletin_war_ally_planet->adjust_army(0, 1, 0);
                            break;
                        case 2:
                            bulletin_war_ally_planet->adjust_army(0, 0, 1);
                            break;
                    }
                }
            } else {
                if (stock > 0) {
                    switch (i) {
                        case 0:
                            bulletin_war_ally_planet->adjust_army(-1, 0, 0);
                            break;
                        case 1:
                            bulletin_war_ally_planet->adjust_army(0, -1, 0);
                            break;
                        case 2:
                            bulletin_war_ally_planet->adjust_army(0, 0, -1);
                            break;
                    }
                    (*to_send)++;
                }
            }
        }

        if (stock <= 0) {
            draw_manager.draw(
                bulletin_war_ally_ids_army[i*5 + 4],
                IMG_TD2_TR0,
                {BULLETIN_X + 20,
                 BULLETIN_Y + 164 + i*44,
                 0, 0.5f, 1, 1});
        } else {
            draw_manager.draw(bulletin_war_ally_ids_army[i*5 + 4], nullptr);
        }
    }
}

void BulletinDrawer::bulletin_set_next_text(const char* in_text, ...) {
    if (bulletin_text_idx >= BULLETIN_LINES) {
        L.error("Tried to set invalid bulletin text index %d to %s", bulletin_text_idx, in_text);
        return;
    }

    va_list args;
    va_start(args, in_text);
    vsnprintf(bulletin_text[bulletin_text_idx++], BULLETIN_MAX_TEXT - 1, in_text, args);
    va_end(args);
}

void BulletinDrawer::bulletin_set_text_col(RGB col) {
    bulletin_text_col[bulletin_text_idx] = col;
}

void BulletinDrawer::bulletin_write_planet_info(Star* s, Planet* p) {
    if (!s || !p) {
        L.fatal("Attempt to write data for null star / planet");
    }

    Player *owner = nullptr;
    if (p->is_owned()) owner = exostate.get_player(p->get_owner());

    RGB col = COL_TEXT_BAD;
    if (owner && owner->is_human())
        col = COL_TEXT2;

    bulletin_set_text_col(col);
    bulletin_set_next_text("Planet %s, System %s", p->get_name(), s->name);
    bulletin_set_next_text("");
    bulletin_set_text_col(col);
    bulletin_set_next_text("%s", owner ? owner->get_full_name() : "");
    bulletin_set_next_text("");
}

void BulletinDrawer::bulletin_open() {
    id_bulletin_header_flag = draw_manager.new_sprite_id();
    id_bulletin_header_l = draw_manager.new_sprite_id();
    id_bulletin_header_r = draw_manager.new_sprite_id();
    id_bulletin_panel = draw_manager.new_sprite_id();
    id_bulletin_black = draw_manager.new_sprite_id();
    id_bulletin_bg = draw_manager.new_sprite_id();
    id_bulletin_bg_preserve = draw_manager.new_sprite_id();
    id_bulletin_bg_scan = draw_manager.new_sprite_id();
    id_bulletin_yesno = draw_manager.new_sprite_id();

    bulletin_bg_preserve = nullptr;

    for (int i = 0; i < BULLETIN_LINES; ++i) {
        id_bulletin_text[i] = draw_manager.new_sprite_id();
    }

    draw_manager.fill(
        id_bulletin_panel,
        {BULLETIN_X, BULLETIN_Y,
         BULLETIN_W, BULLETIN_H},
         COL_BORDERS);

    bulletin_set_bg(nullptr);

    draw_manager.fill(
        id_bulletin_black,
        {BULLETIN_X + BULLETIN_BORDER,
         BULLETIN_Y + BULLETIN_BORDER,
         436, 306},
         {0, 0, 0});

    // Draw header flag and background
    draw_manager.fill(
        id_bulletin_header_flag,
        {BULLETIN_FLAG_BG_X, BULLETIN_FLAG_BG_Y,
         BULLETIN_FLAG_BG_W, BULLETIN_FLAG_BG_H},
         COL_BORDERS);

    draw_manager.draw(
        id_bulletin_header_l,
        IMG_TS1_HEADER,
        {BULLETIN_FLAG_BG_X - 2,
         BULLETIN_Y - 2,
         1, 1, 1, 1});

    draw_manager.draw(
        id_bulletin_header_r,
        IMG_TS1_HEADER,
        {BULLETIN_FLAG_BG_X + BULLETIN_FLAG_BG_W + 2,
         BULLETIN_Y - 2,
         0, 1, 1, 1});

    _bulletin_is_open = true;
}

void BulletinDrawer::bulletin_close() {
    if (!bulletin_is_open()) {
        L.fatal("Attempt to close bulletin whilst not open");
    }

    for (int i = 0; i < BULLETIN_LINES; ++i) {
        draw_manager.release_sprite_id(id_bulletin_text[i]);
    }

    draw_manager.draw(id_bulletin_panel, nullptr);
    draw_manager.draw(id_bulletin_black, nullptr);
    draw_manager.draw(id_bulletin_header_flag, nullptr);
    draw_manager.draw(id_bulletin_header_l, nullptr);
    draw_manager.draw(id_bulletin_header_r, nullptr);
    draw_manager.draw(id_bulletin_bg, nullptr);
    draw_manager.draw(id_bulletin_bg_preserve, nullptr);
    draw_manager.draw(id_bulletin_bg_scan, nullptr);

    draw_manager.release_sprite_id(id_bulletin_header_flag);
    draw_manager.release_sprite_id(id_bulletin_header_l);
    draw_manager.release_sprite_id(id_bulletin_header_r);
    draw_manager.release_sprite_id(id_bulletin_panel);
    draw_manager.release_sprite_id(id_bulletin_black);
    draw_manager.release_sprite_id(id_bulletin_bg);
    draw_manager.release_sprite_id(id_bulletin_bg_preserve);
    draw_manager.release_sprite_id(id_bulletin_bg_scan);
    draw_manager.release_sprite_id(id_bulletin_yesno);


    // Wipe all info
    bulletin_reset();

    _bulletin_is_open = false;
}

void BulletinDrawer::bulletin_reset() {
    for (int i = 0; i < BULLETIN_LINES; ++i) {
        strncpy(bulletin_text[i], "", 1);
    }

    bulletin_reset_text_cols();
    bulletin_transition = 0;
    bulletin_redraws_needed = true;
    bulletin_has_been_acknowledged = false;
    bulletin_text_idx = 0;
    bulletin_is_yesno = false;
    bulletin_yesno_was_yes = false;

    bulletin_is_war_ally = false;

    // N.B. Don't reset war ally vars here as we need to retrieve them later
}

void BulletinDrawer::bulletin_ensure_closed() {
    if (bulletin_is_open()) bulletin_close();
}

bool BulletinDrawer::bulletin_is_open() {
    return _bulletin_is_open;
}

int BulletinDrawer::bulletin_text_y(int idx) {
    return BULLETIN_Y + BULLETIN_BORDER + 4 + idx * 18;
}

void BulletinDrawer::bulletin_reset_text_cols() {
    for (int i = 0; i < BULLETIN_LINES; ++i) {
        bulletin_text_col[i] = COL_TEXT;
    }
}

bool BulletinDrawer::bulletin_acknowledged() {
    return bulletin_has_been_acknowledged;
}

bool BulletinDrawer::bulletin_was_yesno_yes() {
    return bulletin_yesno_was_yes;
}

void BulletinDrawer::bulletin_set_bg(const char* img) {
    // N.B. Can set nullptr for black
    bulletin_bg = img;
}

void BulletinDrawer::bulletin_set_yesno() {
    bulletin_is_yesno = true;
    bulletin_yesno_was_yes = false;
}

void BulletinDrawer::bulletin_set_war_ally(Planet* p, int mc) {
    bulletin_is_war_ally = true;
    bulletin_war_ally_planet = p;
    bulletin_war_ally_mc = mc;
    bulletin_war_ally_inf = 0;
    bulletin_war_ally_gli = 0;
    bulletin_war_ally_art = 0;
}

int BulletinDrawer::bulletin_get_war_ally_result(int& i, int& g, int& a) {
    i = bulletin_war_ally_inf;
    g = bulletin_war_ally_gli;
    a = bulletin_war_ally_art;
    return bulletin_war_ally_mc;
}

void BulletinDrawer::bulletin_update_bg() {
    int h = 306;
    if (bulletin_transition < 1) {
        h = (int)(306.f * bulletin_transition);
    } else {
        bulletin_bg_preserve = bulletin_bg;
    }

    if (bulletin_bg) {
        if (bulletin_bg_preserve && bulletin_transition < 1) {
            /* FIXME
             * Without removing bulletin_bg, there is a flicker
             * to black before each next bulletin starts. This
             * is because when bulletin_bg is first drawn for
             * the next bulletin, its source region is small
             * but the draw repairs the larger area that it
             * occupied on the previous bulletin - where the
             * preserved bg has now been drawn - and as it's
             * 'below' the preserved background, this doesn't
             * get restored.
             *
             * We're in an awkward situation whereby the
             * background and preseved background are alternately
             * to be displayed on top of one another. The proper
             * solution would likely be to have repairs also restore
             * sprites above their previous location - however
             * we work around the issue here by removing the
             * bulletin background - thereby getting the repair
             * out of the way - before drawing the preserved
             * background.
             */
            draw_manager.draw(id_bulletin_bg, nullptr);
            draw_manager.draw(
                id_bulletin_bg_preserve,
                bulletin_bg_preserve,
                {BULLETIN_BG_X,
                 BULLETIN_BG_Y,
                 0, 0, 1, 1});
        }
        DrawArea a = {0, 0, 436, h};
        draw_manager.set_source_region(
            id_bulletin_bg,
            &a);
        draw_manager.draw(
            id_bulletin_bg,
            bulletin_bg,
            {BULLETIN_BG_X,
             BULLETIN_BG_Y,
             0, 0, 1, 1});
    } else {
        // nullptr -> black background
        draw_manager.fill(
            id_bulletin_black,
            {BULLETIN_BG_X,
             BULLETIN_BG_Y,
             436, h},
             {0, 0, 0});
    }

    // Ensure we don't draw lower then the bg area
    if (bulletin_transition < 1 && h < 434) {
        draw_manager.fill(
            id_bulletin_bg_scan,
            {BULLETIN_BG_X, BULLETIN_BG_Y + h, 436, 2},
             {0, 0, 0xFF});
    } else {
        draw_manager.draw(
            id_bulletin_bg_scan,
            nullptr);
    }
}

void BulletinDrawer::bulletin_set_flag(const char* img) {
    draw_manager.draw(
        img,
        {BULLETIN_FLAG_BG_X + BULLETIN_BORDER,
         BULLETIN_FLAG_BG_Y + BULLETIN_BORDER,
         0, 0, 1, 1});
}

void BulletinDrawer::bulletin_set_player_flag(Player* player) {
    bulletin_set_flag(flags[player->get_flag_idx()]);
}

void BulletinDrawer::bulletin_set_active_player_flag() {
    bulletin_set_player_flag(exostate.get_active_player());
}
