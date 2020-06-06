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

    LunarBattleParams &b = ephstate.lunar_battle;

    Planet *p = exostate.get_active_planet();

    if (!p) {
        L.fatal("Entered lunar battle prep with no active planet");
    }

    if (!p->is_owned()) {
        L.fatal("Entered lunar battle prep with an unowned planet");
    }

    Player *owner = exostate.get_player(p->get_owner());
    Player *aggressor = nullptr;

    b.human_attacking = false;
    b.aggressor_manual_placement = false;
    b.defender_manual_placement = false;

    if (b.aggressor_type == AGG_Player) {
        aggressor = exostate.get_player(b.aggressor_idx);
        if (aggressor == owner) {
            L.fatal("%d is attacking own planet!", owner->get_full_name());
        }
        b.human_attacking = aggressor->is_human();
    }

    // TODO: How does all this work in multiplayer PvP?

    b.human_defending = owner->is_human();
    b.human_battle = b.human_attacking || b.human_defending;

    // Automate CPU vs CPU battles
    if (!b.human_battle) {
        b.auto_battle = true;
    }

    if (b.human_battle) {
        draw_manager.draw(IMG_BATTLE_PREP);
        draw_manager.save_background();
    }
}

ExodusMode LunarBattlePrep::update(float delta) {
    LunarBattleParams &b = ephstate.lunar_battle;

    L.debug("<<< LUNAR BATTLE PREP PLACEHOLDER >>>>");

    if (!b.human_battle) {
        // We've done all prep in enter()
        ephstate.set_ephemeral_state(EPH_LunarBattle);
        return ephstate.get_appropriate_mode();
    }

    if (draw_manager.clicked()) {
        // TODO: Set this based on 'wait for the report' selection
        b.auto_battle = false;
        // TODO: Set this based on preferences
        b.aggressor_manual_placement = false;
        b.defender_manual_placement = false;
        ephstate.set_ephemeral_state(EPH_LunarBattle);
        return ephstate.get_appropriate_mode();
    }

    return ExodusMode::MODE_None;
}
