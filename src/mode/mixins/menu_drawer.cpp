#include "menu_drawer.h"

#include "state/exodus_state.h"

#include "shared.h"
#include "assetpaths.h"

extern ExodusState exostate;

const int MENU_BORDER = 6;
const int MENU_W = 436 + (MENU_BORDER) * 2;
const int MENU_H = 306 + (MENU_BORDER) * 2;
const int MENU_X = (RES_X / 2) - (MENU_W / 2);
const int MENU_Y = (RES_Y / 2) - (MENU_H / 2) - 12;
const int MENU_TEXT_X = MENU_X + MENU_BORDER * 2 + 4;
const int MENU_FLAG_BG_X = (RES_X / 2) - 48 - MENU_BORDER;
const int MENU_FLAG_BG_Y = MENU_Y - 2 - 56 - MENU_BORDER*2;
const int MENU_FLAG_BG_W = 96 + MENU_BORDER*2;
const int MENU_FLAG_BG_H = 56 + MENU_BORDER*2;
const int MENU_BG_X = MENU_X + MENU_BORDER;
const int MENU_BG_Y = MENU_Y + MENU_BORDER;

MenuDrawer::MenuDrawer() {
    menu_is_open = false;

    for (int i = 0; i < MENU_LINES; ++i) {
        strncpy(menu_text[i], "", 1);
    }
}

void MenuDrawer::menu_open(MenuMode mode) {
    menu_mode = mode;

    id_menu_header_flag_bg = draw_manager.new_sprite_id();
    id_menu_header_flag    = draw_manager.new_sprite_id();
    id_menu_header_l       = draw_manager.new_sprite_id();
    id_menu_header_r       = draw_manager.new_sprite_id();
    id_menu_panel          = draw_manager.new_sprite_id();
    id_menu_bg             = draw_manager.new_sprite_id();

    for (int i = 0; i < MENU_LINES; ++i) {
        id_menu_lines[i] = draw_manager.new_sprite_id();
    }

    draw_manager.fill(
        id_menu_panel,
        {MENU_X, MENU_Y,
         MENU_W, MENU_H},
        COL_BORDERS);

    draw_manager.draw(
        id_menu_bg,
        IMG_ME1_MENU,
        {MENU_X + MENU_BORDER,
         MENU_Y + MENU_BORDER,
         0, 0, 1, 1});

    // Draw header flag and background
    draw_manager.fill(
        id_menu_header_flag_bg,
        {MENU_FLAG_BG_X, MENU_FLAG_BG_Y,
         MENU_FLAG_BG_W, MENU_FLAG_BG_H},
         COL_BORDERS);

    draw_manager.draw(
        id_menu_header_flag,
        flags[exostate.get_active_player()->get_flag_idx()],
        {MENU_FLAG_BG_X + MENU_BORDER,
         MENU_FLAG_BG_Y + MENU_BORDER,
         0, 0, 1, 1});

    draw_manager.draw(
        id_menu_header_l,
        IMG_TS1_HEADER,
        {MENU_FLAG_BG_X - 2,
         MENU_Y - 2,
         1, 1, 1, 1});

    draw_manager.draw(
        id_menu_header_r,
        IMG_TS1_HEADER,
        {MENU_FLAG_BG_X + MENU_FLAG_BG_W + 2,
         MENU_Y - 2,
         0, 1, 1, 1});

    menu_is_open = true;
}

void MenuDrawer::menu_close() {
    for (int i = 0; i < MENU_LINES; ++i) {
        draw_manager.draw(id_menu_lines[i], nullptr);
        draw_manager.release_sprite_id(id_menu_lines[i]);
    }

    draw_manager.draw(id_menu_header_flag_bg, nullptr);
    draw_manager.draw(id_menu_header_flag, nullptr);
    draw_manager.draw(id_menu_header_l, nullptr);
    draw_manager.draw(id_menu_header_r, nullptr);
    draw_manager.draw(id_menu_panel, nullptr);
    draw_manager.draw(id_menu_bg, nullptr);

    draw_manager.release_sprite_id(id_menu_header_flag_bg);
    draw_manager.release_sprite_id(id_menu_header_flag);
    draw_manager.release_sprite_id(id_menu_header_l);
    draw_manager.release_sprite_id(id_menu_header_r);
    draw_manager.release_sprite_id(id_menu_panel);
    draw_manager.release_sprite_id(id_menu_bg);

    menu_is_open = false;
}

void MenuDrawer::menu_update(float delta) {
}
