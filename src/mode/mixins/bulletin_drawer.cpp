#include "bulletin_drawer.h"

#include "assetpaths.h"
#include "shared.h"

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

BulletinDrawer::BulletinDrawer() {
    for (int i = 0; i < BULLETIN_LINES; ++i) {
        strncpy(bulletin_text[i], "", 1);
    }
    bulletin_reset_text_cols();
    bulletin_has_been_acknowledged = false;
    bulletin_text_idx = 0;
    _bulletin_is_open = false;
}

void BulletinDrawer::bulletin_start_new() {
    if (!bulletin_is_open())
        bulletin_open();

    bulletin_has_been_acknowledged = false;
    bulletin_text_idx = 0;
    bulletin_reset_text_cols();

    bulletin_set_bg(nullptr);
    bulletin_set_active_player_flag();
}

void BulletinDrawer::bulletin_update(float dt) {
    if (draw_manager.clicked()) {
        bulletin_has_been_acknowledged = true;
    }

    if (!bulletin_has_been_acknowledged) {
        bulletin_draw_text();
    }
}

void BulletinDrawer::bulletin_draw_text() {
    for (int i = 0; i < BULLETIN_LINES; ++i) {
        draw_manager.draw_text(
            id_bulletin_text[i],
            bulletin_text[i],
            Justify::Left,
            BULLETIN_TEXT_X,
            bulletin_text_y(i),
            bulletin_text_col[i]);
    }
}

void BulletinDrawer::bulletin_set_next_text(const char* in_text) {
    bulletin_vset_next_text(in_text);
}

void BulletinDrawer::bulletin_vset_next_text(const char* in_text, ...) {
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

void BulletinDrawer::bulletin_open() {
    id_bulletin_header_flag = draw_manager.new_sprite_id();
    id_bulletin_header_l = draw_manager.new_sprite_id();
    id_bulletin_header_r = draw_manager.new_sprite_id();
    id_bulletin_panel = draw_manager.new_sprite_id();
    id_bulletin_bg = draw_manager.new_sprite_id();
    id_bulletin_new_bg = draw_manager.new_sprite_id();

    for (int i = 0; i < BULLETIN_LINES; ++i) {
        id_bulletin_text[i] = draw_manager.new_sprite_id();
    }

    draw_manager.fill(
        id_bulletin_panel,
        {BULLETIN_X, BULLETIN_Y,
         BULLETIN_W, BULLETIN_H},
         COL_BORDERS);

    bulletin_set_bg(nullptr);

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
    bulletin_start_new();
}

void BulletinDrawer::bulletin_close() {
    if (!bulletin_is_open()) {
        L.fatal("Attempt to close bulletin whilst not open");
    }

    for (int i = 0; i < BULLETIN_LINES; ++i) {
        draw_manager.release_sprite_id(id_bulletin_text[i]);
    }

    draw_manager.draw(id_bulletin_panel, nullptr);
    draw_manager.draw(id_bulletin_header_flag, nullptr);
    draw_manager.draw(id_bulletin_header_l, nullptr);
    draw_manager.draw(id_bulletin_header_r, nullptr);

    draw_manager.release_sprite_id(id_bulletin_header_flag);
    draw_manager.release_sprite_id(id_bulletin_header_l);
    draw_manager.release_sprite_id(id_bulletin_header_r);
    draw_manager.release_sprite_id(id_bulletin_panel);
    draw_manager.release_sprite_id(id_bulletin_bg);
    draw_manager.release_sprite_id(id_bulletin_new_bg);


    bulletin_has_been_acknowledged = false;
    bulletin_text_idx = 0;

    // Wipe all info
    for (int i = 0; i < BULLETIN_LINES; ++i) {
        strncpy(bulletin_text[i], "", 1);
    }

    bulletin_reset_text_cols();

    _bulletin_is_open = false;
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

void BulletinDrawer::bulletin_set_bg(const char* img) {
    if (img) {
        draw_manager.draw(
            img,
            {BULLETIN_X + BULLETIN_BORDER,
             BULLETIN_Y + BULLETIN_BORDER,
             0, 0, 1, 1});
    } else {
        // nullptr -> black background
        draw_manager.fill(
            {BULLETIN_X + BULLETIN_BORDER,
             BULLETIN_Y + BULLETIN_BORDER,
             436, 306},
             {0, 0, 0});
    }

    draw_manager.save_background({BULLETIN_X, BULLETIN_Y,
                                  BULLETIN_W, BULLETIN_H});
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
