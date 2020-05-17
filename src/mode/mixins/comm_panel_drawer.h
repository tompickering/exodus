#ifndef GUARD_COMM_PANEL_DRAWER_H
#define GUARD_COMM_PANEL_DRAWER_H

#include "draw/draw.h"

#include "anim.h"
#include "state/exodus_state.h"

#define MAX_TEXT 256

extern const int COMM_W;
extern const int COMM_H;
extern const int PNL_BORDER;

extern DRAWMANAGER draw_manager;

extern ExodusState exostate;

class CommPanelDrawer {
    public:
        CommPanelDrawer();
        void comm_set_title(const char*);
        void comm_set_anim(const Anim&);
        void comm_set_img_caption(const char*);
        void comm_set_text(int, const char*);
        void comm_open();
        void comm_close();
        void comm_ensure_closed();
        bool comm_is_open();
    private:
        bool _comm_is_open;

        char comm_title[MAX_TEXT];
        char comm_img_caption[MAX_TEXT];
        char comm_text0[MAX_TEXT];
        char comm_text1[MAX_TEXT];
        char comm_text2[MAX_TEXT];
        char comm_text3[MAX_TEXT];
        char comm_text4[MAX_TEXT];
        char comm_text5[MAX_TEXT];
        char *comm_text[6];
        Anim comm_anim;

        SprID id_comm_panel;
        SprID id_comm_img;
        SprID id_comm_buttons;
};

#endif
