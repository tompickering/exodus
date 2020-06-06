#include "lunar_battle_prep.h"

#include "assetpaths.h"

enum ID {
    END,
};

LunarBattlePrep::LunarBattlePrep() : ModeBase("LunarBattlePrep") {
    stackable = false;
    stage = LBP_Idle;
}

void LunarBattlePrep::enter() {
    ModeBase::enter(ID::END);
    stage = LBP_Idle;

    // TODO: Only draw for human battle

    draw_manager.draw(IMG_BATTLE_PREP);
    draw_manager.save_background();
}

ExodusMode LunarBattlePrep::update(float delta) {
    L.debug("<<< LUNAR BATTLE PREP PLACEHOLDER >>>>");

    if (draw_manager.clicked()) {
        ephstate.set_ephemeral_state(EPH_LunarBattle);
        return ephstate.get_appropriate_mode();
    }

    return ExodusMode::MODE_None;
}
