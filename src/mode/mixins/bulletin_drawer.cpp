#include "bulletin_drawer.h"

#include "assetpaths.h"
#include "shared.h"

const int BULLETIN_BORDER = 6;
const int BULLETIN_W = 436 + (BULLETIN_BORDER) * 2;
const int BULLETIN_H = 306 + (BULLETIN_BORDER) * 2;
const int BULLETIN_X = (RES_X / 2) - (BULLETIN_W / 2);
const int BULLETIN_Y = (RES_Y / 2) - (BULLETIN_H / 2) - 12;
const int BULLETIN_TEXT_X = BULLETIN_X + BULLETIN_BORDER * 2 + 8;
const int BULLETIN_FLAG_BG_X = (RES_X / 2) - 48 - BULLETIN_BORDER;
const int BULLETIN_FLAG_BG_Y = BULLETIN_Y - 2 - 56 - BULLETIN_BORDER*2;
const int BULLETIN_FLAG_BG_W = 96 + BULLETIN_BORDER*2;
const int BULLETIN_FLAG_BG_H = 56 + BULLETIN_BORDER*2;

BulletinDrawer::BulletinDrawer() {
    for (int i = 0; i < BULLETIN_LINES; ++i) {
        strncpy(bulletin_text[i], "", 1);
    }
}

void BulletinDrawer::bulletin_update(float dt) {
}

void BulletinDrawer::bulletin_draw_text() {
    for (int i = 0; i < BULLETIN_LINES; ++i) {
        draw_manager.draw_text(
            id_bulletin_text[i],
            bulletin_text[i],
            Justify::Left,
            BULLETIN_TEXT_X,
            bulletin_text_y(i),
            COL_TEXT);
    }
}

void BulletinDrawer::bulletin_set_text(int idx, const char* in_text) {
    bulletin_vset_text(idx, in_text);
}

void BulletinDrawer::bulletin_vset_text(int idx, const char* in_text, ...) {
    if (idx >= BULLETIN_LINES) {
        L.fatal("Tried to set invalid bulletin text index %d to %s", idx, in_text);
    }

    va_list args;
    va_start(args, in_text);
    vsnprintf(bulletin_text[idx], BULLETIN_MAX_TEXT - 1, in_text, args);
    va_end(args);
}

void BulletinDrawer::bulletin_open() {
    Player *player = exostate.get_active_player();

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
    draw_manager.fill(
        {BULLETIN_X + BULLETIN_BORDER,
         BULLETIN_Y + BULLETIN_BORDER,
         436, 306},
         {0, 0, 0});

    draw_manager.save_background({BULLETIN_X, BULLETIN_Y,
                                  BULLETIN_W, BULLETIN_H});

    // Draw header flag and background
    // TODO: This is not always the player flag!
    draw_manager.fill(
        id_bulletin_header_flag,
        {BULLETIN_FLAG_BG_X, BULLETIN_FLAG_BG_Y,
         BULLETIN_FLAG_BG_W, BULLETIN_FLAG_BG_H},
         COL_BORDERS);
    draw_manager.draw(
        flags[player->get_flag_idx()],
        {BULLETIN_FLAG_BG_X + BULLETIN_BORDER,
         BULLETIN_FLAG_BG_Y + BULLETIN_BORDER,
         0, 0, 1, 1});

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

    bulletin_draw_text();

    _bulletin_is_open = true;
}

void BulletinDrawer::bulletin_close() {
    if (!_bulletin_is_open) {
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


    _bulletin_is_open = false;

    // Wipe all info
    for (int i = 0; i < BULLETIN_LINES; ++i) {
        strncpy(bulletin_text[i], "", 1);
    }
}

bool BulletinDrawer::bulletin_is_open() {
    return _bulletin_is_open;
}

int BulletinDrawer::bulletin_text_y(int idx) {
    return BULLETIN_Y + BULLETIN_BORDER + 8 + idx * 30;
}
