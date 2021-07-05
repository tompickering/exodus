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

    frame_time_elapsed = 0;

    SpaceBattleParams &b = ephstate.space_battle;
    L.debug("STARTING BATTLE - ENEMY SHIPS: %d", b.enemy_ships + b.enemy_scouts + b.enemy_cargo);
}

BattleShip* SpaceBattle::place(BattleShipType type, bool enemy, int hp) {
    return place(type, enemy, hp, 0);
}

BattleShip* SpaceBattle::place(BattleShipType type, bool enemy, int hp, int shield) {
    if (next_ship > &ships[MAX_SHIPS - 1]) {
        L.error("Too many ships");
        return nullptr;
    }

    int placed = 0;
    for (int i = 0; i < MAX_SHIPS; ++i) {
        if (!ships[i].exists) {
            continue;
        }
        if (ships[i].type == type && ships[i].enemy == enemy) {
            placed++;
        }
    }

    int x = 640;
    int y = 512;
    BattleShipAction act = BSA_Idle;

    // FIXME: Prevent placement collisions?

    if (enemy) {
        switch (type) {
            case SHIP_Warship:
                x = 600 + RND(300);
                y = 600 + RND(300);
                act = BSA_Attack;
                break;
            case SHIP_Scout:
                x = 600 + RND(300);
                y = 600 + RND(300);
                act = BSA_Report;
                break;
            case SHIP_Transporter:
                x = 820;
                y = 900;
                break;
            default:
                L.warn("No placement logic for enemy ship type %d", (int)type);
                break;
        }
    } else {
        switch (type) {
            case SHIP_Starship:
                x = 640;
                y = 512;
                break;
            case SHIP_Warship:
                x = 540;
                y = 380;
                if (placed == 1 || placed == 3) {
                    x = 740 ;
                }
                if (placed == 2 || placed == 3) {
                    y = 630;
                }
                x += RND(20);
                y += RND(20);
                break;
            case SHIP_Bomber:
                x = 300 + RND(600);
                y = 300 + RND(600);
                if (placed == 0) {
                    x = 600;
                    y = 100;
                }
                if (placed == 1) {
                    x = 800;
                    y = 180;
                }
                if (placed == 2) {
                    x = 950;
                    y = 300;
                }
                break;
            case SHIP_Scout:
                x = 300 + RND(600);
                y = 300 + RND(600);
                if (placed == 0) {
                    x = 300;
                    y = 300;
                }
                if (placed == 1) {
                    x = 250;
                    y = 380;
                }
                if (placed == 2) {
                    x = 500;
                    y = 950;
                }
                if (placed == 3) {
                    x = 340;
                    y = 820;
                }
                if (placed == 4) {
                    x = 300;
                    y = 700;
                }
                break;
            case SHIP_Transporter:
                x = 300 + RND(600);
                y = 300 + RND(600);
                if (placed == 0) {
                    x = 200;
                    y = 570;
                }
                break;
            default:
                L.warn("No placement logic for ship type %d", (int)type);
                break;
        }
    }

    x = 1280 - x;
    y = 1024 - y;

    BattleShip *s = next_ship++;
    L.debug("Placing %s ship", enemy?"CPU":"human");
    s->init(type, enemy, x, y, hp, shield);
    s->action = act;
    return s;
}

void SpaceBattle::distribute(BattleShipType type, bool enemy, int n_ships, int stacklim, int max_stacks) {
    if (max_stacks > MAX_SHIPS_PER_TYPE) {
        L.warn("More stacks than MAX_SHIPS_PER_TYPE: %d", max_stacks);
    }

    if (n_ships > 0) {
        if (n_ships > stacklim) {
            int div = n_ships / max_stacks;
            int add = n_ships % max_stacks;
            for (int i = 0; i < max_stacks; ++i) {
                place(type, enemy, div+(i==0?add:0));
            }
        } else {
            place(type, enemy, n_ships);
        }
    }
}

void SpaceBattle::prepare() {
    Player *p = exostate.get_active_player();
    SpaceBattleParams &b = ephstate.space_battle;

    selected = nullptr;

    for (int i = 0; i < MAX_SHIPS; ++i) {
        ships[i].exists = false;
    }

    next_ship = &ships[0];

    int shields = p->get_starship().shield_generators;
    place(SHIP_Starship, false, 1, shields*5);

    distribute(SHIP_Warship, true, b.enemy_ships, 10, 4);
    distribute(SHIP_Scout, true, b.enemy_scouts, 5, 2);
    if (b.enemy_cargo) {
        place(SHIP_Transporter, true, b.enemy_cargo);
    }

    const Fleet &f = p->get_fleet();

    distribute(SHIP_Warship, false, f.warships, 10, 4);
    distribute(SHIP_Bomber, false, f.bombers, 10, 3);
    distribute(SHIP_Scout, false, f.scouts, 10, 5);
    if (f.transporters) {
        place(SHIP_Transporter, true, f.transporters);
    }
}

void SpaceBattle::draw() {
    for (int i = 0; i < MAX_SHIPS; ++i) {
        if (!ships[i].exists) {
            continue;
        }

        if (ships[i].hp <= 0) {
            continue;
        }

        const BattleShip& s = ships[i];
        int draw_x = s.x / 2;
        int draw_y = s.y / 2;

        bool sel = false;
        if (ships[i].enemy) {
            sel = (selected && selected->target == &ships[i]);
        } else {
            sel = (selected == &ships[i]);
        }

        draw_manager.draw(
            s.spr_id,
            sel ? s.spr_sel : s.spr,
            {draw_x, draw_y,
             .5f, .5f, 1, 1});

        draw_manager.draw(
            s.spr_id_label,
            s.spr_label,
            {draw_x + 24, draw_y - 10,
             .5f, .5f, 1, 1});
    }
}

void SpaceBattle::update_mouse() {
    // TODO: PROCr_cycleaction
    for (int i = 0; i < MAX_SHIPS; ++i) {
        if (!ships[i].exists) {
            continue;
        }

        if (draw_manager.query_click(ships[i].spr_id).id) {
            if (!ships[i].enemy) {
                selected = &ships[i];
                L.debug("Selected ship %d", i);
            }
        }

        if (draw_manager.query_click_r(ships[i].spr_id).id) {
            if (ships[i].enemy) {
                if (selected && selected->exists && selected->hp > 0) {
                    selected->target = &ships[i];
                    L.debug("Targeting ship %d", i);
                }
            }
        }
    }
}

void SpaceBattle::update_battle() {
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
                update_battle();
                frame_time_elapsed = 0;
                stage = SB_Wait;
            }
            // Fall through here - handle UI etc in SB_Wait
        case SB_Wait:
            {
                update_mouse();

                frame_time_elapsed += delta;
                if (frame_time_elapsed >= 0.4) {
                    stage = SB_Battle;
                }

                if (draw_manager.clicked()) {
                    //stage = SB_Exit;
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

void BattleShip::init(BattleShipType _type, bool _enemy, int _x, int _y, int _hp, int _shield) {
    exists = true;
    type = _type;
    enemy = _enemy;
    x = _x;
    y = _y;
    hp = _hp;
    shield_max = _shield;
    shield = shield_max;
    target = nullptr;

    spr_id = draw_manager.new_sprite_id();
    spr_id_label = draw_manager.new_sprite_id();

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

    spr_label = nullptr;
    switch (type) {
        case SHIP_Starship:
            spr_label = IMG_RD1_TYP1;
            break;
        case SHIP_Warship:
            spr_label = IMG_RD1_TYP2;
            break;
        case SHIP_Bomber:
            spr_label = IMG_RD1_TYP3;
            break;
        case SHIP_Scout:
            spr_label = IMG_RD1_TYP4;
            break;
        case SHIP_Transporter:
            spr_label = IMG_RD1_TYP5;
            break;
    }
}

void BattleShip::cleanup() {
    if (exists) {
        draw_manager.release_sprite_id(spr_id);
        draw_manager.release_sprite_id(spr_id_label);
    }
    exists = false;
}
