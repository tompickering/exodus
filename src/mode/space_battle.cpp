#include "space_battle.h"

#include "assetpaths.h"

enum ID {
    BACKGROUND,
    END,
};

SpaceBattle::SpaceBattle() : ModeBase("SpaceBattle") {
}

void SpaceBattle::enter() {
    ModeBase::enter(ID::END);

    draw_manager.draw(
        id(ID::BACKGROUND),
        IMG_RADAR,
        {0, 0, 0, 0, 1, 1});

    stage = SB_Setup;

    draw_manager.show_cursor(true);

    SpaceBattleParams &b = ephstate.space_battle;
    L.debug("STARTING BATTLE - ENEMY SHIPS: %d", b.enemy_ships);
}

void SpaceBattle::prepare() {
    Player *p = exostate.get_active_player();
    SpaceBattleParams &b = ephstate.space_battle;

    selected = -1;

    for (int i = 0; i < MAX_SHIPS; ++i) {
        ships[i].exists = false;
    }

    int shields = p->get_starship().shield_generators;
    ships[0].init(SHIP_Starship, false, 640, 512, 1, shields*5);
}

void SpaceBattle::draw() {
    for (int i = 0; i < MAX_SHIPS; ++i) {
        if (!ships[i].exists) {
            continue;
        }

        const BattleShip& s = ships[i];

        draw_manager.draw(
            s.spr_id,
            selected == i ? s.spr_sel : s.spr,
            {s.x/2, s.y/2,
             .5f, .5f, 1, 1});
    }
}

ExodusMode SpaceBattle::update(float delta) {

    switch (stage) {
        case SB_Setup:
            {
                prepare();
                stage = SB_Battle;
            }
            break;
        case SB_Battle:
            {
                if (draw_manager.clicked()) {
                    stage = SB_Exit;
                }
            }
            break;
        case SB_Exit:
            {
                for (int i = 0; i < MAX_SHIPS; ++i) {
                    ships[i].cleanup();
                }

                ephstate.set_ephemeral_state(EPH_ResumeFly);
                return ExodusMode::MODE_Pop;
            }
            break;
    }

    draw();

    return ExodusMode::MODE_None;
}

void BattleShip::init(BattleShipType _type, bool _enemy, int _x, int _y, int _hp) {
    init(_type, _enemy, _x, _y, _hp, 0);
}

void BattleShip::init(BattleShipType _type, bool _enemy, int _x, int _y, int _hp, int _shield) {
    exists = true;
    type = _type;
    enemy = _enemy;
    x = _x;
    y = _y;
    hp = _hp;
    shield_max = _shield;
    shield = shield_max;
    target = -1;

    spr_id = draw_manager.new_sprite_id();

    spr = IMG_RD1_SYMBOL3;
    spr_sel = IMG_RD1_SYMBOL3S;

    if (!enemy) {
        if (type == SHIP_Starship) {
            spr = IMG_RD1_SYMBOL2;
            spr_sel = IMG_RD1_SYMBOL2S;
        } else {
            spr = IMG_RD1_SYMBOL1;
            spr_sel = IMG_RD1_SYMBOL1S;
        }
    }
}

void BattleShip::cleanup() {
    if (exists) {
        draw_manager.release_sprite_id(spr_id);
    }
    exists = false;
}
