#include "lunar_battle.h"

#include "assetpaths.h"

static const int SURF_X =  0;
static const int SURF_Y = 72;
static const int BLK_SZ = 40;

enum ID {
    END,
};

LunarBattle::LunarBattle() : ModeBase("LunarBattle"), CommPanelDrawer() {
    stage = LB_Idle;
}

void LunarBattle::enter() {
    ModeBase::enter(ID::END);
    stage = LB_Idle;

    LunarBattleParams &b = ephstate.lunar_battle;

    Planet *p = exostate.get_active_planet();

    n_units = 0;

    use_alt_aliens = (bool)(rand() % 2);

    if (!b.auto_battle) {
        draw_manager.draw(
            p->moon_sprites()->bg,
            {SURF_X, SURF_Y, 0, 0, 1, 1});
        draw_manager.save_background();
    }

    draw_manager.show_cursor(!b.auto_battle);
}

ExodusMode LunarBattle::update(float delta) {
    LunarBattleParams &b = ephstate.lunar_battle;
    LunarBattleReport &rpt = ephstate.lunar_battle_report;

    L.debug("<<< LUNAR BATTLE PLACEHOLDER >>>>");

    // TODO: Battle - in the meantime just fake result!
    if (b.auto_battle || draw_manager.clicked()) {
        rpt.aggressor_units_lost = 11;
        rpt.defender_units_lost = 7;
        rpt.aggressor_won = (bool)(rand() % 2);
        ephstate.set_ephemeral_state(EPH_LunarBattleReport);
        return ephstate.get_appropriate_mode();
    }

    return ExodusMode::MODE_None;
}

BattleUnit::BattleUnit(int _x, int _y, int _hp, bool _def)
    : x(_x), y(_y), hp(_hp), defending(_def) {
    can_act = true;
    idle = nullptr;
    walk = nullptr;
    fire = nullptr;
    dead = nullptr;
    move_sfx = nullptr;
    shoot_sfx = nullptr;
    can_shoot_behind = true;
}

Inf::Inf(int _x, int _y, int _hp, bool _def) : BattleUnit(_x, _y, _hp, _def) {
    move = 3;
    fire_range = 3;
    // TODO: SFX
    if (_def) {
        idle = IMG_GF4_4;
        walk = IMG_GF4_4_2;
        fire = IMG_GF4_16;
        dead = IMG_GF4_7;
    } else {
        idle = IMG_GF4_1;
        walk = IMG_GF4_1_2;
        fire = IMG_GF4_13;
        dead = IMG_GF4_10;
    }
}

Gli::Gli(int _x, int _y, int _hp, bool _def) : BattleUnit(_x, _y, _hp, _def) {
    move = 4;
    fire_range = 4;
    // TODO: SFX
    // TODO: Glider crashing sprite
    if (_def) {
        // Blue
        idle = IMG_GF4_5;
        walk = IMG_GF4_5;
        fire = IMG_GF4_17;
        dead = IMG_GF4_11;
    } else {
        // Green
        idle = IMG_GF4_2;
        walk = IMG_GF4_2;
        fire = IMG_GF4_14;
        dead = IMG_GF4_8;
    }
}

Art::Art(int _x, int _y, int _hp, bool _def) : BattleUnit(_x, _y, _hp, _def) {
    move = 0;
    fire_range = 7;
    can_shoot_behind = false;
    if (_def) {
        idle = IMG_GF4_6;
        walk = IMG_GF4_6;
        fire = IMG_GF4_18;
        dead = IMG_GF4_12;
    } else {
        idle = IMG_GF4_3;
        walk = IMG_GF4_3;
        fire = IMG_GF4_15;
        dead = IMG_GF4_9;
    }
}

LBGun::LBGun(int _x, int _y) : BattleUnit(_x, _y, 6, true) {
    move = 0;
    fire_range = 100;
    idle = IMG_GF4_21;
    walk = IMG_GF4_21;
    fire = IMG_GF4_24;
    dead = IMG_GF4_23;
}

LBCtl::LBCtl(int _x, int _y) : BattleUnit(_x, _y, 10, true) {
    move = 0;
    fire_range = 0;
    can_act = false;
    idle = IMG_GF4_20;
    walk = IMG_GF4_20;
    fire = IMG_GF4_20;
    dead = IMG_GF4_22;
}

Rebel::Rebel(int _x, int _y, int _hp) : BattleUnit(_x, _y, _hp, false) {
    // TODO: Check these
    move = 3;
    fire_range = 3;
    idle = IMG_RF1_1;
    walk = IMG_RF1_1_2;
    fire = IMG_RF1_13;
    dead = IMG_RF1_7;
}

AInf::AInf(int _x, int _y, int _hp, bool alt) : BattleUnit(_x, _y, _hp, false) {
    move = 3;
    fire_range = 3;
    if (!alt) {
        idle = IMG_AL1_1;
        walk = IMG_AL1_1_2;
        fire = IMG_AL1_13;
        dead = IMG_AL1_7;
    } else {
        idle = IMG_AL2_1;
        walk = IMG_AL2_1_2;
        fire = IMG_AL2_13;
        dead = IMG_AL2_7;
    }
}

AArt::AArt(int _x, int _y, int _hp, bool alt) : BattleUnit(_x, _y, _hp, false) {
    move = 0;
    fire_range = 7;
    can_shoot_behind = false;
    if (!alt) {
        idle = IMG_AL1_3;
        walk = IMG_AL1_3;
        fire = IMG_AL1_15;
        dead = IMG_AL1_9;
    } else {
        idle = IMG_AL2_3;
        walk = IMG_AL2_3;
        fire = IMG_AL2_15;
        dead = IMG_AL2_9;
    }
}
