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
    CA_BionicProbe,
    CA_GoodsTransfer,
    CA_StartProduction,
    CA_PlanAttack,
    CA_Trade,
    CA_Attack,
    CA_CallOffAttack,
};

enum CommImg {
    CI_Human,
    CI_HumanThoughtful,
    CI_HumanConfident,
    CI_HumanPlanet,
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
    DIA_S_ThrustBroken,
    DIA_S_PlanFly,
    DIA_S_PlanSettle,
    DIA_S_PlanProbe,
    DIA_S_PlanSettle2,
    DIA_S_PlanClaim,
    DIA_S_NamePlanet,
    DIA_S_NamePlanetConfirm,
    DIA_S_LookAgainBadLaws,
    DIA_S_LookAgainNoEssentials,
    DIA_S_CommsBroken,
    DIA_S_PlanetComm,
    DIA_S_HailPlanet,
    DIA_S_Attack,
    DIA_S_AttackPayOffWantMore,
    DIA_S_AttackAllyCompensate,
    DIA_S_AttackAllyApology,
    DIA_S_AttackAllyProceed,
    DIA_S_Trade,
    DIA_S_TradeRefuseRebuke,
    DIA_S_Offer,
    DIA_S_ProposeAlliance,
    DIA_S_ProposeAllianceAggressively,
    DIA_S_OfferAllianceMoney,
    DIA_S_OfferAllianceResponse,
    DIA_S_Comment,
    DIA_S_CommentApology,
    DIA_S_CommentCompensationLower,
    DIA_S_CommentCompensationRefuse,
    DIA_S_CommentCompensationAccept,
    DIA_S_CommentCompliment,
    DIA_S_CommentInsult,
    DIA_S_CommentThreaten,
    DIA_S_PlanAttack,
    DIA_S_FirstPlanetAdvice,
    DIA_S_FirstPlanetAdviceUrgent,
    DIA_S_FleetNotInSystem,
    DIA_S_NoScouts,
    DIA_S_NoCoverBombers,
    DIA_S_NoBombers,
    DIA_S_ArtificialPlanetStarInvalid,
    DIA_S_FlyAlreadyFlying,
    DIA_S_FlyAlreadyThere,
    DIA_S_ZoomButNotVisited,
    DIA_S_Quit,
    // For these, CPU is talking to us - so CPU is comm_player and human is comm_other
    DIA_S_CPU_Attack,
    DIA_S_CPU_AttackAlly,
    DIA_S_CPU_AttackPayOff,
    DIA_S_CPU_AttackPayOffOffer,
    DIA_S_CPU_Trade,
    DIA_S_CPU_TradeRequestFee,
    DIA_S_CPU_TradeFeeOffer,
    DIA_S_CPU_TradeInsult,
    DIA_S_CPU_Offer,
    DIA_S_CPU_ProposeAlliance,
    DIA_S_CPU_ProposeAllianceAggressively,
    DIA_S_CPU_AllianceQuery,
    // Battle comms
    DIA_S_B_OpenCommsAttacker,
    DIA_S_B_OpenCommsDefender,
    DIA_S_B_OfferMoneyAttacker, // }
    DIA_S_B_MockAttacker,       // }> As in, we ARE the attacker, mocking the defender
    DIA_S_B_IntimidateAttacker, // }
    DIA_S_B_OfferMoneyDefender,       // }
    DIA_S_B_OfferMoneyDefenderAccept, // }
    DIA_S_B_OfferMoneyDefenderReject, // }
    DIA_S_B_RequestMoneyDefender,     // }> As in, we ARE the defender
    DIA_S_B_IntimidateDefender,       // }
    DIA_S_B_NeverMind,
    DIA_S_B_CPU_OpenCommsAttacker,
    DIA_S_B_CPU_CommsAttackerResponse,
    DIA_S_B_CPU_OpenCommsDefender,
    DIA_S_B_CPU_CommsDefenderReject,
    DIA_S_B_CPU_CommsDefenderRequestMore,
    DIA_S_B_CPU_CommsDefenderAccept,
    DIA_S_B_CPU_OpenCommsRebels,
    DIA_S_B_CPU_RebelsOfferCompromise,
    DIA_S_B_CPU_RebelsOfferBetterLife,
    DIA_S_B_CPU_RebelsOfferNothing,
    DIA_S_B_CPU_RebelsProposeNoMC,
    DIA_S_B_CPU_RebelsProposeMC,
    DIA_S_B_CPU_RebelsMCOffer,
};

// These are from the perspective of the *player*
// I.E. what is currently being said to *us*
enum CommRecv {
    DIA_R_None,
    DIA_R_Close,
    DIA_R_ProceedOrAbort,
    DIA_R_Proxy,
    DIA_R_SettleCannotAfford,
    DIA_R_SettleQueryProbe,
    DIA_R_SettleProbeResults,
    DIA_R_SettleIntelligentLife,
    DIA_R_SettlePlanetInfo,
    DIA_R_SettleNamePlanet,
    DIA_R_SettleNamePlanetProceed,
    DIA_R_AwaitingOrders,
    DIA_R_Greeting,
    DIA_R_AttackAlly,
    DIA_R_AttackPayOff,
    DIA_R_AttackPayOffMore,
    DIA_R_NoAttackResponse,
    DIA_R_TradeOK,
    DIA_R_TradeFee,
    DIA_R_TradeRefuse,
    DIA_R_OfferListen,
    DIA_R_OfferQuery,
    DIA_R_OfferAllianceMoneyResponse,
    DIA_R_CommentListen,
    DIA_R_CommentRequestCompensation,
    // For these, CPU is talking to us - so CPU is comm_player and human is comm_other
    DIA_R_CPU_AttackResponse,
    DIA_R_CPU_AttackResponseAllied,
    DIA_R_CPU_AttackAllyProceed,
    DIA_R_CPU_AttackPayOffListen,
    DIA_R_CPU_AttackPayOffReject,
    DIA_R_CPU_TradeResponse,
    DIA_R_CPU_TradeFeeListen,
    DIA_R_CPU_TradeInsultResponse,
    DIA_R_CPU_OfferElaborate,
    DIA_R_CPU_ProposeAlliance,
    DIA_R_CPU_ProposeAllianceMoney,
    DIA_R_CPU_ProposeAllianceAggressively,
    // Battle comms
    DIA_R_B_OpenCommsAttacker,
    DIA_R_B_OpenCommsDefender,
    DIA_R_B_OfferMoneyDefenderListen,
    DIA_R_B_CPU_CommsAttacker,
    DIA_R_B_CPU_CommsDefender,
    DIA_R_B_CPU_CommsDefenderAcceptResponse,
    DIA_R_B_CPU_RebelComms,
    DIA_R_B_CPU_RebelsListen,
    DIA_R_B_CPU_RebelsDemandMC,
    DIA_R_B_CPU_RebelsAcceptOffer,
};

typedef struct {
    int mc;
    int mc2;
    AllianceType alliance_type;
    int alliance_prob;
    bool may_proceed;
    bool battle_first_comms;
    int location;
    int months;
    int battle_strength_att;
    int battle_strength_def;
} CommContext;

class CommPanelDrawer {
    public:
        CommPanelDrawer();
        CommAction comm_update(float);
        void comm_open(CommSend);
        void comm_close();
        void comm_ensure_closed();
        bool comm_is_open();
        CommContext comm_ctx;
        CommAction comm_action_check();
    private:
        bool _comm_is_open;

        void comm_draw_text();
        void comm_set_title(const char*, ...);
        void comm_set_speech(const char*, ...);
        void comm_set_race(Race);
        void comm_set_img(CommImg);
        void comm_set_img_caption(const char*);
        void comm_set_img_caption_upper(const char*, ...);
        void comm_set_img_caption_lower(const char*, ...);
        void comm_set_text(int, const char*, ...);

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
        float comm_time_since_open;
        float comm_time_since_text_mouseover;
        unsigned char comm_text_interactive_mask;
        unsigned char comm_text_disabled_mask;
        int comm_mouseover_text;
        Anim comm_anim;

        void comm_draw_over_img_elements();

        int comm_title_len;
        bool comm_speech;

        bool comm_enable_throbber = false;
        bool comm_enable_distort = false;

        void comm_exit_anim(CommAction);
        bool comm_exit_anim_active;
        float comm_exit_anim_time;
        CommAction comm_exit_anim_action;
        void comm_exit_anim_update(float);

        void comm_complete_speech();

        SprID id_comm_panel;
        SprID id_comm_bg_t;
        SprID id_comm_bg_b;
        SprID id_comm_title;
        SprID id_comm_img;
        SprID id_comm_img_bg;
        SprID id_comm_throbber;
        SprID id_comm_ally_trade;
        SprID id_comm_ally_nonattack;
        SprID id_comm_ally_war;
        SprID id_comm_buttons;
        SprID id_comm_buttons_bg;
        SprID id_comm_adj;
        SprID id_comm_adj_ok;
        SprID id_text[6];
        SprID id_comm_input_frame;
        SprID id_comm_input_box;

        CommAction comm_action;

        int comm_text_slots;
        int comm_text_y(int);

        void comm_show_buttons(bool);
        void comm_show_adj(bool);

        void comm_init(CommSend); // Called once to set image etc
        void comm_prepare(int);   // Called for each new dialogue phase
        void comm_send(CommSend);
        void comm_recv(CommRecv);
        void comm_process_responses();

        CommSend comm_proxyto;
        void comm_recv_proxyto(CommSend);

        float comm_planet_name_confirm_time;
};

#endif
