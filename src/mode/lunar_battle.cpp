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
    L.debug("<<< LUNAR BATTLE PLACEHOLDER >>>>");
    ephstate.clear_ephemeral_state();
    return ExodusMode::MODE_Pop;
}
