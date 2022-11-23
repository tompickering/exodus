#ifndef GUARD_ALIEN_VESSEL_H
#define GUARD_ALIEN_VESSEL_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

#include "mixins/frame_drawer.h"
#include "mixins/panel_drawer.h"

#define MAX_COMM_LINES 10

extern ExodusState exostate;
extern EphemeralState ephstate;

class AlienVessel : ModeBase, PanelDrawer, FrameDrawer {
    public:
        AlienVessel();
        virtual void enter() override;
        virtual void exit() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            AV_Approach,
            AV_Idle,
            AV_Comm,
            AV_CommSpeech,
            AV_Attack,
            AV_Surrender,
            AV_Exit,
        };

        Stage stage;

        enum VesselType {
            VESSEL_NoID,
            VESSEL_GuildPatrol,
            VESSEL_Trading,
            VESSEL_TradingConvoy,
            VESSEL_Travelling,
            VESSEL_Science,
            VESSEL_Mining,
            VESSEL_Religious,
            VESSEL_Unknown,
        };

        VesselType type;

        const char *ship_img;
        const char *ship_name;
        bool will_respond;
        bool will_hail;
        bool will_attack;
        bool will_surrender;

        int enemy_ships;
        int enemy_cargo;
        int enemy_scouts;

        float approach;
        bool comm_done;
        float comm_timer;
        int comm_line;
        bool surrender_done;

        SprID comm_ids[MAX_COMM_LINES];
        const char *comm_text[MAX_COMM_LINES];

        void clear_overlay();
        void set_comm_text();
};

#endif
