#include "lunar_battle.h"

#include <cmath>

#include "assetpaths.h"

static const int SURF_X =  0;
static const int SURF_Y = 72;
static const int BLK_SZ = 40;
static const float MOVE_RATE = 1.5f;
static const int BG_WIDTH = 16;
static const int BG_HEIGHT = 11;

enum ID {
    END,
};

LunarBattle::LunarBattle() : ModeBase("LunarBattle"), CommPanelDrawer() {
    stage = LB_Idle;
    unit_moving = false;
    move_interp = 0;
}

void LunarBattle::enter() {
    ModeBase::enter(ID::END);
    stage = LB_Idle;

    LunarBattleParams &b = ephstate.lunar_battle;

    Planet *p = exostate.get_active_planet();

    n_units = 0;
    unit_moving = false;
    move_interp = 0;
    n_cover = 0;

    use_alt_aliens = (bool)(rand() % 2);

    if (!b.auto_battle) {
        draw_manager.draw(
            p->moon_sprites()->bg,
            {SURF_X, SURF_Y, 0, 0, 1, 1});

        place_cover();

        for (int i = 0; i < n_cover; ++i) {
            const char *spr = p->moon_sprites()->cover0;
            if (cover[i].alt) spr = p->moon_sprites()->cover1;
            draw_manager.draw(
                spr,
                {SURF_X + cover[i].x * BLK_SZ,
                 SURF_Y + cover[i].y * BLK_SZ,
                 0, 0, 1, 1});
        }

        draw_manager.save_background();

        for (int i = 0; i < BATTLE_UNITS_MAX; ++i) {
            unit_ids[i] = draw_manager.new_sprite_id();
        }

        place_units();
    }

    draw_manager.show_cursor(!b.auto_battle);
}

void LunarBattle::exit() {
    LunarBattleParams &b = ephstate.lunar_battle;

    if (!b.auto_battle) {
        for (int i = 0; i < BATTLE_UNITS_MAX; ++i) {
            draw_manager.release_sprite_id(unit_ids[i]);
        }
    }

    ModeBase::exit();
}

ExodusMode LunarBattle::update(float delta) {
    LunarBattleParams &b = ephstate.lunar_battle;
    LunarBattleReport &rpt = ephstate.lunar_battle_report;

    L.debug("<<< LUNAR BATTLE PLACEHOLDER >>>>");

    // TODO: Battle - in the meantime just fake result!
    if (b.auto_battle || (draw_manager.clicked() && unit_moving)) {
        rpt.aggressor_units_lost = 11;
        rpt.defender_units_lost = 7;
        rpt.aggressor_won = (bool)(rand() % 2);
        ephstate.set_ephemeral_state(EPH_LunarBattleReport);
        return ephstate.get_appropriate_mode();
    }

    if (unit_moving) {
        move_interp += delta * MOVE_RATE;
        if (move_interp > 1) move_interp = 1;
    }

    if (draw_manager.clicked()) {
        units[0].do_move(DIR_Left);
        unit_moving = true;
        move_interp = 0;
        // TODO: move SFX
    }

    draw_units();

    if (move_interp >= 1) {
        unit_moving = false;
        move_interp = 0;

        for (int i = 0; i < n_units; ++i) {
            units[i].x = units[i].tgt_x;
            units[i].y = units[i].tgt_y;
        }
    }

    return ExodusMode::MODE_None;
}

void LunarBattle::place_cover() {
    n_cover = RND(COVER_MAX); // 1 - 10 cover obstacles
    for (int i = 0; i < n_cover; ++i) {
        cover[i] = Cover(rand() % BG_WIDTH, rand() % BG_HEIGHT, (bool)(rand() % 1));
    }
}

void LunarBattle::place_units() {
    Planet *p = exostate.get_active_planet();

    if (p->has_lunar_base()) {
        place_unit(LBCtl(14, 5));
        place_unit(LBGun(13, 5));
        place_unit(LBGun(13, 4));
        place_unit(LBGun(13, 6));
        place_unit(LBGun(12, 5));
    }
}

void LunarBattle::place_unit(BattleUnit unit) {
    units[n_units++] = unit;
}

void LunarBattle::draw_units() {
    for (int i = 0; i < n_units; ++i) {
        int x = units[i].x;
        int y = units[i].y;
        int tx = units[i].tgt_x;
        int ty = units[i].tgt_y;

        const char *spr = units[i].idle;
        int draw_x = SURF_X + x * BLK_SZ;
        int draw_y = SURF_Y + y * BLK_SZ;

        // Unit is moving - interpolate draw position
        if (x != tx || y !=ty) {
            int tdraw_x = SURF_X + tx * BLK_SZ;
            int tdraw_y = SURF_Y + ty * BLK_SZ;
            draw_x += (int)((float)(tdraw_x - draw_x) * move_interp);
            draw_y += (int)((float)(tdraw_y - draw_y) * move_interp);
            if (fmod(move_interp, 0.4) < 0.2) spr = units[i].walk;
        }

        draw_manager.draw(
            unit_ids[i],
            spr,
            {draw_x, draw_y,
             0, 0, 1, 1});
    }
}

BattleUnit::BattleUnit(int _x, int _y, int _hp, bool _def)
    : x(_x), y(_y), hp(_hp), defending(_def) {
    tgt_x = x;
    tgt_y = y;
    can_act = true;
    idle = nullptr;
    walk = nullptr;
    fire = nullptr;
    dead = nullptr;
    move_sfx = nullptr;
    shoot_sfx = nullptr;
    can_shoot_behind = true;
}

// No validation here - LunarBattle is responsible for correct puppeteering
void BattleUnit::do_move(Direction d) {
    if (d == DIR_Up)    tgt_y--;
    if (d == DIR_Down)  tgt_y++;
    if (d == DIR_Left)  tgt_x--;
    if (d == DIR_Right) tgt_x++;
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
