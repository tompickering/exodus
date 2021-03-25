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
};

enum CommImg {
    CI_Human,
    CI_Yokon,
    CI_Teri,
    CI_Urkash,
    CI_Gordoon,
    CI_Rebels,
};

// These are the player's *input* into the conversation
// These are always used to initiate the comm panel,
// even when a CPU player appears to be initiating a
// conversation with us.
enum CommSend {
    DIA_S_PlanFly,
    DIA_S_PlanSettle,
    DIA_S_NamePlanet,
    DIA_S_HailPlanet,
    DIA_S_Attack,
    DIA_S_Offer,
    DIA_S_Comment,
};

// These are from the perspective of the *player*
// I.E. what is currently being said to *us*
enum CommRecv {
    DIA_R_None,
    DIA_R_ProceedOrAbort,
    DIA_R_SettleCannotAfford,
    DIA_R_SettlePlanetInfo,
    DIA_R_SettleNamePlanet,
    DIA_R_Greeting,
};

class CommPanelDrawer {
    public:
        CommPanelDrawer();
        void comm_update(float);
        void comm_draw_text();
        void comm_set_title(const char*, ...);
        void comm_set_speech(const char*, ...);
        void comm_set_race(Race);
        void comm_set_img(CommImg);
        void comm_set_img_caption(const char*);
        void comm_set_img_caption_upper(const char*, ...);
        void comm_set_img_caption_lower(const char*, ...);
        void comm_set_text(int, const char*, ...);
        void comm_open(CommSend);
        void comm_close();
        void comm_ensure_closed();
        bool comm_is_open();
        CommAction comm_check_action();
        void comm_set_text_interactive_mask(unsigned char);
    private:
        bool _comm_is_open;

        Player *comm_player;
        Player *comm_other;
        Planet *comm_planet;
        CommRecv comm_state;

        CommAction comm_report_action;

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
        float comm_time;
        float comm_time_since_text_mouseover;
        unsigned char comm_text_interactive_mask;
        int comm_mouseover_text;
        Anim comm_anim;

        int comm_title_len;
        bool comm_speech;

        SprID id_comm_panel;
        SprID id_comm_bg_t;
        SprID id_comm_bg_b;
        SprID id_comm_title;
        SprID id_comm_img;
        SprID id_comm_buttons;
        SprID id_comm_buttons_bg;
        SprID id_text[6];

        CommAction comm_action;

        int comm_text_slots;
        int comm_text_y(int);

        void comm_show_buttons(bool);

        void comm_init(CommSend); // Called once to set image etc
        void comm_prepare(int);   // Called for each new dialogue phase
        void comm_send(CommSend);
        void comm_recv(CommRecv);
        void comm_process_responses();
};

#endif
