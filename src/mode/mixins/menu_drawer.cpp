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
const int MENU_TEXT_X = MENU_X + MENU_BORDER * 2 + 2;
const int MENU_FLAG_BG_X = (RES_X / 2) - 48 - MENU_BORDER;
const int MENU_FLAG_BG_Y = MENU_Y - 2 - 56 - MENU_BORDER*2;
const int MENU_FLAG_BG_W = 96 + MENU_BORDER*2;
const int MENU_FLAG_BG_H = 56 + MENU_BORDER*2;
const int MENU_BG_X = MENU_X + MENU_BORDER;
const int MENU_BG_Y = MENU_Y + MENU_BORDER;

MenuDrawer::MenuDrawer() {
    _menu_is_open = false;
}

bool MenuDrawer::menu_is_open() {
    return _menu_is_open;
}

void MenuDrawer::menu_open(MenuMode mode) {
    menu_mode = mode;

    for (int i = 0; i < MENU_LINES; ++i) {
        strncpy(menu_text[i], "", 1);
    }

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

    menu_open_specific_mode();

    _menu_is_open = true;
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

    _menu_is_open = false;
}

void MenuDrawer::menu_update(float delta) {
    for (int i = 0; i < MENU_LINES; ++i) {
        draw_manager.draw_text(
            id_menu_lines[i],
            menu_text[i],
            Justify::Left,
            MENU_TEXT_X,
            MENU_Y + MENU_BORDER + 2 + i * 20,
            menu_text_col[i]);
    }

    menu_specific_update();
}

void MenuDrawer::menu_set_txt(int idx, RGB col, const char* in_text, ...) {
    if (idx >= MENU_LINES) {
        L.error("Tried to set invalid menu text index %d to %s", idx, in_text);
        return;
    }

    va_list args;
    va_start(args, in_text);
    vsnprintf(menu_text[idx], MENU_MAX_TEXT - 1, in_text, args);
    va_end(args);

    menu_text_col[idx] = col;
}

void MenuDrawer::menu_set_opt(int idx, const char* in_text, ...) {
    if (idx >= MENU_LINES) {
        L.error("Tried to set invalid menu option index %d to %s", idx, in_text);
        return;
    }

    va_list args;
    va_start(args, in_text);
    vsnprintf(menu_text[idx], MENU_MAX_TEXT - 1, in_text, args);
    va_end(args);

    menu_text_col[idx] = COL_TEXT;
    draw_manager.set_selectable(id_menu_lines[idx]);
}

void MenuDrawer::menu_open_specific_mode() {
    Player *p = exostate.get_active_player();
    int art_cost = 1000;
    bool art_ok = true;

    if (!p->can_afford(art_cost)) art_ok = false;
    if (!p->has_invention(INV_OrbitalMassConstruction)) art_ok = false;
    // TODO: Also check that the player doesn't have an artificial planet
    // (or rather, whatever pphase != 3 implies).

    switch(menu_mode) {
        case MM_Ctrl:
            menu_set_txt(0, COL_TEXT2, "Please select, %s", p->get_ref());
            menu_set_opt(2, "Change Officers & Taxes");
            menu_set_opt(3, "Secret Service");
            // TODO: Hotkey
            menu_set_opt(4, "Set / Replace Star Markers (M)");
            // TODO: Hotkey
            menu_set_opt(5, "Equip Starship (E)");
            if (art_ok) {
                menu_set_opt(6, "Build Artificial Planet (%dMC)", art_cost);
            } else {
                menu_set_txt(6, COL_TEXT_GREYED, "Build Artificial Planet (%dMC)", art_cost);
            }
            menu_set_opt(8, "Wait One Month (Spc)");
            // TODO: Hotkey
            menu_set_opt(10, "Show Distances (D)");
            // TODO: Hotkey
            menu_set_opt(11, "Save Game (S for last)");
            menu_set_opt(12, "Quit Game");
            menu_set_opt(14, "Exit Menu");
            break;
        case MM_OfficersAndTaxes:
            break;
        case MM_NewOfficer:
            break;
        case MM_SecretService:
            break;
        case MM_StarMarker:
            break;
        case MM_EquipShip:
            break;
        case MM_ArtificialWorld:
            break;
        case MM_Save:
            break;
        case MM_Stat:
            break;
        case MM_GenInfo:
            break;
        case MM_FleetInfo:
            break;
        case MM_News:
            break;
        case MM_Planets:
            break;
        case MM_Inventions:
            break;
        case MM_ShipStatus:
            break;
        case MM_ShipDamage:
            break;
        case MM_Stats:
            break;
    }
}

void MenuDrawer::menu_specific_update() {
    Player *p = exostate.get_active_player();

    switch(menu_mode) {
        case MM_Ctrl:
            // 2: Change Officers & Taxes
            // 3: Secret Service
            // 4: Set / Replace Star Markers
            // 5: Equip Starship
            // 6: Build Artificial Planet
            // 8: Wait One Month
            // 10: Show Distances
            // 11: Save Game
            // 12: Quit Game
            // 14: Exit Menu
            if (draw_manager.query_click(id_menu_lines[14]).id) {
                menu_close();
            }
            break;
        case MM_OfficersAndTaxes:
            break;
        case MM_NewOfficer:
            break;
        case MM_SecretService:
            break;
        case MM_StarMarker:
            break;
        case MM_EquipShip:
            break;
        case MM_ArtificialWorld:
            break;
        case MM_Save:
            break;
        case MM_Stat:
            break;
        case MM_GenInfo:
            break;
        case MM_FleetInfo:
            break;
        case MM_News:
            break;
        case MM_Planets:
            break;
        case MM_Inventions:
            break;
        case MM_ShipStatus:
            break;
        case MM_ShipDamage:
            break;
        case MM_Stats:
            break;
    }
}
