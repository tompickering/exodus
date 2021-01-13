#include "menu_drawer.h"

#include "state/exodus_state.h"
#include "save/save.h"

#include "shared.h"
#include "assetpaths.h"

extern ExodusState exostate;
extern SAVEMANAGER save_manager;

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

MenuAction MenuDrawer::menu_get_action() {
    return menu_action;
}

void MenuDrawer::menu_open(MenuMode mode) {
    menu_mode = mode;
    menu_action = MA_None;

    for (int i = 0; i < MENU_LINES; ++i) {
        strncpy(menu_text[i], "", 1);
        draw_manager.unset_selectable(id_menu_lines[i]);
    }

    if (!_menu_is_open) {
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
    }

    draw_manager.draw(
        id_menu_bg,
        menu_get_bg(),
        {MENU_X + MENU_BORDER,
         MENU_Y + MENU_BORDER,
         0, 0, 1, 1});

    menu_open_specific_mode();

    _menu_is_open = true;
}

void MenuDrawer::menu_close() {
    menu_action = MA_None;

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
    menu_action = MA_None;

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

const char* MenuDrawer::menu_get_bg() {
    switch (menu_mode) {
        case MM_Save:
            return IMG_ME2_MENU;
        default:
            return IMG_ME1_MENU;
    }
    return IMG_ME1_MENU;
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
            menu_set_txt(0, COL_TEXT2, "Secret Service");
            menu_set_txt(2, COL_TEXT, "Please choose:");
            menu_set_opt(4, "Information Services (20+ MC)");
            menu_set_opt(5, "A Personal File (100 MC)");
            menu_set_opt(6, "Terrorist Attacks (50+ MC)");
            menu_set_opt(7, "Orbital Bomb Attacks (500+ MC)");
            menu_set_opt(14, "Exit Menu");
            break;
        case MM_StarMarker:
            break;
        case MM_EquipShip:
            break;
        case MM_ArtificialWorld:
            break;
        case MM_Save: {
            const SaveMeta *meta = save_manager.get_all_meta(true);
            menu_set_txt(0, COL_TEXT2, "Save game in slot:");
            for (int i = 0; i < MAX_SLOTS; ++i) {
                if (meta[i].exists) {
                    // TODO: Position these elements nicely
                    menu_set_opt(i+2, "%d: %s / Month %d / Planets %d",
                                 i+1, meta[i].name, meta[i].month, meta[i].planets);
                } else {
                    menu_set_opt(i+2, "%d", i+1);
                }
            }
            menu_set_opt(14, "Exit Menu");
            break;
            }
        case MM_Stat:
            menu_set_txt(0, COL_TEXT2, "Please select, %s", p->get_ref());
            menu_set_opt(2, "General Information");
            menu_set_opt(3, "Fleet Information");
            // TODO: Hotkey
            menu_set_opt(4, "Recall Latest News (R)");
            menu_set_opt(5, "List Planets");
            menu_set_opt(6, "List Inventions");
            menu_set_opt(7, "Relationship to Lords");
            menu_set_opt(9, "Starship Status");
            menu_set_opt(10, "Starship Damage");
            menu_set_opt(12, "Statistics");
            menu_set_opt(14, "Exit Menu");
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
            if (draw_manager.query_click(id_menu_lines[3]).id) {
                menu_open(MM_SecretService);
            }
            // 4: Set / Replace Star Markers
            // 5: Equip Starship
            if (draw_manager.query_click(id_menu_lines[5]).id) {
                menu_action = MA_EquipShip;
            }
            // 6: Build Artificial Planet
            // 8: Wait One Month
            // 10: Show Distances
            // 11: Save Game
            if (draw_manager.query_click(id_menu_lines[11]).id) {
                // Re-open save menu to refresh metadata
                menu_open(MM_Save);
            }
            // 12: Quit Game
            // 14: Exit Menu
            if (draw_manager.query_click(id_menu_lines[14]).id) {
                menu_action = MA_Close;
            }
            break;
        case MM_OfficersAndTaxes:
            break;
        case MM_NewOfficer:
            break;
        case MM_SecretService:
            // 4: Information Services (20+ MC)
            // 5: A Personal File (100 MC)
            // 6: Terrorist Attacks (50+ MC)
            // 7: Orbital Bomb Attacks (500+ MC)
            // 14: Exit Menu
            if (draw_manager.query_click(id_menu_lines[14]).id) {
                menu_open(MM_Ctrl);
            }
            break;
        case MM_StarMarker:
            break;
        case MM_EquipShip:
            break;
        case MM_ArtificialWorld:
            break;
        case MM_Save:
            for (int i = 0; i < MAX_SLOTS; ++i) {
                if (draw_manager.query_click(id_menu_lines[i+2]).id) {
                    save_manager.save(i);
                    menu_open(MM_Save);
                }
            }
            if (draw_manager.query_click(id_menu_lines[14]).id) {
                menu_open(MM_Ctrl);
            }
            break;
        case MM_Stat:
            // 2: General Information
            // 3: Fleet Information
            // 4: Recall Latest News
            // 5: List Planets
            // 6: List Inventions
            // 7: Relationship to Lords
            // 9: Starship Status
            // 10: Starship Damage
            // 12: Statistics
            // 14: Exit Menu
            if (draw_manager.query_click(id_menu_lines[14]).id) {
                menu_action = MA_Close;
            }
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
