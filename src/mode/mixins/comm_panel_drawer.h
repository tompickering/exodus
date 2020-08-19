#ifndef GUARD_COMM_PANEL_DRAWER_H
#define GUARD_COMM_PANEL_DRAWER_H

#include "draw/draw.h"

#include "anim.h"
#include "state/exodus_state.h"

#define COMM_MAX_TEXT 256

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

enum CommImg {
    CI_Human,
    CI_Yokon,
    CI_Teri,
    CI_Urkash,
    CI_Gordoon,
    CI_Rebels,
};

class CommPanelDrawer {
    public:
        CommPanelDrawer();
        void comm_update(float);
        void comm_draw_text();
        void comm_set_title(const char*);
        void comm_set_title(const char*, bool);
        void comm_set_race(Race);
        void comm_set_img(CommImg);
        void comm_set_img_caption(const char*);
        void comm_set_img_caption(const char*, const char*);
        void comm_set_text(int, const char*, ...);
        void comm_set_buttons(bool);
        void comm_open(int);
        void comm_close();
        void comm_ensure_closed();
        bool comm_is_open();
        CommAction comm_check_action();
        void comm_set_text_interactive_mask(unsigned char);
    private:
        bool _comm_is_open;

        char comm_title[COMM_MAX_TEXT];
        char comm_img_caption_upper[COMM_MAX_TEXT];
        char comm_img_caption_lower[COMM_MAX_TEXT];
        char comm_text0[COMM_MAX_TEXT];
        char comm_text1[COMM_MAX_TEXT];
        char comm_text2[COMM_MAX_TEXT];
        char comm_text3[COMM_MAX_TEXT];
        char comm_text4[COMM_MAX_TEXT];
        char comm_text5[COMM_MAX_TEXT];
        char *comm_text[6];
        float comm_time_open;
        float comm_time_since_text_mouseover;
        unsigned char comm_text_interactive_mask;
        int comm_mouseover_text;
        Anim comm_anim;

        int comm_title_len;
        bool comm_title_gradual;

        SprID id_comm_panel;
        SprID id_comm_title;
        SprID id_comm_img;
        SprID id_comm_buttons;
        SprID id_text[6];

        CommAction comm_action;

        int comm_text_slots;
        int comm_text_y(int);

        bool show_buttons;
};

#endif
