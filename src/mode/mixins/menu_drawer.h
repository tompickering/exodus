#ifndef GUARD_MENU_DRAWER_H
#define GUARD_MENU_DRAWER_H

#include "draw/draw.h"

#include "state/exodus_state.h"

#define MENU_LINES     15
#define MENU_MAX_TEXT 256

extern DRAWMANAGER draw_manager;

extern ExodusState exostate;

enum MenuMode {
    MM_Ctrl,
    MM_OfficersAndTaxes,
    MM_NewOfficer,
    MM_SecretService,
    MM_StarMarker,
    MM_EquipShip,
    MM_ArtificialWorld,
    MM_Save,
    MM_Stat,
    MM_GenInfo,
    MM_FleetInfo,
    MM_News,
    MM_Planets,
    MM_Inventions,
    MM_ShipStatus,
    MM_ShipDamage,
    MM_Stats,
};

class MenuDrawer {
    public:
        MenuDrawer();
        void menu_open(MenuMode);
        void menu_update(float);
        void menu_close();
        bool menu_is_open();
    private:
        MenuMode menu_mode;

        SprID id_menu_lines[MENU_LINES];
        SprID id_menu_header_flag_bg;
        SprID id_menu_header_flag;
        SprID id_menu_header_l;
        SprID id_menu_header_r;
        SprID id_menu_panel;
        SprID id_menu_bg;
        char menu_text[MENU_LINES][MENU_MAX_TEXT];
        RGB menu_text_col[MENU_LINES];
        bool _menu_is_open;

        const char* menu_get_bg();
        void menu_set_txt(int, RGB, const char*, ...);
        void menu_set_opt(int, const char*, ...);
        void menu_open_specific_mode();
        void menu_specific_update();
};

#endif
