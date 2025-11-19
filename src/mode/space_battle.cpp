#include "space_battle.h"

#include <math.h>

#include "anim.h"
#include "util/value.h"

#include "assetpaths.h"
#include "exodus_features.h"

#define FAIL_SPRITES 3

static const DrawArea AREA_INFO   = {518, 400, 120, 28};
static const DrawArea AREA_DETAIL = {518, 428, 120, 28};
static const DrawArea AREA_AUTO   = {518, 456, 120, 28};
static const DrawArea AREA_QUIT   = {518, 484, 120, 28};

static const int PANEL_W = 326;
static const int PANEL_H = 190;
static const int PANEL_X = RES_X/2 - PANEL_W/2;
static const int PANEL_Y = RES_Y/2 - PANEL_H/2;

Anim fail_anim(FAIL_SPRITES,
               IMG_RD1_FAIL1,
               IMG_RD1_FAIL2,
               IMG_RD1_FAIL3);

Anim exp_anim(11, IMG_RD1_DEAD1, IMG_RD1_DEAD2, IMG_RD1_DEAD3, IMG_RD1_DEAD4,
                  IMG_RD1_DEAD5, IMG_RD1_DEAD6, IMG_RD1_DEAD7, IMG_RD1_DEAD8,
                  IMG_RD1_DEAD9, IMG_RD1_DEAD10, nullptr);

enum ID {
    BACKGROUND,
    FAIL_SHIP_STATS,
    FAIL_DIAGNOSTICS,
    BUTTON_DETAIL,
    BUTTON_AUTO,
    COUNT_WARSHIPS,
    COUNT_BOMBERS,
    COUNT_SCOUTS,
    COUNT_TRANSPORTS,
    COUNT_ENEMY,
    HEALTH_INDICATOR,
    BOW,
    PANEL,
    SURRENDER_OPT,
    BTN_ENG,
    BTN_DATA,
    BTN_INFO,
    BTN_QUIT,
    BTN_GUIDE,
    END,
};

SpaceBattle::SpaceBattle() : ModeBase("SpaceBattle") {
}

void SpaceBattle::enter() {
    ModeBase::enter(ID::END);

    //audio_manager.fade_out(1000);
    audio_manager.target_music(MUS_ST3_58);

    draw_manager.draw(
        id(ID::BACKGROUND),
        FEATURE(EF_SPACE_BATTLE_LAYOUT) ? IMG_SPACE_BG : IMG_RADAR,
        {0, 0, 0, 0, 1, 1});

    stage = SB_Setup;

    draw_manager.show_cursor(true);

    starship = nullptr;

    auto_battle = false;
    frame_time_elapsed = 0;
    panel_showing = false;
    resolution_delay = 0;
    resolution = SBRES_None;

    initial_warships = 0;

    fail_ship_stats = -1;
    fail_diagnostics = -1;
    fail_battle_readout = false;
    fail_battle_readout_this_frame = false;

    for (int i = 0; i < N_EXPLOSIONS; ++i) {
        explosions[i].spr_id = draw_manager.new_sprite_id();
    }

    do_explosions = false;
    enemy_ships_destroyed = 0;

    SpaceBattleParams &b = ephstate.space_battle;
    L.debug("STARTING BATTLE - ENEMY SHIPS: %d", b.enemy_ships + b.enemy_scouts + b.enemy_cargo);

    if (FEATURE(EF_SPACE_BATTLE_LAYOUT)) {
        draw_manager.draw(
            id(ID::BTN_INFO),
            IMG_BTN_INFO,
            {518, 456, 0, 0, 1, 1});

        draw_manager.draw(
            id(ID::BTN_QUIT),
            IMG_BTN_QUIT,
            {518, 484, 0, 0, 1, 1});
    }

    if (FEATURE(EF_MANUAL)) {
        draw_manager.draw(
            id(ID::BTN_GUIDE),
            IMG_BTNGUIDE,
            {RES_X-4, 150, 1, 0, 1, 1});
    }
}

void SpaceBattle::exit() {
    for (int i = 0; i < N_EXPLOSIONS; ++i) {
        draw_manager.release_sprite_id(explosions[i].spr_id);
    }

    ModeBase::exit();
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

    float x = 640;
    float y = 512;
    BattleShipAction act = BSA_Idle;

    // FIXME: Prevent placement collisions?

    if (enemy) {
        switch (type) {
            case SHIP_Warship:
                x = 600 + RND(300);
                y = 600 + RND(300);
                act = BSA_AttackSlow;
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
                act = BSA_AttackSlow;
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
                act = BSA_AttackSlow;
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
                act = BSA_AttackSlow;
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
                act = BSA_Report;
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
    L.debug("Placing %s ship at %f %f", enemy?"CPU":"human", x, y);
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
    player = exostate().get_active_player();
    SpaceBattleParams &b = ephstate.space_battle;

    selected = nullptr;

    for (int i = 0; i < MAX_SHIPS; ++i) {
        ships[i].exists = false;
    }

    for (int i = 0; i < MAX_ROCKETS; ++i) {
        rockets[i].exists = false;
    }

    next_ship = &ships[0];

    int shields = player->get_starship().shield_generators;
    starship = place(SHIP_Starship, false, 1, shields*5);

    distribute(SHIP_Warship, true, b.enemy_ships, 10, 4);
    distribute(SHIP_Scout, true, b.enemy_scouts, 5, 2);
    if (b.enemy_cargo) {
        place(SHIP_Transporter, true, b.enemy_cargo);
    }

    const Fleet &f = player->get_fleet();

    initial_warships = f.warships;

    distribute(SHIP_Warship, false, f.warships, 10, 4);
    distribute(SHIP_Bomber, false, f.bombers, 10, 3);
    distribute(SHIP_Scout, false, f.scouts, 10, 5);
    if (f.transporters) {
        place(SHIP_Transporter, false, f.transporters);
    }
}

void SpaceBattle::draw() {
    bool draw_full_detail = full_detail && !fail_battle_readout_this_frame;

    // Draw ships
    for (int i = 0; i < MAX_SHIPS; ++i) {
        BattleShip& s = ships[i];

        if (!s.exists) {
            continue;
        }

        if (s.hp <= 0) {
            continue;
        }

        int draw_x = (int)(s.x / 2.f);
        int draw_y = (int)(s.y / 2.f);

        bool sel = false;
        if (s.enemy) {
            sel = (selected && selected->target == &s);
        } else {
            sel = (selected == &s);
        }

        draw_manager.draw(
            s.spr_id,
            fail_battle_readout_this_frame ? nullptr : (s.draw_hit ? IMG_RD1_HIT : (sel ? s.spr_sel : s.spr)),
            {draw_x, draw_y,
             .5f, .5f, 1, 1});

        draw_manager.draw(
            s.spr_id_label,
            fail_battle_readout_this_frame ? nullptr : s.spr_label,
            {draw_x + 24, draw_y - 10,
             .5f, .5f, 1, 1});

        if (s.enemy) {
            if (s.draw_hp) {
                char t[4];
                snprintf(t, sizeof(t), "%03d", s.hp);

                draw_manager.draw_text(
                    s.spr_id_label_hp,
                    Font::_3x5,
                    t,
                    Justify::Right,
                    draw_x - 20,
                    draw_y - 18,
                    COL_TEXT_BAD);
            } else {
                draw_manager.draw(s.spr_id_label_hp, nullptr);
            }
        } else {
            if (draw_full_detail) {
                char t[4];
                snprintf(t, sizeof(t), "%03d", s.hp);

                draw_manager.draw_text(
                    s.spr_id_label_hp,
                    Font::_3x5,
                    t,
                    Justify::Left,
                    draw_x + 40,
                    draw_y - 18,
                    COL_TEXT2);

                const char* act = nullptr;
                switch (s.action) {
                    case BSA_Idle:
                        act = IMG_RD1_ACT0;
                        break;
                    case BSA_AttackSlow:
                        act = IMG_RD1_ACT1;
                        break;
                    case BSA_AttackFast:
                        act = IMG_RD1_ACT3;
                        break;
                    case BSA_Report:
                        act = IMG_RD1_ACT4;
                        break;
                    case BSA_Move:
                        act = IMG_RD1_ACT5;
                        break;
                    case BSA_MAX:
                        break;
                }

                draw_manager.draw(
                    s.spr_id_label_action,
                    act,
                    {draw_x + 24, draw_y + 2,
                     .5f, .5f, 1, 1});
            } else {
                draw_manager.draw(s.spr_id_label_hp, nullptr);
                draw_manager.draw(s.spr_id_label_action, nullptr);
            }
        }
    }

    // Draw rockets
    for (int i = 0; i < MAX_ROCKETS; ++i) {
        const Rocket &r = rockets[i];

        if (!r.exists) {
            continue;
        }

        int draw_x = (int)(r.x / 2.f);
        int draw_y = (int)(r.y / 2.f);

        draw_manager.draw(
            r.spr_id,
            fail_battle_readout_this_frame ? nullptr : r.get_sprite(),
            {draw_x, draw_y,
             .5f, .5f, 1, 1});
    }

    if (fail_ship_stats < 0) {
        // Draw top-left ship counts
        int count_wsp = count_ships(SHIP_Warship, false);
        int count_bmb = count_ships(SHIP_Bomber, false);
        int count_sct = count_ships(SHIP_Scout, false);
        int count_trn = count_ships(SHIP_Transporter, false);
        int count_enm = count_ships(SHIP_Warship, true);

        char count[4];
        count[3] = '\0';

        snprintf(count, sizeof(count), "%03d", count_wsp);
        count[3] = '\0';
        draw_manager.draw_text(
            id(ID::COUNT_WARSHIPS),
            Font::_3x5,
            count,
            Justify::Right,
            104, 30,
            COL_TEXT2);

        snprintf(count, sizeof(count), "%03d", count_bmb);
        count[3] = '\0';
        draw_manager.draw_text(
            id(ID::COUNT_BOMBERS),
            Font::_3x5,
            count,
            Justify::Right,
            104, 48,
            COL_TEXT2);

        snprintf(count, sizeof(count), "%03d", count_sct);
        count[3] = '\0';
        draw_manager.draw_text(
            id(ID::COUNT_SCOUTS),
            Font::_3x5,
            count,
            Justify::Right,
            104, 66,
            COL_TEXT2);

        snprintf(count, sizeof(count), "%03d", count_trn);
        count[3] = '\0';
        draw_manager.draw_text(
            id(ID::COUNT_TRANSPORTS),
            Font::_3x5,
            count,
            Justify::Right,
            104, 84,
            COL_TEXT2);

        snprintf(count, sizeof(count), "%03d", count_enm);
        count[3] = '\0';
        draw_manager.draw_text(
            id(ID::COUNT_ENEMY),
            Font::_3x5,
            count,
            Justify::Right,
            104, 110,
            COL_TEXT2);
    } else {
        // System failure
        draw_manager.draw(
            id(ID::FAIL_SHIP_STATS),
            fail_anim.frame(fail_ship_stats),
            {10, 10, 0, 0, 1, 1});
    }

    if (fail_diagnostics < 0) {
        // Drop top-right ship diagnostics
        // FIXME: There can be some graphical corruption here...
        for (int i = 0; i < N_EXPLOSIONS; ++i) {
            Explosion &exp = explosions[i];
            const char* spr = nullptr;
            if (do_explosions && exp.frame >= 0 && exp.frame < exp_anim.get_n_frames()) {
                spr = exp_anim.frame(exp.frame);
            }
            draw_manager.draw(
                exp.spr_id,
                spr,
                {exp.x, exp.y, 0, 0, 1, 1});
        }

        int dmg = player->get_starship().pct_damage_struct;
        const char *health_spr = IMG_RD1_ST1;
        if (dmg > 70) {
            health_spr = IMG_RD1_ST4;
        } else if (dmg > 50) {
            health_spr = IMG_RD1_ST3;
        } else if (dmg > 0) {
            health_spr = IMG_RD1_ST2;
        }
        draw_manager.draw(
            id(ID::HEALTH_INDICATOR),
            health_spr,
            {618, 126, 0, 0, 1, 1});
        draw_manager.draw(
            id(ID::BOW),
            IMG_RD1_BOW,
            {610, 118, 0, 0, 1, 1});
    } else {
        // System failure
        draw_manager.draw(
            id(ID::FAIL_DIAGNOSTICS),
            fail_anim.frame(fail_diagnostics),
            {RES_X - 10, 10, 1, 0, 1, 1});
    }

    // Draw buttons
    if (!FEATURE(EF_SPACE_BATTLE_LAYOUT)) {
        const char* button_detail = nullptr;
        const char* button_auto = nullptr;

        if (FEATURE(EF_FLIP_BUTTONS)) {
            button_detail = full_detail ? nullptr : IMG_RD1_F1 ;
            button_auto = auto_battle ? IMG_RD1_A1 : nullptr;
        } else {
            button_detail = full_detail ? IMG_RD1_F1 : nullptr;
            button_auto = auto_battle ? nullptr : IMG_RD1_A1;
        }

        draw_manager.draw(
            id(ID::BUTTON_DETAIL),
            button_detail,
            {AREA_DETAIL.x, AREA_DETAIL.y, 0, 0, 1, 1});
        draw_manager.draw(
            id(ID::BUTTON_AUTO),
            button_auto,
            {AREA_AUTO.x, AREA_AUTO.y, 0, 0, 1, 1});
    }
}

void SpaceBattle::update_mouse() {
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

        // FIXME: This isn't a good trigger, because labels flicker during fail_battle_readout
        if (draw_manager.query_click(ships[i].spr_id_label).id) {
            ships[i].cycle_action();
        }
        if (draw_manager.query_click(ships[i].spr_id_label_action).id) {
            ships[i].cycle_action();
        }
    }
}

SpaceBattle::Stage SpaceBattle::update_buttons() {
    if (FEATURE(EF_SPACE_BATTLE_LAYOUT)) {
        draw_manager.draw(
            id(ID::BTN_ENG),
            auto_battle ? IMG_BTN_ENG_DOWN : IMG_BTN_ENG,
            {518, 400, 0, 0, 1, 1});

        draw_manager.draw(
            id(ID::BTN_DATA),
            full_detail ? IMG_BTN_DATA_DOWN : IMG_BTN_DATA,
            {518, 428, 0, 0, 1, 1});

        if (draw_manager.query_click(id(ID::BTN_ENG)).id) {
            auto_battle = !auto_battle;
        }

        if (draw_manager.query_click(id(ID::BTN_DATA)).id) {
            full_detail = !full_detail;
        }

        if (draw_manager.query_click(id(ID::BTN_INFO)).id) {
            return SB_Info;
        }

        if (draw_manager.query_click(id(ID::BTN_QUIT)).id) {
            return SB_Surrender;
        }
    } else {
        if (!draw_manager.clicked()) {
            return stage;
        }

        if (draw_manager.mouse_in_area(AREA_INFO)) {
            return SB_Info;
        }

        if (draw_manager.mouse_in_area(AREA_DETAIL)) {
            full_detail = !full_detail;
            return stage;
        }

        if (draw_manager.mouse_in_area(AREA_AUTO)) {
            auto_battle = !auto_battle;
            return stage;
        }

        if (draw_manager.mouse_in_area(AREA_QUIT)) {
            return SB_Surrender;
        }
    }

    return stage;
}

void SpaceBattle::ships_think() {
    for (int i = 0; i < MAX_SHIPS; ++i) {
        BattleShip *s = &ships[i];
        if (!s->exists) {
            continue;
        }

        if (s->hp <= 0) {
            continue;
        }

        if (!auto_battle && !s->enemy) {
            continue;
        }

        if (s->target) {
            // Unset invalid targets
            if (!(s->target->exists && s->target->hp > 0)) {
                s->target = nullptr;
            }
        }

        // If we don't have a target, pick a new one
        if (!s->target) {
            if (s->enemy) {
                // Enemy target selection
                int r = RND(5);
                if (r < 3) {
                    s->target = find_ship(SHIP_Warship, !s->enemy);
                    if (!s->target) s->target = starship;
                } else if (r == 3) {
                    s->target = find_ship(SHIP_Bomber, !s->enemy);
                    if (!s->target) s->target = starship;
                } else if (r == 4) {
                    s->target = starship;
                } else {
                    if (onein(3)) {
                        s->target = find_ship(SHIP_Transporter, !s->enemy);
                    } else {
                        s->target = find_ship(SHIP_Scout, !s->enemy);
                    }
                }
            } else {
                // Player target selection
                s->target = find_ship(SHIP_Warship, !s->enemy);
            }
        }

        if (s->action == BSA_AttackSlow && onein(50)) {
            s->action = BSA_AttackFast;
        }

        if (s->action == BSA_AttackFast && onein(50)) {
            s->action = BSA_AttackSlow;
        }
    }
}

void SpaceBattle::update_ships() {
    for (int i = 0; i < MAX_SHIPS; ++i) {
        BattleShip *s = &ships[i];

        if (!s->exists) {
            continue;
        }

        if (!(s->target && s->target->exists && s->target->hp > 0)) {
            continue;
        }

        if (s->action == BSA_Idle) {
            continue;
        }

        float tx = s->target->x;
        float ty = s->target->y;

        float sa = tx - s->x;
        float sb = ty - s->y;

        float denom = sqrt(sa*sa + sb*sb);
        float dx = 2 * sa / denom;
        float dy = 2 * sb / denom;

        if (s->type == SHIP_Scout) {
            dx *= 2;
            dy *= 2;
        }

        if (s->type != SHIP_Starship) {
            if ((sa*sa) + (sb*sb) > (50*50)) {
                s->x += dx;
                s->y += dy;
            } else {
                int rdx = (2 - (rand() % 5));
                int rdy = (2 - (rand() % 5));

                // Bias towards moving away
                if (!((rdx > 0) ^ (dx > 0))) {
                    if (!onein(3)) {
                        rdx = -rdx;
                    }
                }
                if (!((rdy > 0) ^ (dy > 0))) {
                    if (!onein(3)) {
                        rdy = -rdy;
                    }
                }
                s->x += rdx;
                s->y += rdy;
            }
        }

        bool fire = (s->action == BSA_AttackSlow && onein(15));
        fire = fire || (s->action == BSA_AttackFast && onein(7));
        if (fire) {
            spawn_rocket(s, dx*4, dy*4);
        }

        // Restore shields
        if (s->shield < s->shield_max && onein(30)) {
            s->shield++;
        }

        if (s->type == SHIP_Starship) {
            if (s->target && s->target->exists && s->target->hp > 0) {
                // FIXME: Orig does this outside of the loop - and ends up using the wrong dx / dy! We'll fix this...
                for (int i = 0; i < player->get_starship().laser_guns; ++i) {
                    if (onein(15)) {
                        spawn_rocket(s, dx*4, dy*4);
                    }
                }
            }
        }
    }
}

void SpaceBattle::update_rockets() {
    for (int i = 0; i < MAX_ROCKETS; ++i) {
        if (!rockets[i].exists) {
            continue;
        }
        rockets[i].update();
    }
}

void SpaceBattle::ships_act() {
    for (int i = 0; i < MAX_SHIPS; ++i) {
        BattleShip *s = &ships[i];

        if (!(s->exists && s->hp > 0)) {
            continue;
        }

        if (s->action == BSA_AttackSlow || s->action == BSA_AttackFast) {
            // PROCr_doattack
            BattleShip *t = s->target;

            if (!t) {
                continue;
            }

            if (s->type == SHIP_Starship) {
                // FIXME: Copying orig, but what's going on here?
                s->hp = 10;
                for (int i = 0; i < player->get_starship().laser_guns; ++i) {
                    do_attack(s);
                }
                if (player->get_starship().missile_launchers > 0) {
                    if (onein(10)) {
                        player->get_starship().missile_launchers--;
                        for (int j = 0; j < 3; ++j) {
                            do_attack(s);
                        }
                    }
                }
            } else {
                do_attack(s);
            }
        } else if (s->action == BSA_Report) {
            // PROCr_report

            if (fail_battle_readout_this_frame) {
                continue;
            }

            BattleShip *t = s->target;

            if (!t) {
                continue;
            }

            if (t->hp <= 0) {
                s->target = nullptr;
                continue;
            }

            int dx = abs(s->x - t->x);
            int dy = abs(s->y - t->y);

            // This is what the original does...!
            int dist = sqrt(dx) + sqrt(dy);

            if (dist > 20) {
                continue;
            }

            s->target->draw_hp = true;
        }
    }
}

void SpaceBattle::do_attack(BattleShip* s) {
    // PROCr_attack
    BattleShip *t = s->target;

    if (!(s && t && t->exists)) {
        return;
    }

    if (t->hp <= 0) {
        s->target = nullptr;
        return;
    }

    int hits = 0;

    int iters = s->hp;

    if (FEATURE(EF_FIX_FLEET_ADMIRAL)) {
        if (!s->enemy) {
            OfficerQuality offq = player->get_officer(OFF_Fleet);
            if (offq == OFFQ_Average) {
                iters = (int)(((float)iters) * 1.5f);
            }
            if (offq == OFFQ_Good) {
                iters = (int)(((float)iters) * 2.0f);
            }
        }
    }

    for (int i = 0; i < iters; ++i) {
        int r = (s->action == BSA_AttackFast) ? 160 : 200;
        if (t->action == BSA_AttackFast) {
            r -= 40;
        }
        if (s->type == SHIP_Bomber) {
            r += 100;
        }
        if (onein(r)) {
            hits++;
        }
    }

    if (hits == 0) {
        return;
    }

    // PROCr_hit

    if (t->type == SHIP_Starship) {
        if (t->shield <= 0) {
            if (!do_explosions) {
                start_explosions();
            }
            if (fail_ship_stats < 0 && onein(20))
                fail_ship_stats = 0;
            if (fail_diagnostics < 0 && onein(20))
                fail_diagnostics = 0;
            if ((!fail_battle_readout) && onein(30))
                fail_battle_readout = true;
        }
        // PROCr_hitstsh
        int intern = 0;

        t->shield -= 1;
        if (t->shield < 0) {
            intern = -t->shield;
            t->shield = 0;
        }

        if (intern > 0) {
            for (int i = 0; i < intern; ++i) {
                int r = RND(10);
                int *d;
                if (r <= 7) {
                    d = &player->get_starship().pct_damage_struct;
                    *d = min((*d)+4, 100);
                    L.info("STARSHIP STRUCTURE: %d%%", 100-(*d));
                    if ((*d) >= 100) {
                        resolution = SBRES_StarshipDestroyed;
                    }
                } else if (r <= 9) {
                    d = &player->get_starship().pct_damage_thrust;
                    *d = min((*d)+4, 100);
                    L.info("STARSHIP THRUSTERS: %d%%", 100-(*d));
                } else {
                    d = &player->get_starship().pct_damage_comms;
                    *d = min((*d)+6, 100);
                    L.info("STARSHIP COMMS: %d%%", 100-(*d));
                }
            }
        }

        // SUGGEST: Orig had a half-implemented screen wobble here - maybe experiment with that
    } else {
        L.info("%s ship type %d HP: %d->%d", t->enemy?"CPU":"Human", (int)t->type, t->hp, t->hp - hits);
        int t_hp_initial = t->hp;
        t->hp = max(t->hp - hits, 0);

        if (t->enemy) {
            enemy_ships_destroyed += (t_hp_initial - t->hp);
        } else {
            int *count = nullptr;
            switch (t->type) {
                case SHIP_Warship:
                    count = &player->get_fleet_nonconst().warships;
                    break;
                case SHIP_Bomber:
                    count = &player->get_fleet_nonconst().bombers;
                    break;
                case SHIP_Scout:
                    count = &player->get_fleet_nonconst().scouts;
                    break;
                case SHIP_Transporter:
                    count = &player->get_fleet_nonconst().transporters;
                    break;
                default:
                    L.error("No case to handle destruction of player ship type %d", (int)t->type);
                    break;
            }

            if (count) {
                *count = max(*count - hits, 0);
            }
        }

        if (t->hp <= 0) {
            s->target = nullptr;
            if (selected && selected->target == t) {
                selected->target = nullptr;
            }
            t->cleanup();
        }
    }

    t->draw_hit = true;

    audio_manager.play_sfx(SFX_EXPLOSION);
}

void SpaceBattle::start_explosions() {
    do_explosions = true;
    explosions[0].x = 565;
    explosions[0].y = 91;
    explosions[0].frame = -8;
    for (int i = 1; i < N_EXPLOSIONS; ++i) {
        Explosion &exp = explosions[i];
        exp.x = 552 + RND(26);
        exp.y = 78 + RND(26);
        exp.frame = RND(15) * -1;
    }
}

void SpaceBattle::update_battle() {
    if (do_explosions) {
        bool stop_explosions = true;
        for (int i = 0; i < N_EXPLOSIONS; ++i) {
            Explosion &exp = explosions[i];
            if (++exp.frame < exp_anim.get_n_frames()) {
                stop_explosions = false;
            }
        }
        do_explosions = !stop_explosions;
    }

    if (fail_ship_stats >= 0) {
        fail_ship_stats = (fail_ship_stats + 1) % FAIL_SPRITES;
    }
    if (fail_diagnostics >= 0) {
        fail_diagnostics = (fail_diagnostics + 1) % FAIL_SPRITES;
    }

    // This 'flickers' if fail_battle_readout is true
    fail_battle_readout_this_frame = fail_battle_readout && onein(4);

    for (int i = 0; i < MAX_SHIPS; ++i) {
        ships[i].draw_hp = false;
        ships[i].draw_hit = false;
    }

    ships_think();
    update_ships();
    update_rockets();
    ships_act();

    if (onein(4)) audio_manager.play_sfx(SFX_GLIDE_LOW);
    if (onein(10)) audio_manager.play_sfx(SFX_SHOT);
}

ExodusMode SpaceBattle::update(float delta) {
    if (bulletin_is_open()) {
        bulletin_update(delta);
        switch (bulletin_get_praction()) {
            case BPR_Close:
                bulletin_ensure_closed();
                break;
            default:
                break;
        }
        return ExodusMode::MODE_None;
    }

    if (draw_manager.query_click(id(ID::BTN_GUIDE)).id) {
        bulletin_start_manual(BMP_SpaceCombat_3);
        return ExodusMode::MODE_None;
    }

    SpaceBattleParams &b = ephstate.space_battle;

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

                if (resolution == SBRES_None) {
                    if (!find_ship(SHIP_Warship, true)) {
                        resolution_delay = 0;
                        resolution = SBRES_Won;

                        if (b.enemy_hostile) {
                            achievement_manager.unlock(ACH_DefeatHostileFleet);
                        }

                        L.info("BATTLE WON");
                    }
                }

                frame_time_elapsed = 0;
                stage = SB_Wait;

                switch (resolution) {
                    case SBRES_None:
                        break;
                    case SBRES_Won:
                        stage = SB_Resolved;
                        return ExodusMode::MODE_None;
                    case SBRES_StarshipDestroyed:
                        L.info("BATTLE LOST: Starship destroyed");
                        draw_manager.fade_black(1.2f, 24);
                        stage = SB_Resolved;
                        return ExodusMode::MODE_None;
                    case SBRES_Surrendered:
                        // Shouldn't need to detect here - stage is set when we press the surrender button
                        stage = SB_Resolved;
                        return ExodusMode::MODE_None;
                }
            }
            // Fall through here - handle UI etc in SB_Wait
        case SB_Wait:
            {
                update_mouse();

                frame_time_elapsed += delta;
                if (frame_time_elapsed >= 0.4) {
                    stage = SB_Battle;
                }

                stage = update_buttons();
            }
            break;
        case SB_Info:
            {
                if (!panel_showing) {
                    L.debug("Info button clicked");

                    draw_manager.fill(
                        id(ID::PANEL),
                        {PANEL_X - BORDER, PANEL_Y - BORDER,
                         PANEL_W + 2*BORDER, PANEL_H + 2*BORDER},
                        COL_BORDERS);
                    draw_manager.fill(
                        FILL_3D_Out_Hollow,
                        {PANEL_X - BORDER, PANEL_Y - BORDER,
                         PANEL_W + 2*BORDER, PANEL_H + 2*BORDER},
                        COL_BORDERS);
                    draw_manager.fill_pattern(
                        {PANEL_X, PANEL_Y,
                         PANEL_W, PANEL_H});

                    const Starship& ship = player->get_starship();
                    char text[128];

                    const char* status = "INTACT";
                    if (ship.pct_damage_struct > 0) {
                        status = "DAMAGED";
                    }
                    if (ship.pct_damage_struct > 70) {
                        status = "CRITICAL";
                    }

                    snprintf(text, sizeof(text), "Starship status: %s", status);

                    draw_manager.draw_text(
                        text,
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 4,
                        COL_TEXT2);

                    snprintf(text, sizeof(text), "Shields:");
                    draw_manager.draw_text(
                        "Shields:",
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 44,
                        COL_TEXT);
                    if (starship && starship->shield_max > 0) {
                        int sh = starship->shield;
                        int shmax = starship->shield_max;
                        snprintf(text, sizeof(text), "%d%%", 100*sh/shmax);
                    } else {
                        snprintf(text, sizeof(text), "Not fitted");
                    }

                    draw_manager.draw_text(
                        text,
                        Justify::Left,
                        PANEL_X + 160, PANEL_Y + 44,
                        COL_TEXT);

                    snprintf(text, sizeof(text), "%d%%", 100-ship.pct_damage_struct);
                    draw_manager.draw_text(
                        "Structure:",
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 84,
                        COL_TEXT);
                    draw_manager.draw_text(
                        text,
                        Justify::Left,
                        PANEL_X + 160, PANEL_Y + 84,
                        COL_TEXT);

                    snprintf(text, sizeof(text), "%d%%", 100-ship.pct_damage_thrust);
                    draw_manager.draw_text(
                        "Thrust:",
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 104,
                        COL_TEXT);
                    draw_manager.draw_text(
                        text,
                        Justify::Left,
                        PANEL_X + 160, PANEL_Y + 104,
                        COL_TEXT);

                    snprintf(text, sizeof(text), "%d%%", 100-ship.pct_damage_comms);
                    draw_manager.draw_text(
                        "Comm Systems:",
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 124,
                        COL_TEXT);
                    draw_manager.draw_text(
                        text,
                        Justify::Left,
                        PANEL_X + 160, PANEL_Y + 124,
                        COL_TEXT);

                    int ca = 40;
                    ca += count_ships(SHIP_Warship, false) * 5;
                    ca += count_ships(SHIP_Bomber, false) * 2;
                    int cb = max(1, count_ships(SHIP_Warship, true) * 5);
                    float ratio;
                    if (ca > cb) {
                        ratio = ((int)(((float)ca/(float)cb)*100))/100.f;
                        snprintf(text, sizeof(text), "Winning chances are %.2f : 1", ratio);
                    } else {
                        ratio = ((int)(((float)cb/(float)ca)*100))/100.f;
                        snprintf(text, sizeof(text), "Winning chances are 1 : %.2f", ratio);
                    }

                    draw_manager.draw_text(
                        text,
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 164,
                        COL_TEXT);

                    panel_showing = true;
                }

                if (draw_manager.clicked()) {
                    draw_manager.draw(id(ID::PANEL), nullptr);
                    panel_showing = false;
                    stage = SB_Wait;
                } else {
                    // Skip normal draw etc
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case SB_Surrender:
            {
                if (!panel_showing) {
                    L.debug("Surrender button clicked");

                    draw_manager.fill(
                        id(ID::PANEL),
                        {PANEL_X - BORDER, PANEL_Y - BORDER,
                         PANEL_W + 2*BORDER, PANEL_H + 2*BORDER},
                        COL_BORDERS);
                    draw_manager.fill(
                        FILL_3D_Out_Hollow,
                        {PANEL_X - BORDER, PANEL_Y - BORDER,
                         PANEL_W + 2*BORDER, PANEL_H + 2*BORDER},
                        COL_BORDERS);
                    draw_manager.fill_pattern(
                        {PANEL_X, PANEL_Y,
                         PANEL_W, PANEL_H});

                    draw_manager.draw_text(
                        "SURRENDER?",
                        Justify::Centre,
                        RES_X/2, PANEL_Y + 28,
                        COL_TEXT);

                    draw_manager.fill(FILL_3D_Out, {PANEL_X+2, PANEL_Y+PANEL_H-46, 41, 26}, COL_BORDERS);
                    draw_manager.fill(FILL_3D_Out, {PANEL_X+PANEL_W-43, PANEL_Y+PANEL_H-46, 41, 26}, COL_BORDERS);

                    draw_manager.draw(
                        id(ID::SURRENDER_OPT),
                        IMG_BR14_EXPORT,
                        {RES_X/2, PANEL_Y+PANEL_H-20,
                         .5f, 1, 1, 1});

                    panel_showing = true;
                }

                SpriteClick clk = draw_manager.query_click(id(ID::SURRENDER_OPT));

                if (clk.id) {
                    draw_manager.draw(id(ID::SURRENDER_OPT), nullptr);
                    draw_manager.draw(id(ID::PANEL), nullptr);
                    panel_showing = false;

                    if (clk.x < .5f) {
                        resolution = SBRES_Surrendered;
                        stage = SB_Resolved;
                    } else {
                        stage = SB_Wait;
                    }
                }

                return ExodusMode::MODE_None;
            }
            break;
        case SB_Resolved:
            {
                if (draw_manager.fade_active()) {
                    return ExodusMode::MODE_None;
                }

                if (resolution_delay == 0) {
                    if (resolution == SBRES_StarshipDestroyed) {
                        draw_manager.draw(IMG_INTRO_SPACE);
                    }
                } else if (resolution_delay > 1.2f) {
                    resolution_delay = 0;
                    switch (resolution) {
                        case SBRES_Won:
                            stage = SB_Won;
                            break;
                        case SBRES_StarshipDestroyed:
                            stage = SB_Destroyed;
                            break;
                        case SBRES_Surrendered:
                            stage = SB_Surrendered;
                            break;
                        default:
                            L.error("Unexpected resolution: %d", (int)resolution);
                            break;
                    }
                    return ExodusMode::MODE_None;
                }

                resolution_delay += delta;
                return ExodusMode::MODE_None;
            }
            break;
        case SB_Destroyed:
            {
                if (resolution_delay == 0) {
                    draw_manager.draw(TGT_Secondary, IMG_BG_SHIP0);
                    draw_manager.draw_text(
                        TGT_Secondary,
                        Font::Large,
                        "Your starship has been destroyed.",
                        Justify::Left, 40, 40,
                        COL_TEXT2);

                    if (player->get_starship().escape_capsule) {
                        draw_manager.draw_text(TGT_Secondary, Font::Default,
                            "The escape capsule has been activated",
                            Justify::Left, 40, 100, COL_TEXT);
                        draw_manager.draw_text(TGT_Secondary, Font::Default,
                            "and has saved you and your command staff.",
                            Justify::Left, 40, 120, COL_TEXT);
                        draw_manager.draw_text(TGT_Secondary, Font::Default,
                            "Watching this, the pirate ships fled,",
                            Justify::Left, 40, 160, COL_TEXT);
                        draw_manager.draw_text(TGT_Secondary, Font::Default,
                            "leaving your entire fleet alone.",
                            Justify::Left, 40, 180, COL_TEXT);
                        /*
                         * Orig picked an arbitrary owned planet name here, or
                         * if none owned, printed a "non-planetary orbit" -
                         * however testing, it seems both had the same effect
                         * and you'd just continue your flight plan as if nothing
                         * had changed. We retain this behaviour - but just print
                         * "your destination" so the description at least fits
                         * that behaviour.
                         *
                         * Anyway, if you have no planets and have now blown up
                         * your starship, you have bigger problems.
                         */
                        draw_manager.draw_text(TGT_Secondary, Font::Default,
                            "A new ship is going to be built at your destination.",
                            Justify::Left, 40, 220, COL_TEXT);
                    } else {
                        draw_manager.draw_text(TGT_Secondary, Font::Default,
                            "And without an escape capsule,",
                            Justify::Left, 40, 100, COL_TEXT);
                        draw_manager.draw_text(TGT_Secondary, Font::Default,
                            "you and your officers did not",
                            Justify::Left, 40, 120, COL_TEXT);
                        draw_manager.draw_text(TGT_Secondary, Font::Default,
                            "survive.",
                            Justify::Left, 40, 140, COL_TEXT);
                        draw_manager.draw_text(TGT_Secondary, Font::Default,
                            "Without a leader, your empire",
                            Justify::Left, 40, 180, COL_TEXT);
                        draw_manager.draw_text(TGT_Secondary, Font::Default,
                            "collapses into little independent",
                            Justify::Left, 40, 200, COL_TEXT);
                        draw_manager.draw_text(TGT_Secondary, Font::Default,
                            "governments which are soon taken",
                            Justify::Left, 40, 220, COL_TEXT);
                        draw_manager.draw_text(TGT_Secondary, Font::Default,
                            "over by your enemies.",
                            Justify::Left, 40, 240, COL_TEXT);
                    }

                    draw_manager.pixelswap_start();
                }

                if (resolution_delay > 0 && !draw_manager.pixelswap_active()) {
                    if (draw_manager.clicked()) {
                        if (player->get_starship().escape_capsule) {
                            player->get_starship().reset();
                            player->get_starship().pct_damage_thrust = 99;
                            player->get_starship().pct_damage_comms = 99;
                            player->get_starship().pct_damage_struct = 99;
                            stage = SB_Exit;
                        } else {
                            ephstate.game_over_reason = GAMEOVER_Dead;
                            ephstate.set_ephemeral_state(EPH_GameOver);
                            return ephstate.get_appropriate_mode();
                        }
                    }
                }

                resolution_delay += delta;
                return ExodusMode::MODE_None;
            }
            break;
        case SB_Surrendered:
            {
                if (resolution_delay == 0) {
                    draw_manager.draw(TGT_Secondary, IMG_BG_SHIP0);
                    draw_manager.draw_text(
                        TGT_Secondary,
                        Font::Large,
                        "The starship has been boarded.",
                        Justify::Left, 40, 40,
                        COL_TEXT2);

                    int mc = player->remove_all_mc(MC_SpaceBattleSurrendered);

                    Fleet &fl = player->get_fleet_nonconst();
                    int stolen_transporters = min(RND(7), fl.transporters);
                    fl.transporters = max(0, fl.transporters - stolen_transporters);

                    int y = 100;
                    char text[64];
                    const char *pl;

                    snprintf(text, sizeof(text), "The enemy took %dMC", mc);
                    draw_manager.draw_text(
                        TGT_Secondary, Font::Default,
                        text,
                        Justify::Left, 40, y,
                        COL_TEXT);
                    y += 20;

                    pl = stolen_transporters == 1 ? "" : "s";
                    snprintf(text, sizeof(text), "and %d transporter%s.", stolen_transporters, pl);
                    draw_manager.draw_text(
                        TGT_Secondary, Font::Default,
                        text,
                        Justify::Left, 40, y,
                        COL_TEXT);
                    y += 40;

                    y = report_starship_condition(y);

                    draw_manager.pixelswap_start();
                }

                if (resolution_delay > 0 && !draw_manager.pixelswap_active()) {
                    if (draw_manager.clicked()) {
                        stage = SB_Exit;
                    }
                }

                resolution_delay += delta;
                return ExodusMode::MODE_None;
            }
        case SB_Won:
            {
                if (resolution_delay == 0) {
                    draw_manager.draw(TGT_Secondary, IMG_BG_SHIP0);
                    draw_manager.draw_text(
                        TGT_Secondary,
                        Font::Large,
                        "The battle is over.",
                        Justify::Left, 40, 40,
                        COL_TEXT2);

                    int y = 100;
                    char text[64];
                    const char *pl;


                    int captured_scouts = count_ships(SHIP_Scout, true);
                    int captured_transporters = count_ships(SHIP_Transporter, true);

                    if (captured_scouts + captured_transporters > 0) {
                        draw_manager.draw_text(
                            TGT_Secondary, Font::Default,
                            "We have captured these enemy ships:",
                            Justify::Left, 40, y,
                            COL_TEXT);
                        y += 20;

                        if (captured_scouts > 0) {
                            pl = captured_scouts == 1 ? "" : "s";
                            snprintf(text, sizeof(text), "%d scout%s.", captured_scouts, pl);
                            draw_manager.draw_text(
                                TGT_Secondary, Font::Default,
                                text,
                                Justify::Left, 40, y,
                                COL_TEXT);
                            player->get_fleet_nonconst().scouts += captured_scouts;
                            y += 20;
                        }

                        if (captured_transporters > 0) {
                            pl = captured_transporters == 1 ? "" : "s";
                            snprintf(text, sizeof(text), "%d transporter%s.", captured_transporters, pl);
                            draw_manager.draw_text(
                                TGT_Secondary, Font::Default,
                                text,
                                Justify::Left, 40, y,
                                COL_TEXT);
                            player->get_fleet_nonconst().transporters += captured_transporters;
                            y += 20;
                        }

                        y += 20;
                    }

                    if (FEATURE(EF_SPACE_BATTLE_MC)) {
                        if (enemy_ships_destroyed > 0) {
                            int mc = RND(2*enemy_ships_destroyed);
                            exostate().get_active_player()->give_mc(mc, MC_SpaceBattleLoot);

                            snprintf(text, sizeof(text), "You were able to liberate %dMC", mc);

                            draw_manager.draw_text(
                                TGT_Secondary, Font::Default,
                                text,
                                Justify::Left, 40, y,
                                COL_TEXT);
                            y += 20;

                            draw_manager.draw_text(
                                TGT_Secondary, Font::Default,
                                "from the enemy fleet.",
                                Justify::Left, 40, y,
                                COL_TEXT);
                            y += 40;
                        }
                    }

                    const Starship &ship = player->get_starship();

                    if (ship.repair_hangar) {
                        const int &war = player->get_fleet().warships;
                        int repaired = RND(20);
                        // FIXME: This limit of 250 isn't imposed in fleet production etc
                        if (war + repaired > 250) {
                            repaired = max(250 - war, 0);
                        }
                        if (war + repaired > initial_warships) {
                            repaired = max(initial_warships - war, 0);
                        }
                        if (repaired > 0) {
                            player->get_fleet_nonconst().warships += repaired;

                            draw_manager.draw_text(
                                TGT_Secondary, Font::Default,
                                "Our techs have been able to repair",
                                Justify::Left, 40, y,
                                COL_TEXT);
                            y += 20;

                            pl = repaired == 1 ? "" : "s";
                            snprintf(text, sizeof(text), "%d warship%s.", repaired, pl);
                            draw_manager.draw_text(
                                TGT_Secondary, Font::Default,
                                text,
                                Justify::Left, 40, y,
                                COL_TEXT);
                            y += 40;
                        }
                    }

                    y = report_starship_condition(y);

                    draw_manager.pixelswap_start();
                }

                if (resolution_delay > 0 && !draw_manager.pixelswap_active()) {
                    if (draw_manager.clicked()) {
                        stage = SB_Exit;
                    }
                }

                resolution_delay += delta;
                return ExodusMode::MODE_None;
            }
            break;
        case SB_Exit:
            {
                // Ensure that cargo is destroyed according to any destroyed transporters
                player->cap_freight_randomly();

                for (int i = 0; i < MAX_SHIPS; ++i) {
                    ships[i].cleanup();
                }

                for (int i = 0; i < MAX_ROCKETS; ++i) {
                    rockets[i].cleanup();
                }

                ephstate.set_ephemeral_state(EPH_ResumeFly);
                return ephstate.get_appropriate_mode();
            }
            break;
    }

    draw();

    return ExodusMode::MODE_None;
}

int SpaceBattle::report_starship_condition(int y) {
    const Starship &ship = player->get_starship();
    char text[64];

    draw_manager.draw_text(
        TGT_Secondary, Font::Default,
        "The condition of the starship",
        Justify::Left, 40, y,
        COL_TEXT);
    y += 20;

    const char *cond = "very good";
    if (ship.pct_damage_struct >  5) cond = "good";
    if (ship.pct_damage_struct > 20) cond = "acceptable";
    if (ship.pct_damage_struct > 50) cond = "bad";
    if (ship.pct_damage_struct > 70) cond = "critical";
    if (ship.pct_damage_struct > 85) cond = "dangerous";
    snprintf(text, sizeof(text), "is %s.", cond);

    draw_manager.draw_text(
        TGT_Secondary, Font::Default,
        text,
        Justify::Left, 40, y,
        COL_TEXT);
    y += 40;

    if (ship.pct_damage_thrust > 50) {
        draw_manager.draw_text(
            TGT_Secondary, Font::Default,
            "Heavily damaged: Thrust",
            Justify::Left, 40, y,
            COL_TEXT);
        y += 20;
    }

    // FIXME: Comms become dysfunctional at >50, why >70?
    // FIXME: 'Destroyed' less bad than 'Heavily damaged'?
    if (ship.pct_damage_comms > 70) {
        if (ship.pct_damage_comms > 90) {
            draw_manager.draw_text(
                TGT_Secondary, Font::Default,
                "Heavily damaged: Comm Systems",
                Justify::Left, 40, y,
                COL_TEXT);
        } else {
            draw_manager.draw_text(
                TGT_Secondary, Font::Default,
                "Destroyed: Comm Systems",
                Justify::Left, 40, y,
                COL_TEXT);
        }
        y += 20;
    }

    y += 20;

    return y;
}

BattleShip* SpaceBattle::find_ship(BattleShipType type, bool enemy) {
    for (int i = 0; i < MAX_SHIPS; ++i) {
        if (!(ships[i].exists && ships[i].hp > 0)) {
            continue;
        }

        if (ships[i].type == type && ships[i].enemy == enemy) {
            return &ships[i];
        }
    }

    return nullptr;
}

int SpaceBattle::count_ships(BattleShipType type, bool enemy) {
    int count = 0;

    for (int i = 0; i < MAX_SHIPS; ++i) {
        if (!(ships[i].exists && ships[i].hp > 0)) {
            continue;
        }

        if (ships[i].type == type && ships[i].enemy == enemy) {
            count += ships[i].hp;
        }
    }

    return count;
}

Rocket* SpaceBattle::spawn_rocket(BattleShip* ship, float dx, float dy) {
    Rocket *r = nullptr;
    for (int i = 0; i < MAX_ROCKETS; ++i) {
        if (!rockets[i].exists) {
            r = &rockets[i];
            break;
        }
    }

    if (r) {
        r->init(ship->x, ship->y, dx, dy);
    } else {
        // Shouldn't be a warning - this is what the original does in normal gameplay
        L.debug("Tried to spawn a new rocket when we are at max");
    }

    return r;
}

void BattleShip::init(BattleShipType _type, bool _enemy, float _x, float _y, int _hp, int _shield) {
    exists = true;
    type = _type;
    enemy = _enemy;
    x = _x;
    y = _y;
    hp = _hp;
    shield_max = _shield;
    shield = shield_max;
    target = nullptr;
    draw_hp = false;
    draw_hit = false;

    spr_id = draw_manager.new_sprite_id();
    spr_id_label = draw_manager.new_sprite_id();
    spr_id_label_hp = draw_manager.new_sprite_id();
    spr_id_label_action = draw_manager.new_sprite_id();

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
    draw_manager.draw(spr_id, nullptr);
    draw_manager.draw(spr_id_label, nullptr);
    draw_manager.draw(spr_id_label_hp, nullptr);
    draw_manager.draw(spr_id_label_action, nullptr);
    if (exists) {
        draw_manager.release_sprite_id(spr_id);
        draw_manager.release_sprite_id(spr_id_label);
        draw_manager.release_sprite_id(spr_id_label_hp);
        draw_manager.release_sprite_id(spr_id_label_action);
    }
    exists = false;
}

void BattleShip::cycle_action() {
    // PROCr_cycleaction
    bool ok = false;
    do {
        action = (BattleShipAction)(((int)action + 1) % (int)BSA_MAX);

        switch (type) {
            case SHIP_Starship:
                ok = (action == BSA_AttackSlow);
                break;
            case SHIP_Warship:
                ok = (action == BSA_AttackSlow || action == BSA_AttackFast);
                break;
            case SHIP_Bomber:
                ok = (action == BSA_AttackSlow || action == BSA_AttackFast);
                break;
            case SHIP_Scout:
                ok = (action == BSA_Report);
                break;
            case SHIP_Transporter:
                ok = (action == BSA_Move);
                break;
        }

        if (action == BSA_Idle) {
            ok = true;
        }
    } while (!ok);
}

void Rocket::init(float _x, float _y, float _dx, float _dy) {
    exists = true;
    ticks_alive = 0;
    x = _x;
    y = _y;
    dx = _dx;
    dy = _dy;
    spr_id = draw_manager.new_sprite_id();
}

void Rocket::update() {
    if (!exists) {
        return;
    }

    x += dx;
    y += dy;

    // Orig is just > here - but we start timer from 0 instead of 1, so use >=
    if (++ticks_alive >= 20) {
        cleanup();
    }
}

const char* Rocket::get_sprite() const {
    if (ticks_alive < 10) {
        return IMG_RD1_TLINE1;
    }
    if (ticks_alive < 15) {
        return IMG_RD1_TLINE2;
    }
    return IMG_RD1_TLINE3;
}

void Rocket::cleanup() {
    draw_manager.draw(spr_id, nullptr);
    if (exists) {
        draw_manager.release_sprite_id(spr_id);
    }
    exists = false;
}
