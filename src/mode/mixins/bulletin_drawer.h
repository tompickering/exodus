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

#define BULLETIN_REPORT_SUMMARY_LINES 6

extern DRAWMANAGER draw_manager;

enum BulletinMode : uint8_t {
    BM_Default,
    BM_Report,
    BM_ReportSummary,
    BM_Manual,
};

enum BulletinManualPage : uint8_t {
    BMP_Contents,
    BMP_Contents_END,
    BMP_FirstSteps,
    BMP_FirstSteps_2,
    BMP_FirstSteps_3,
    BMP_FirstSteps_4,
    BMP_FirstSteps_5,
    BMP_FirstSteps_6,
    BMP_FirstSteps_7,
    BMP_FirstSteps_END,
    BMP_GalaxyMap,
    BMP_GalaxyMap_2,
    BMP_GalaxyMap_3,
    BMP_GalaxyMap_END,
    BMP_Ctrl,
    BMP_Ctrl_2,
    BMP_Ctrl_3,
    BMP_Ctrl_END,
    BMP_StarMap,
    BMP_StarMap_2,
    BMP_StarMap_END,
    BMP_Comm,
    BMP_Comm_2,
    BMP_Comm_3,
    BMP_Comm_4,
    BMP_Comm_END,
    BMP_PlanetMap,
    BMP_PlanetMap_2,
    BMP_PlanetMap_3,
    BMP_PlanetMap_END,
    BMP_LunarBattle,
    BMP_LunarBattle_2,
    BMP_LunarBattle_3,
    BMP_LunarBattle_4,
    BMP_LunarBattle_END,
    BMP_Trade,
    BMP_Trade_END,
    BMP_Alliances,
    BMP_Alliances_END,
    BMP_Fleet,
    BMP_Fleet_END,
    BMP_Science,
    BMP_Science_END,
    BMP_SpaceCombat,
    BMP_SpaceCombat_2,
    BMP_SpaceCombat_END,
    BMP_EndingTheGame,
    BMP_EndingTheGame_END,
    BMP_Tips,
    BMP_Tips_END,
    BMP_FAQ,
    BMP_FAQ_HowEarnMC,
    BMP_FAQ_WhyLessMC,
    BMP_FAQ_WhyLessFood,
    BMP_FAQ_END,
    BMP_MAX,
};

enum BulletinPRAction : uint8_t {
    BPR_None,
    BPR_Back,
    BPR_Forward,
    BPR_Close,
    BPR_Zoom,
    BPR_OpenSpecificReport,
};

class BulletinDrawer {
    public:
        BulletinDrawer();
        bool bulletin_start_new(bool);
        bool bulletin_start_new(bool, Star*);
        bool bulletin_start_new(bool, int);
        bool bulletin_start_new(bool, BulletinMode);
        bool bulletin_start_manual(BulletinManualPage);
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
        void bulletin_set_report_summary_page(int);

        void bulletin_set_war_ally(Planet*, int);
        int bulletin_get_war_ally_result(int&, int&, int&);

        int bulletin_get_specific_report() { return bulletin_report_specific; }
    private:
        BulletinMode bulletin_mode;

        bool bulletin_continue_manual(BulletinManualPage);

        bool bulletin_start_new_internal(bool, int, BulletinMode);
        void bulletin_open();    // Open using bulletin_start_new()
        void bulletin_close();   // Close using bulletin_ensure_closed()
        void bulletin_reset();
        bool _bulletin_is_open;
        void bulletin_update_bg();
        void bulletin_set_prbuttons();

        void bulletin_update_manual_page(bool);

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
        SprID id_bulletin_contents;

        void bulletin_draw_events();
        void bulletin_draw_text();
        int bulletin_text_y(int);
        int bulletin_text_y(int, int);
        void bulletin_draw_report_summary();

        int bulletin_text_idx;
        bool bulletin_has_been_acknowledged;
        float bulletin_transition;

        bool bulletin_redraws_needed;

        bool bulletin_is_yesno;
        bool bulletin_yesno_was_yes;

        BulletinPRAction bulletin_praction;

        const PlanetReport *bulletin_report;

        int bulletin_report_summary_page;
        int bulletin_report_specific;

        BulletinManualPage bulletin_manual_page_opened;
        BulletinManualPage bulletin_manual_page_current;

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

        BulletinManualPage bulletin_get_end_page();
        const char* bulletin_get_manual_flag();
};

#endif
