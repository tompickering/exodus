#ifndef GUARD_MENU_DRAWER_H
#define GUARD_MENU_DRAWER_H

#include "draw/draw.h"

#include "state/exodus_state.h"

#define MENU_LINES     15
#define MENU_MAX_TEXT 256
#define MENU_N_OFFICERS 5


#define COST_INF_MC       20
#define COST_INF_ALLIES   20
#define COST_INF_NPLANS   20
#define COST_INF_INV      50
#define COST_INF_SURF    100

#define COST_FILE        100

#define COST_ATT_CMD     200
#define COST_ATT_AGRI    200
#define COST_ATT_PLU     150
#define COST_ATT_ARMY     70
#define COST_ATT_PORT    100
#define COST_ATT_TRADE   100
#define COST_ATT_MINE     50

#define COST_ATT_BOMB    500
#define COST_ATT_NUKE   1000

#define OFF_PAY_MC        50

#define COST_ART        1000


extern DRAWMANAGER draw_manager;

extern ExodusState exostate;

enum MenuMode {
    MM_Ctrl,
    MM_OfficersAndTaxes,
    MM_NewOfficer,
    MM_OldOfficer,
    MM_OldOfficerPaid,
    MM_OldOfficerDismissed,
    MM_OldOfficerKilled,
    MM_SecretService,
    MM_SecInfo,
    MM_PlayerChoice,
    MM_SecInfoMC,
    MM_SecInfoAllies,
    MM_SecInfoPlanets,
    MM_SecInfoInventions,
    MM_SecPersonalFile,
    MM_SecAttack,
    MM_SecBombing,
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
    MM_Invention,
    MM_Relations,
    MM_ShipStatus,
    MM_ShipDamage,
    MM_Stats,
};

enum MenuAction {
    MA_None,
    MA_EquipShip,
    MA_BuildArtificialWorld,
    MA_Close,
};

class MenuDrawer {
    public:
        MenuDrawer();
        void menu_open(MenuMode);
        void menu_open_invention(Invention);
        ExodusMode menu_update(float);
        void menu_close();
        bool menu_is_open();
        MenuAction menu_get_action();
    private:
        MenuMode menu_mode;

        ExodusMode menu_new_mode;

        SprID id_menu_lines[MENU_LINES];
        SprID id_menu_header_flag_bg;
        SprID id_menu_header_flag;
        SprID id_menu_header_l;
        SprID id_menu_header_r;
        SprID id_menu_panel;
        SprID id_menu_bg;
        SprID id_menu_sci;
        SprID id_menu_tax;
        SprID id_menu_scimore;
        SprID id_menu_taxmore;
        SprID id_menu_newoff[MENU_N_OFFICERS];
        SprID id_menu_newoff_opt;
        SprID id_menu_artplanname;
        char menu_text[MENU_LINES][MENU_MAX_TEXT];
        RGB menu_text_col[MENU_LINES];
        bool _menu_is_open;
        MenuAction menu_action;
        Officer menu_new_officer;
        OfficerQuality menu_new_officer_quality;

        bool menu_row_clicked(int);
        int menu_clickable_rows;

        // Use this to open player select
        // Given mode will be opened following choice,
        // with menu_selected_player set
        void menu_open_player_select(MenuMode);
        Player* menu_selected_player;
        int menu_line_players[MENU_LINES];
        void menu_print_other_players();
        bool menu_player_selected();
        MenuMode menu_player_select_mode;

        bool first_update;
        int menu_get_y(int);
        const char* menu_get_bg();
        void menu_set_txt(int, RGB, const char*, ...);
        void menu_set_opt(int, const char*, ...);
        void menu_set_opt(int, const char*, bool);
        void menu_open_specific_mode();
        bool menu_specific_update();

        Invention menu_inv;
        int menu_art_planet_phase;
        bool menu_art_planet_named;

        int menu_planets_start;
        bool menu_planets_more;
};

#endif
