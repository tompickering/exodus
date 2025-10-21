#ifndef GUARD_BULLETIN_DRAWER_H
#define GUARD_BULLETIN_DRAWER_H

#include "draw/draw.h"

#include "anim.h"
#include "galaxy/star.h"
#include "galaxy/planet.h"
#include "state/exodus_state.h"

#define BULLETIN_LINES     20
#define BULLETIN_MAX_TEXT 256

#define BULLETIN_WAR_ALLY_IDS 15

extern DRAWMANAGER draw_manager;

enum BulletinMode : uint8_t {
    BM_Default,
    BM_Report,
};

enum BulletinPRAction : uint8_t {
    BPR_None,
    BPR_Back,
    BPR_Forward,
    BPR_Close,
    BPR_Zoom,
};

class BulletinDrawer {
    public:
        BulletinDrawer();
        bool bulletin_start_new(bool);
        bool bulletin_start_new(bool, Star*);
        bool bulletin_start_new(bool, int);
        bool bulletin_start_new(bool, BulletinMode);
        void bulletin_update(float);
        void bulletin_set_next_text(const char*, ...);
        void bulletin_set_text_col(RGB);
        void bulletin_write_planet_info(Star*, Planet*);
        void bulletin_reset_text_cols();
        void bulletin_ensure_closed();
        bool bulletin_is_open();
        bool bulletin_acknowledged();
        bool bulletin_was_yesno_yes();
        void bulletin_set_bg(const char*);
        void bulletin_set_flag(const char*);
        void bulletin_set_player_flag(Player*);
        void bulletin_set_active_player_flag();
        void bulletin_set_yesno();
        BulletinPRAction bulletin_get_praction();
        void bulletin_set_report(const PlanetReport* report);

        void bulletin_set_war_ally(Planet*, int);
        int bulletin_get_war_ally_result(int&, int&, int&);
    private:
        BulletinMode bulletin_mode;

        bool bulletin_start_new_internal(bool, int, BulletinMode);
        void bulletin_open();    // Open using bulletin_start_new()
        void bulletin_close();   // Close using bulletin_ensure_closed()
        void bulletin_reset();
        bool _bulletin_is_open;
        void bulletin_update_bg();
        void bulletin_set_prbuttons();

        char bulletin_text[BULLETIN_LINES][BULLETIN_MAX_TEXT];
        RGB bulletin_text_col[BULLETIN_LINES];
        const char* bulletin_bg;
        const char* bulletin_bg_preserve;

        SprID id_bulletin_header_flag;
        SprID id_bulletin_header_l;
        SprID id_bulletin_header_r;
        SprID id_bulletin_panel;
        SprID id_bulletin_black;
        SprID id_bulletin_bg;
        SprID id_bulletin_bg_preserve;
        SprID id_bulletin_bg_scan;
        SprID id_bulletin_text[BULLETIN_LINES];
        SprID id_bulletin_yesno;
        SprID id_bulletin_prbuttons;
        SprID id_bulletin_zoom;

        void bulletin_draw_events();
        void bulletin_draw_text();
        int bulletin_text_y(int);

        int bulletin_text_idx;
        bool bulletin_has_been_acknowledged;
        float bulletin_transition;

        bool bulletin_redraws_needed;

        bool bulletin_is_yesno;
        bool bulletin_yesno_was_yes;

        BulletinPRAction bulletin_praction;

        const PlanetReport *bulletin_report;

        bool bulletin_is_war_ally;
        Planet* bulletin_war_ally_planet;
        int bulletin_war_ally_mc;
        int bulletin_war_ally_inf;
        int bulletin_war_ally_gli;
        int bulletin_war_ally_art;

        void bulletin_war_ally_init();
        void bulletin_war_ally_update();

        SprID bulletin_war_ally_ids_army[BULLETIN_WAR_ALLY_IDS];
        SprID bulletin_war_ally_id_exit;
};

#endif
