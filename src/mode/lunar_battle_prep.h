#ifndef GUARD_LUNAR_BATTLE_PREP_H
#define GUARD_LUNAR_BATTLE_PREP_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

#include "mixins/battle_report.h"

extern ExodusState exostate;

class LunarBattlePrep : ModeBase, BattleReport {
    public:
        LunarBattlePrep();
        virtual void enter() override;
        virtual void exit() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            LBP_Auto,
            LBP_InitialPause,
            LBP_InvaderReport,
            LBP_AllySupport,
            LBP_GuildSupport,
            LBP_BuyMines,
            LBP_CommandOrWait,
            LBP_OptionGroupSize,
            LBP_GroupSize,
            LBP_OptionPlacement,
            LBP_AutoBattleWait,
            LBP_AutoBattleWait1,
            LBP_AutoBattleWait2,
            LBP_StartBattle,
            LBP_Conclude,
            LBP_AutoBattleConclude,
        };

        Stage stage;

        int mines_available;
        int mines_price;
        int mines_to_buy;

        bool stage_started;
        float pause;

        void set_stage(Stage);

        int get_agg_total();
        int get_def_total();

        void draw_panel();
};

#endif
