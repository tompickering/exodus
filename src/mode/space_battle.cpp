#include "space_battle.h"

#include <math.h>

#include "anim.h"
#include "util/value.h"

#include "assetpaths.h"

#define FAIL_SPRITES 3

static const DrawArea AREA_INFO   = {518, 400, 120, 28};
static const DrawArea AREA_DETAIL = {518, 428, 120, 28};
static const DrawArea AREA_AUTO   = {518, 456, 120, 28};
static const DrawArea AREA_QUIT   = {518, 484, 120, 28};

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

    starship = nullptr;

    auto_battle = false;
    frame_time_elapsed = 0;
    resolution = SBRES_None;

    fail_ship_stats = -1;
    fail_diagnostics = -1;
    fail_battle_readout = false;
    fail_battle_readout_this_frame = false;

    for (int i = 0; i < N_EXPLOSIONS; ++i) {
        explosions[i].spr_id = draw_manager.new_sprite_id();
    }

    do_explosions = false;

    SpaceBattleParams &b = ephstate.space_battle;
    L.debug("STARTING BATTLE - ENEMY SHIPS: %d", b.enemy_ships + b.enemy_scouts + b.enemy_cargo);
}

void SpaceBattle::exit() {
    for (int i = 0; i < N_EXPLOSIONS; ++i) {
        draw_manager.release_sprite_id(explosions[i].spr_id);
    }
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
    player = exostate.get_active_player();
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

    distribute(SHIP_Warship, false, f.warships, 10, 4);
    distribute(SHIP_Bomber, false, f.bombers, 10, 3);
    distribute(SHIP_Scout, false, f.scouts, 10, 5);
    if (f.transporters) {
        place(SHIP_Transporter, false, f.transporters);
    }
}

void SpaceBattle::draw() {
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
            Font::Tiny,
            count,
            Justify::Right,
            104, 30,
            COL_TEXT2);

        snprintf(count, sizeof(count), "%03d", count_bmb);
        count[3] = '\0';
        draw_manager.draw_text(
            id(ID::COUNT_BOMBERS),
            Font::Tiny,
            count,
            Justify::Right,
            104, 48,
            COL_TEXT2);

        snprintf(count, sizeof(count), "%03d", count_sct);
        count[3] = '\0';
        draw_manager.draw_text(
            id(ID::COUNT_SCOUTS),
            Font::Tiny,
            count,
            Justify::Right,
            104, 66,
            COL_TEXT2);

        snprintf(count, sizeof(count), "%03d", count_trn);
        count[3] = '\0';
        draw_manager.draw_text(
            id(ID::COUNT_TRANSPORTS),
            Font::Tiny,
            count,
            Justify::Right,
            104, 84,
            COL_TEXT2);

        snprintf(count, sizeof(count), "%03d", count_enm);
        count[3] = '\0';
        draw_manager.draw_text(
            id(ID::COUNT_ENEMY),
            Font::Tiny,
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
    draw_manager.draw(
        id(ID::BUTTON_DETAIL),
        full_detail ? IMG_RD1_F1 : nullptr,
        {AREA_DETAIL.x, AREA_DETAIL.y, 0, 0, 1, 1});
    draw_manager.draw(
        id(ID::BUTTON_AUTO),
        auto_battle ? nullptr : IMG_RD1_A1,
        {AREA_AUTO.x, AREA_AUTO.y, 0, 0, 1, 1});
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

SpaceBattle::Stage SpaceBattle::update_buttons() {
    if (!draw_manager.clicked()) {
        return stage;
    }

    if (draw_manager.mouse_in_area(AREA_INFO)) {
        return SB_Info;
    }

    if (draw_manager.mouse_in_area(AREA_DETAIL)) {
        // TODO: Implement this
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
            s->x += dx;
            s->y += dy;
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
                // FIXME: This is true to orig, but shouldn't this be missile_launchers...?
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
            // TODO: PROCr_report
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

    for (int i = 0; i < s->hp; ++i) {
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
        if (s->shield <= 0) {
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

        s->shield -= 1;
        if (s->shield < 0) {
            intern = -s->shield;
            s->shield = 0;
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

        // TODO: Orig had a half-implemented screen wobble here - maybe experiment with that
    } else {
        L.info("%s ship type %d HP: %d->%d", t->enemy?"CPU":"Human", (int)t->type, t->hp, t->hp - hits);
        t->hp = max(t->hp - hits, 0);
        if (t->hp <= 0) {
            s->target = nullptr;
            if (selected && selected->target == t) {
                selected->target = nullptr;
            }
            // TODO: Is there any sort of explosion effect on death?
            t->cleanup();
        }
    }

    t->draw_hit = true;
    // TODO: Hit SFX at the end of PROCr_hit
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
        ships[i].draw_hit = false;
    }

    ships_think();
    update_ships();
    update_rockets();
    ships_act();
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

                if (resolution == SBRES_None) {
                    if (!find_ship(SHIP_Warship, true)) {
                        resolution = SBRES_Won;
                    }
                }

                frame_time_elapsed = 0;
                stage = SB_Wait;

                switch (resolution) {
                    case SBRES_None:
                        break;
                    case SBRES_Won:
                        // TODO
                        L.info("BATTLE WON");
                        stage = SB_Exit; // Placeholder
                        return ExodusMode::MODE_None;
                    case SBRES_StarshipDestroyed:
                        // TODO
                        L.info("BATTLE LOST: Starship destroyed");
                        stage = SB_Exit; // Placeholder
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
                // TODO
                L.debug("Info button clicked");
                stage = SB_Wait;
            }
            break;
        case SB_Surrender:
            {
                // TODO
                L.debug("Surrender button clicked");
                stage = SB_Wait;
            }
            break;
        case SB_Exit:
            {
                for (int i = 0; i < MAX_SHIPS; ++i) {
                    ships[i].cleanup();
                }

                for (int i = 0; i < MAX_ROCKETS; ++i) {
                    rockets[i].cleanup();
                }

                ephstate.set_ephemeral_state(EPH_ResumeFly);
                return ExodusMode::MODE_Pop;
            }
            break;
    }

    draw();

    return ExodusMode::MODE_None;
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
    draw_hit = false;

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
    draw_manager.draw(spr_id, nullptr);
    draw_manager.draw(spr_id_label, nullptr);
    if (exists) {
        draw_manager.release_sprite_id(spr_id);
        draw_manager.release_sprite_id(spr_id_label);
    }
    exists = false;
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
