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

    b.aggressor_inf = 0;
    b.aggressor_gli = 0;
    b.aggressor_art = 0;
    b.defender_inf = 0;
    b.defender_gli = 0;
    b.defender_art = 0;

    // Get numbers of defending units
    p->get_army(b.defender_inf, b.defender_gli, b.defender_art);

    int m = exostate.get_orig_month();

    // Orig: PROCb_gpr
    int def_mc = owner->get_mc();
    if      (def_mc <  30) mines_price = 5;
    else if (def_mc <  60) mines_price = 8;
    else if (def_mc < 100) mines_price = 10;
    else if (def_mc < 200) mines_price = 15;
    else if (def_mc < 501) mines_price = 20;
    else                   mines_price = 25;

    if      (m <  20) mines_available = 5;
    else if (m <  50) mines_available = 8;
    else if (m < 100) mines_available = 10;
    else              mines_available = 12;

    b.defender_mines = 0;

    // TODO - Supplements from war allies - ensure we don't subtrace these from
    // garrison at the end of combat!

    switch (b.aggressor_type) {
        case AGG_Player:
            b.aggressor_inf = aggressor->get_fleet().freight.infantry;
            b.aggressor_gli = aggressor->get_fleet().freight.gliders;
            b.aggressor_art = aggressor->get_fleet().freight.artillery;
            break;
        case AGG_Rebels:
            // TODO
            break;
        case AGG_Aliens:
            b.aggressor_inf = RND(m/2);
            b.aggressor_art = RND(m);
            break;
    }

    int agg_total = b.aggressor_inf + b.aggressor_gli + b.aggressor_art;
    int def_total = b.defender_inf  + b.defender_gli  + b.defender_art;

    // TODO: I think this only happens for humans
    if (def_total == 0) {
        b.defender_inf = 1;
        def_total = 1;
    }

    // Default group sizes - humans can override.
    b.aggressor_group_size = 12;
    if (agg_total <= 200) b.aggressor_group_size = 8;
    if (agg_total <=  90) b.aggressor_group_size = 5;
    if (agg_total <=  30) b.aggressor_group_size = 3;

    b.defender_group_size = 12;
    if (def_total <= 200) b.defender_group_size = 8;
    if (def_total <=  90) b.defender_group_size = 5;
    if (def_total <=  30) b.defender_group_size = 3;

    if (b.human_battle) {
        draw_manager.draw(IMG_BATTLE_PREP);
        draw_manager.save_background();
    }

    draw_manager.show_cursor(b.human_battle);
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
