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
    _bulletin_is_open = false;
}

void BulletinDrawer::bulletin_start_new(bool transition) {
    if (!bulletin_is_open())
        bulletin_open();

    bulletin_reset();

    bulletin_transition = transition ? 0 : 1;

    bulletin_set_bg(nullptr);
    bulletin_set_active_player_flag();
}

void BulletinDrawer::bulletin_update(float dt) {
    bulletin_transition += dt * BULLETIN_TRANSITION_RATE;
    if (bulletin_transition > 1) bulletin_transition = 1;

    if (bulletin_redraws_needed && !bulletin_has_been_acknowledged) {
        bulletin_update_bg();
        bulletin_draw_text();
    }

    bulletin_redraws_needed = bulletin_transition < 1;

    if (draw_manager.clicked()) {
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
    bulletin_vset_next_text("Planet %s, System %s", p->get_name(), s->name);
    bulletin_set_next_text("");
    bulletin_set_text_col(col);
    bulletin_vset_next_text("%s", owner ? owner->get_full_name() : "");
    bulletin_set_next_text("");
}

void BulletinDrawer::bulletin_open() {
    id_bulletin_header_flag = draw_manager.new_sprite_id();
    id_bulletin_header_l = draw_manager.new_sprite_id();
    id_bulletin_header_r = draw_manager.new_sprite_id();
    id_bulletin_panel = draw_manager.new_sprite_id();
    id_bulletin_bg = draw_manager.new_sprite_id();
    id_bulletin_bg_scan = draw_manager.new_sprite_id();

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
    draw_manager.draw(id_bulletin_header_flag, nullptr);
    draw_manager.draw(id_bulletin_header_l, nullptr);
    draw_manager.draw(id_bulletin_header_r, nullptr);

    draw_manager.release_sprite_id(id_bulletin_header_flag);
    draw_manager.release_sprite_id(id_bulletin_header_l);
    draw_manager.release_sprite_id(id_bulletin_header_r);
    draw_manager.release_sprite_id(id_bulletin_panel);
    draw_manager.release_sprite_id(id_bulletin_bg);
    draw_manager.release_sprite_id(id_bulletin_bg_scan);


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
    // N.B. Can set nullptr for black
    bulletin_bg = img;
}

void BulletinDrawer::bulletin_update_bg() {
    int h = 306;
    if (bulletin_transition < 1) h = (int)(306.f * bulletin_transition);
    if (bulletin_bg) {
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
            {BULLETIN_BG_X,
             BULLETIN_BG_Y,
             436, h},
             {0, 0, 0});
    }

    if (bulletin_transition == 1) {
        // The above will have drawn over the last scan bar
        draw_manager.save_background({BULLETIN_X, BULLETIN_Y,
                                      BULLETIN_W, BULLETIN_H});
    } else if (h < 434) { // Ensures we don't draw lower then the bg area
        draw_manager.fill(
            id_bulletin_bg_scan,
            {BULLETIN_BG_X, BULLETIN_BG_Y + h, 436, 2},
             {0, 0, 0xFF});
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
