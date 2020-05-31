#ifndef GUARD_BULLETIN_DRAWER_H
#define GUARD_BULLETIN_DRAWER_H

#include "draw/draw.h"

#include "anim.h"
#include "state/exodus_state.h"

#define BULLETIN_LINES     20
#define BULLETIN_MAX_TEXT 256

extern DRAWMANAGER draw_manager;

extern ExodusState exostate;

class BulletinDrawer {
    public:
        BulletinDrawer();
        void bulletin_start_new(bool);
        void bulletin_update(float);
        void bulletin_set_next_text(const char*);
        void bulletin_vset_next_text(const char*, ...);
        void bulletin_set_text_col(RGB);
        void bulletin_reset_text_cols();
        void bulletin_ensure_closed();
        bool bulletin_is_open();
        bool bulletin_acknowledged();
        void bulletin_set_bg(const char*);
        void bulletin_set_flag(const char*);
        void bulletin_set_player_flag(Player*);
        void bulletin_set_active_player_flag();
    private:
        void bulletin_open();    // Open using bulletin_start_new()
        void bulletin_close();   // Close using bulletin_ensure_closed()
        bool _bulletin_is_open;
        void bulletin_update_bg();

        char bulletin_text[BULLETIN_LINES][BULLETIN_MAX_TEXT];
        RGB bulletin_text_col[BULLETIN_LINES];
        const char* bulletin_bg;

        SprID id_bulletin_header_flag;
        SprID id_bulletin_header_l;
        SprID id_bulletin_header_r;
        SprID id_bulletin_panel;
        SprID id_bulletin_bg;
        SprID id_bulletin_bg_scan;
        SprID id_bulletin_text[BULLETIN_LINES];

        void bulletin_draw_text();
        int bulletin_text_y(int);

        int bulletin_text_idx;
        bool bulletin_has_been_acknowledged;
        float bulletin_transition;

        bool bulletin_redraws_needed;
};

#endif
