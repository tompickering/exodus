#include "lunar_battle.h"

#include "assetpaths.h"

enum ID {
    END,
};

LunarBattle::LunarBattle() : ModeBase("LunarBattle"), CommPanelDrawer() {
    stage = LB_Idle;
}

void LunarBattle::enter() {
    ModeBase::enter(ID::END);
    stage = LB_Idle;
}

ExodusMode LunarBattle::update(float delta) {
    LunarBattleParams &b = ephstate.lunar_battle;
    LunarBattleReport &rpt = ephstate.lunar_battle_report;

    L.debug("<<< LUNAR BATTLE PLACEHOLDER >>>>");

    // TODO: Battle - in the meantime just fake result!
    rpt.aggressor_units_lost = 11;
    rpt.defender_units_lost = 7;
    rpt.aggressor_won = (bool)(rand() % 2);
    ephstate.set_ephemeral_state(EPH_LunarBattleReport);
    return ephstate.get_appropriate_mode();

    return ExodusMode::MODE_None;
}
