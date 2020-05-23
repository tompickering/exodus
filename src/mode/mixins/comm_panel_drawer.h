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

enum CommAction {
    CA_None,
    CA_Proceed,
    CA_Abort,
    CA_Text0,
    CA_Text1,
    CA_Text2,
    CA_Text3,
    CA_Text4,
    CA_Text5,
};

class CommPanelDrawer {
    public:
        CommPanelDrawer();
        void comm_set_title(const char*);
        void comm_set_anim(const Anim&);
        void comm_set_img_caption(const char*);
        void comm_set_text(int, const char*);
        void comm_vset_text(int, const char*, ...);
        void comm_open(int);
        void comm_close();
        void comm_ensure_closed();
        bool comm_is_open();
        CommAction comm_check_action();
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
        SprID id_text[6];

        CommAction comm_action;

        int comm_text_slots;
        int comm_text_y(int);
};

#endif
