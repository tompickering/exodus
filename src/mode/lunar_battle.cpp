#include "lunar_battle.h"

#include <cmath>

#include "anim.h"
#include "assetpaths.h"

#include "util/value.h"

static const int SURF_X =  0;
static const int SURF_Y = 72;
static const int BLK_SZ = 40;
static const float MOVE_RATE = 1.5f;
static const float EXP_RATE = 2.f;
static const int BG_WIDTH = 16;
static const int BG_HEIGHT = 11;

const char* STR_None   = "";
const char* STR_Ground = "Ground";
const char* STR_Inf    = "Infantry";
const char* STR_Gli    = "Gliders";
const char* STR_Art    = "Artillery";
const char* STR_LBGun  = "Base, Gun";
const char* STR_LBCtl  = "Base, Control";
const char* STR_Tele   = "Rescue Beam";
const char* STR_Mine   = "Mine";

enum ID {
    BG,
    BTN_INFO,
    BTN_SPEED,
    BTN_TALK,
    BTN_QUIT,
    PANEL_UNIT_BG,
    PANEL_UNIT,
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    CURSOR,
    EXPLOSION,
    HIGHLIGHT,
    END,
};

static Anim anim_explode(
    7,
    IMG_GF4_XP1,
    IMG_GF4_XP2,
    IMG_GF4_XP3,
    IMG_GF4_XP4,
    IMG_GF4_XP5,
    IMG_GF4_XP6,
    nullptr
);

LunarBattle::LunarBattle() : ModeBase("LunarBattle"), CommPanelDrawer() {
    stage = LB_Move;
    panel_mode = LBPM_None;
    unit_moving = false;
    move_interp = 0;
    shot_interp = 0;
    exp_interp = 0;
    fire_time = 0;
    defender_turn = true;
    target_unit = nullptr;
    cursor_x = -1;
    cursor_y = -1;
    cursor_prev_x = -1;
    cursor_prev_y = -1;
    damage_to_apply = 0;
    panel_unit = nullptr;
}

void LunarBattle::enter() {
    ModeBase::enter(ID::END);
    stage = LB_Move;
    panel_mode = LBPM_None;

    LunarBattleParams &b = ephstate.lunar_battle;

    Planet *p = exostate.get_active_planet();

    Player *owner = nullptr;
    if (p->is_owned()) owner = exostate.get_player(p->get_owner());

    n_units = 0;
    unit_moving = false;
    move_interp = 0;
    shot_interp = 0;
    exp_interp = 0;
    fire_time = 0;
    n_cover = 0;

    cursor_x = -1;
    cursor_y = -1;
    cursor_prev_x = -1;
    cursor_prev_y = -1;

    use_alt_aliens = (bool)(rand() % 2);

    human_turn = true;

    if (b.auto_battle) {
        stage = LB_Auto;
    } else {
        place_cover();
        place_units();
        draw_ground();

        // Ensure this happens *after* place_units()!
        for (int i = 0; i < BATTLE_UNITS_MAX; ++i) {
            units[i].spr_id = draw_manager.new_sprite_id();
        }

        defender_turn = (bool)(owner && owner->is_human());
        reset_round();
        stage = LB_SelectUnit;
    }

    draw_manager.show_cursor(!b.auto_battle);

    // Modified at the start of combat, but best always pointing to something valid
    active_unit = &units[0];
    target_unit = nullptr;
    panel_unit = nullptr;
}

void LunarBattle::draw_ground() {
    Planet *p = exostate.get_active_planet();

    draw_manager.draw(
        id(ID::BG),
        p->moon_sprites()->bg,
        {SURF_X, SURF_Y, 0, 0, 1, 1});

    for (int i = 0; i < n_cover; ++i) {
        const char *spr = p->moon_sprites()->cover0;
        if (cover[i].alt) spr = p->moon_sprites()->cover1;
        draw_manager.draw(
            spr,
            {SURF_X + cover[i].x * BLK_SZ,
             SURF_Y + cover[i].y * BLK_SZ,
             0, 0, 1, 1});
    }

    draw_manager.save_background({SURF_X, SURF_Y, RES_X, RES_Y});
}

void LunarBattle::exit() {
    LunarBattleParams &b = ephstate.lunar_battle;

    if (!b.auto_battle) {
        for (int i = 0; i < BATTLE_UNITS_MAX; ++i) {
            draw_manager.release_sprite_id(units[i].spr_id);
        }
    }

    ModeBase::exit();
}

ExodusMode LunarBattle::update(float delta) {
    LunarBattleParams &b = ephstate.lunar_battle;
    LunarBattleReport &rpt = ephstate.lunar_battle_report;

    Planet *p = exostate.get_active_planet();
    Player *defender = exostate.get_player(p->get_owner());
    Player *aggressor_player = nullptr;

    if (b.aggressor_type == AGG_Player) {
        aggressor_player = exostate.get_player(b.aggressor_idx);
    }

    if (defender_turn) {
        human_turn = defender->is_human();
    } else {
        human_turn = (bool)(aggressor_player && aggressor_player->is_human());
    }

    switch (stage) {
        case LB_Auto:
            // TODO: Auto - in the meantime just fake result!
            // We should return with a result immediately
            rpt.aggressor_units_lost = 11;
            rpt.defender_units_lost = 7;
            rpt.aggressor_won = (bool)(rand() % 2);
            ephstate.set_ephemeral_state(EPH_LunarBattleReport);
            return ephstate.get_appropriate_mode();
        case LB_SelectUnit:
            if (!select_unit()) {
                // Neither side has a unit which can move.
                // Clear turn markers and try again.
                L.info("No units can move - resetting round");
                reset_round();
                if (!select_unit()) {
                    L.fatal("No units can move - someone should have won!");
                }
            }
            L.info("Next unit selected");
            fire_time = 0;
            target_unit = nullptr;
            damage_to_apply = 0;
            stage = LB_Move;
            // Give chance to update human_player
            return ExodusMode::MODE_None;
        case LB_Move:
            if (active_unit->moves_remaining <= 0) {
                stage = LB_Fire;
                break;
            }

            if (move_interp >= 1) {
                unit_moving = false;
                move_interp = 0;
                active_unit->x = active_unit->tgt_x;
                active_unit->y = active_unit->tgt_y;
                active_unit->moves_remaining--;
                // TODO: Check if we've stepped on a mine. If so, LB_CheckWon.
                break;
            }

            if (unit_moving) {
                move_interp += delta * MOVE_RATE;
                if (move_interp > 1) move_interp = 1;
            } else {
                Direction move_dir = DIR_None;

                if (get_valid_move_directions() == 0) {
                    // Movement is impossible - skip this stage
                    active_unit->moves_remaining = 0;
                    break;
                }

                if (human_turn) {
                    if (draw_manager.query_click(id(ID::ARROW_UP)).id)
                        move_dir = DIR_Up;
                    if (draw_manager.query_click(id(ID::ARROW_DOWN)).id)
                        move_dir = DIR_Down;
                    if (draw_manager.query_click(id(ID::ARROW_LEFT)).id)
                        move_dir = DIR_Left;
                    if (draw_manager.query_click(id(ID::ARROW_RIGHT)).id)
                        move_dir = DIR_Right;
                    if (draw_manager.query_click(active_unit->spr_id).id) {
                        // We wish to end our movement phase early
                        move_dir = DIR_None;
                        active_unit->moves_remaining = 0;
                    }

                    if (input_manager.consume(K_Up))
                        move_dir = DIR_Up;
                    if (input_manager.consume(K_Down))
                        move_dir = DIR_Down;
                    if (input_manager.consume(K_Left))
                        move_dir = DIR_Left;
                    if (input_manager.consume(K_Right))
                        move_dir = DIR_Right;
                    if (input_manager.consume(K_Enter)) {
                        // We wish to end our movement phase early
                        move_dir = DIR_None;
                        active_unit->moves_remaining = 0;
                    }
                } else {
                    // TODO: AI movement - for now just stick with the random choice
                    move_dir = ai_decide_move_direction();

                    if (move_dir == DIR_None) {
                        // DIR_None from AI function indicates we can't move
                        active_unit->moves_remaining = 0;
                    }
                }

                if (move_dir != DIR_None) {
                    active_unit->do_move(move_dir);
                    unit_moving = true;
                    move_interp = 0;
                    // TODO: move SFX
                }
            }
            break;
        case LB_Fire:
            if (shot_interp > 0) {
                shot_interp -= delta * active_unit->fire_rate;
                if (shot_interp < 0) {
                    shot_interp = 0;
                }
                break;
            }

            if (active_unit->shots_remaining <= 0) {
                active_unit->turn_taken = true;
                shot_interp = 0;
                stage = LB_CalcDamage;
            } else {
                if (!target_unit) {
                    if (human_turn) {
                        if (draw_manager.query_click(active_unit->spr_id).id) {
                            // We are choosing not to fire
                            active_unit->shots_remaining = 0;
                            break;
                        }
                        for (int i = 0; i < n_units; ++i) {
                            if (draw_manager.query_click(units[i].spr_id).id) {
                                if (check_viable_target(&units[i])) {
                                    target_unit = &units[i];
                                    break;
                                }
                            }
                        }
                    } else {
                        if (!target_unit) {
                            set_target_unit();
                        }
                    }
                }

                if (target_unit) {
                    active_unit->shots_remaining--;
                    shot_interp = 1;
                    // TODO: SFX
                } else {
                    if (!human_turn || !check_viable_targets()) {
                        // No valid targets - give up
                        active_unit->shots_remaining = 0;
                    }
                }
            }
            break;
        case LB_CalcDamage:
            damage_to_apply = 0;
            if (target_unit) {
                int power = active_unit->fire_power;
                bool cover = is_in_cover(target_unit);
                bool alienhit = false;
                // CPU units except rebels get a firepower bonus
                if (!human_turn && active_unit->type != UNIT_Rebel) {
                    ++power;
                }
                if (cover) {
                    power -= 3;
                }
                // TODO: Officer power bonuses
                power = clamp(power, 0, 4);
                if (active_unit->is_alien) {
                    if (cover) {
                        power = 1;
                    } else {
                        alienhit = true;
                    }
                }
                for (int i = 0; i < active_unit->hp; ++i) {
                    // Aliens shooting on units not in cover always hit
                    if (alienhit || onein(6 - power)) {
                        ++damage_to_apply;
                    }
                }
                damage_to_apply = min(damage_to_apply, target_unit->hp);
                L.info("Fired %d shots for %d damage - HP %d -> %d",
                    active_unit->hp, damage_to_apply,
                    target_unit->hp, max(0, target_unit->hp - damage_to_apply));
            }
            stage = LB_Damage;
            break;
        case LB_Damage:
            if (!target_unit) {
                stage = LB_CheckWon;
                break;
            }

            if (exp_interp > 0) {
                exp_interp -= delta * EXP_RATE;
                if (exp_interp < 0) {
                    exp_interp = 0;
                    --damage_to_apply;
                    target_unit->hp = max(0, target_unit->hp - 1);
                }
                break;
            } else if (damage_to_apply > 0) {
                exp_interp = 1;
                // TODO: Explosion SFX
                break;
            }

            // Ensure that dead units are at the bottom of the draw stack
            // (Move live ones back to the top)
            if (target_unit->hp <= 0) {
                for (int i = 0; i < BATTLE_UNITS_MAX; ++i) {
                    if (units[i].hp > 0) {
                        draw_manager.refresh_sprite_id(units[i].spr_id);
                    }
                }
                // But leave the cursor and explosion on top
                draw_manager.refresh_sprite_id(id(ID::CURSOR));
                draw_manager.refresh_sprite_id(id(ID::EXPLOSION));
            }

            // If the lunar base control is destroyed, destroy all lunar base guns
            if (target_unit->type == UNIT_LBCtl && target_unit->hp <= 0) {
                for (int i = 0; i < n_units; ++i) {
                    if (units[i].type == UNIT_LBGun) {
                        units[i].hp = 0;
                    }
                }
            }

            stage = LB_CheckWon;
            break;
        case LB_CheckWon:
            {
                int def_units = 0;
                int agg_units = 0;
                for (int i = 0; i < n_units; ++i) {
                    if (units[i].hp > 0) {
                        if (units[i].defending) {
                            def_units++;
                        } else {
                            agg_units++;
                        }
                    }
                }

                if (agg_units == 0 || def_units == 0) {
                    rpt.aggressor_won = def_units == 0;
                    // TODO: Populate rest of report
                    stage = LB_Won;
                } else {
                    stage = LB_SelectUnit;
                }
            }
            break;
        case LB_Won:
            L.debug("BATTLE WINNER: %s", rpt.aggressor_won ? "AGG" : "DEF");
            // TODO: Display result, music etc until click
            ephstate.set_ephemeral_state(EPH_LunarBattleReport);
            return ephstate.get_appropriate_mode();
    }

    update_panel();
    draw_units();
    update_cursor();

    update_arrows();

    if (stage == LB_Fire) {
        fire_time += delta;
    }

    return ExodusMode::MODE_None;
}

void LunarBattle::place_cover() {
    n_cover = RND(COVER_MAX); // 1 - 10 cover obstacles
    for (int i = 0; i < n_cover; ++i) {
        cover[i] = Cover(rand() % BG_WIDTH, rand() % BG_HEIGHT, (bool)(rand() % 2));
    }
}

void LunarBattle::place_units() {
    Planet *p = exostate.get_active_planet();

    // From PROCb_ground
    if (p->has_lunar_base()) {
        place_unit(BattleUnit(UNIT_LBCtl).init(14, 5));
        place_unit(BattleUnit(UNIT_LBGun).init(13, 5));
        place_unit(BattleUnit(UNIT_LBGun).init(13, 4));
        place_unit(BattleUnit(UNIT_LBGun).init(13, 6));
        place_unit(BattleUnit(UNIT_LBGun).init(12, 5));
    }

    // TODO: Manual unit positioning
    place_side_units(true);
    place_side_units(false);
}

// PROCb_esetup
void LunarBattle::place_side_units(bool def) {
    LunarBattleParams &b = ephstate.lunar_battle;
    int inf = b.aggressor_inf;
    int gli = b.aggressor_gli;
    int art = b.aggressor_art;
    int stack = b.aggressor_group_size;

    if (def) {
       inf = b.defender_inf;
       gli = b.defender_gli;
       art = b.defender_art;
       stack = b.defender_group_size;
    }

    if (stack < 2) {
        L.fatal("Stack size < 2: %d", stack);
    }

    stack = max(stack, 2);

    int stacks_inf = inf / stack;
    if (inf % stack) stacks_inf++;
    int stacks_gli = gli / stack;
    if (gli % stack) stacks_gli++;
    int stacks_art = art / stack;
    if (art % stack) stacks_art++;

    // Artillery placement
    if (stacks_art > 0) {
        BattleUnit u = (!def && b.aggressor_type == AGG_Aliens) ? UNIT_AArt : UNIT_Art;
        if (stacks_art >= 4) {
            place_unit(BattleUnit(u).init(def ?  8 : 6,  0, stack, def));
            place_unit(BattleUnit(u).init(def ?  8 : 6, 10, stack, def));
            // SUGGEST: These don't move - should we be placing them at the map edges...?
            place_unit(BattleUnit(u).init(def ? 15 : 0,  4, stack, def));
            art -= 3 * stack;
            while (art > 0) {
                int x = rand() % 7;
                int y = rand() % 11;
                if (def) {
                    x = (rand() % 8) + 8;
                }
                if (unit_at(x, y)) {
                    continue;
                }
                // TODO: Orig also checks gr%() (cover, mines, teleporters) here,
                // but not in the previous placements.
                place_unit(BattleUnit(u).init(x, y, min(stack, art), def));
                art -= min(stack, art);
            }
        } else {
            place_unit(BattleUnit(u).init(def ? 15 : 0, 4, stack, def));
            art -= min(stack, art);

            if (art > 0) {
                place_unit(BattleUnit(u).init(def ? 8 : 6, 1, min(stack, art), def));
                art -= min(stack, art);
            }

            if (art > 0) {
                place_unit(BattleUnit(u).init(def ? 8 : 6, 9, min(stack, art), def));
                art -= min(stack, art);
            }

            // Should not happen - we calculated 3 stacks would satisfy demand
            if (art > 0) {
                L.fatal("Error calculating artillery stacks");
            }
        }
    }

    if (false) {
        // TODO: We use a particular placement strategy 2/3 of the time...
    } else {
        BattleUnit u_inf = UNIT_Inf;
        if (!def) {
            if (b.aggressor_type == AGG_Aliens) u_inf = UNIT_AInf;
            if (b.aggressor_type == AGG_Rebels) u_inf = UNIT_Rebel;
        }

        BattleUnit u_gli = UNIT_Gli;

        while (inf > 0) {
            int x = rand() % 7;
            int y = rand() % 11;
            if (def) {
                x = (rand() % 8) + 8;
            }
            if (unit_at(x, y)) {
                continue;
            }
            // TODO: Orig also checks gr%() (cover, mines, teleporters) here,
            // but not in the previous placements.
            place_unit(BattleUnit(u_inf).init(x, y, min(stack, inf), def));
            inf -= min(stack, inf);
        }

        while (gli > 0) {
            int x = rand() % 7;
            int y = rand() % 11;
            if (def) {
                x = (rand() % 8) + 8;
            }
            if (unit_at(x, y)) {
                continue;
            }
            // TODO: Orig also checks gr%() (cover, mines, teleporters) here,
            // but not in the previous placements.
            place_unit(BattleUnit(u_gli).init(x, y, min(stack, gli), def));
            gli -= min(stack, gli);
        }
    }

    // TODO: Mine placement (orig seems to be defenders only)
    // TODO: Clear up ground (cover etc) with a unit on it
}

void LunarBattle::place_unit(BattleUnit u) {
    L.debug("POSITION UNIT %d %s %d AT (%d, %d)",
        n_units, u.defending ? "D" : "A", u.type, u.x, u.y);
    units[n_units++] = u;
}

// Doesn't consider dead units as 'existing'
BattleUnit* LunarBattle::unit_at(int x, int y) {
    for (int i = 0; i < n_units; ++i) {
        if (units[i].x == x && units[i].y == y && units[i].hp > 0) {
            return &units[i];
        }
    }

    return nullptr;
}

void LunarBattle::draw_units() {
    for (int pass = 0; pass < 2; ++pass) {
        bool draw_dead = pass == 0;
        for (int i = 0; i < n_units; ++i) {
            if (draw_dead && units[i].hp > 0)
                continue;
            if (!draw_dead && units[i].hp <= 0)
                continue;

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

            if (&units[i] == active_unit) {
                if (shot_interp > 0.5f) {
                    spr = units[i].fire;
                }
            }

            if (units[i].hp <= 0) {
                spr = units[i].dead;
            }

            if (units[i].defending) {
                draw_manager.draw(
                    units[i].spr_id,
                    spr,
                    {draw_x + BLK_SZ, draw_y,
                     1, 0, 1, 1});
            } else {
                draw_manager.draw(
                    units[i].spr_id,
                    spr,
                    {draw_x, draw_y,
                     0, 0, 1, 1});
            }

            if (exp_interp > 0 && target_unit == &units[i]) {
                draw_manager.draw(
                    id(ID::EXPLOSION),
                    anim_explode.interp(1 - exp_interp),
                    {draw_x + BLK_SZ/2, draw_y + BLK_SZ/2,
                     0.5, 0.5, 1, 1});
            }

            if (human_turn && stage == LB_Fire && active_unit == &units[i]) {
                bool draw_highlight = false;
                if (!target_unit && (fire_time > 1 || fmod(fire_time, 0.2) < 0.1)) {
                    if (check_viable_targets()) {
                        draw_highlight = true;
                    }
                }
                if (draw_highlight) {
                    draw_manager.draw(
                        id(ID::HIGHLIGHT),
                        IMG_CURSOR_BATTLE1,
                        {draw_x, draw_y,
                         0, 0, 1, 1});
                } else {
                    draw_manager.draw(
                        id(ID::HIGHLIGHT),
                        nullptr);
                }
            }
        }
    }
}

void LunarBattle::update_cursor() {
    if (human_turn && unit_moving) {
        cursor_x = -1;
        cursor_y = -1;
        return;
    }

    SpriteClick mouseover = draw_manager.query_mouseover(id(ID::BG));

    cursor_prev_x = cursor_x;
    cursor_prev_y = cursor_y;

    if (mouseover.id) {
        cursor_x = min((int)(mouseover.x * BG_WIDTH), BG_WIDTH - 1);
        cursor_y = min((int)(mouseover.y * BG_HEIGHT), BG_HEIGHT - 1);;
    } else {
        cursor_x = -1;
        cursor_y = -1;
    }

    bool over_target = false;
    bool over_aiming = false;

    if (stage == LB_Fire) {
        over_target = check_viable_target(unit_at(cursor_x, cursor_y));
        over_aiming = !target_unit && unit_at(cursor_x, cursor_y) == active_unit;
    }

    if (over_aiming) {
        return;
    }

    Planet *p = exostate.get_active_planet();
    bool ice = p->get_moon_class() == MOON_Ice;

    if (cursor_x >= 0 && cursor_y >= 0) {
        draw_manager.draw(
            id(ID::CURSOR),
            (over_target || ice) ? IMG_CURSOR_BATTLE1 : IMG_CURSOR_BATTLE0,
            {SURF_X + BLK_SZ * cursor_x,
             SURF_Y + BLK_SZ * cursor_y,
             0, 0, 1, 1});
    }
}

void LunarBattle::update_panel() {
    // TODO: Determine appropriate mode
    LBPanelMode target_mode = LBPM_Battle;
    if (panel_mode != target_mode) {
        draw_manager.fill({0, SURF_Y - 1, RES_X, 1}, {0, 0, 0});
        draw_manager.fill({0, 0, RES_X, SURF_Y - 1}, COL_BORDERS);
        if (target_mode == LBPM_Setup) {
            for (int i = 0; i < 4; ++i) {
                draw_manager.fill({8 + i*60, 16, 42, 42}, {0, 0, 0});
            }
            draw_manager.fill_pattern({400, 16, 100, 42});
        }
        if (target_mode == LBPM_Battle) {
            Planet *p = exostate.get_active_planet();

            draw_manager.fill_pattern({  8, 12, 160, 50});

            draw_manager.draw_text("The battle of", Justify::Left, 12, 18, COL_TEXT);
            draw_manager.draw_text(p->get_name(), Justify::Left, 12, 36, COL_TEXT);

            draw_manager.fill({175, 15, 44, 44}, {0, 0, 0});

            DrawArea a = {0, 0, 40, 40};
            draw_manager.set_source_region(id(ID::PANEL_UNIT_BG), &a);
            draw_manager.draw(
                id(ID::PANEL_UNIT_BG),
                p->moon_sprites()->bg,
                {177, 17, 0, 0, 1, 1});

            draw_manager.fill_pattern({226, 12, 200, 50});

            draw_manager.draw(
                id(ID::BTN_INFO),
                IMG_GF4_HMENU1,
                {442, 8, 0, 0, 1, 1});
            draw_manager.draw(
                id(ID::BTN_SPEED),
                IMG_GF4_HMENU5,
                {442, 38, 0, 0, 1, 1});
            draw_manager.draw(
                id(ID::BTN_TALK),
                IMG_GF4_HMENU2,
                {536, 8, 0, 0, 1, 1});
            draw_manager.draw(
                id(ID::BTN_QUIT),
                IMG_GF4_HMENU4,
                {536, 38, 0, 0, 1, 1});
        }
        draw_manager.save_background({0, 0, RES_X, SURF_Y - 1});
    }
    // TODO: Call update_panel_setup() during setup
    update_panel_battle();
    panel_mode = target_mode;
}

// The panel as drawn during unit placement
void LunarBattle::update_panel_setup() {
}

// The panel as drawn during battle
void LunarBattle::update_panel_battle() {
    const char* panel_spr = nullptr;
    BattleUnit *draw_unit = nullptr;

    draw_unit = unit_at(cursor_x, cursor_y);

    // Orig doesn't show cursor during walk phase, but it's
    // useful to be able to check unit HPs before moving...
    /*
    if (human_turn && stage == LB_Move) {
        draw_unit = active_unit;
    } else {
        draw_unit = unit_at(cursor_x, cursor_y);
    }
    */

    if (draw_unit) {
        panel_spr = draw_unit->idle;
    }

    if (draw_unit != panel_unit) {
        const char* text = STR_Ground;
        RGB text_col = {0, 0xFF, 0};  // TODO: Check exact colour
        panel_unit = draw_unit;
        // Clears over drawn health info etc
        draw_manager.fill_pattern({226, 12, 200, 50});
        if (draw_unit) {
            text = draw_unit->name;
            // Check if the unit is on our side
            if (human_turn && active_unit->defending == draw_unit->defending) {
                text_col = {0, 0, 0xFF};  // TODO: Check exact colour
                for (int i = 0; i < draw_unit->hp; ++i) {
                    draw_manager.draw(
                        IMG_GF4_SBR,
                        {232 + 10*i, 35, 0, 0, 1, 1});
                }
            }

            // TODO: Conditional based on cover
            if (is_in_cover(draw_unit)) {
                draw_manager.draw(
                    IMG_GF4_COVER,
                    {232, 50, 0, 0, 1, 1});
            }
        }
        draw_manager.draw_text(text, Justify::Left, 232, 13, text_col);
    }

    draw_manager.draw(id(ID::PANEL_UNIT), panel_spr, {177, 17, 0, 0, 1, 1});
}

// 4 bits - UDLR
char LunarBattle::get_valid_move_directions() {
    char dirs = 0xF;
    BattleUnit *u = active_unit;
    if (unit_at(u->x, u->y - 1)) dirs &= ~0x8;
    if (unit_at(u->x, u->y + 1)) dirs &= ~0x4;
    if (unit_at(u->x - 1, u->y)) dirs &= ~0x2;
    if (unit_at(u->x + 1, u->y)) dirs &= ~0x1;
    if (u->y <= 0)               dirs &= ~0x8;
    if (u->y + 1 >= BG_HEIGHT)   dirs &= ~0x4;
    if (u->x <= 0)               dirs &= ~0x2;
    if (u->x + 1 >= BG_WIDTH)    dirs &= ~0x1;
    return dirs;
}

Direction LunarBattle::get_random_move_direction() {
    int v = get_valid_move_directions();

    int n_valid = (v & 1) + ((v >> 1) & 1) + ((v >> 2) & 1) + ((v >> 3) & 1);
    if (n_valid == 0) {
        return DIR_None;
    }

    Direction d = DIR_Right;
    int idx = rand() % n_valid;
    while (v) {
        if (v & 1) {
            if (idx == 0) {
                return d;
            } else {
                --idx;
            }
        }
        v >>= 1;
        d = (Direction)((int)d - 1);
    }

    return DIR_None;
}

bool LunarBattle::ai_can_move_to(BattleUnit *u, int x, int y) {
    if (!u) {
        return false;
    }

    if (x < 0 || x >= BG_WIDTH) {
        return false;
    }

    if (y < 0 || y >= BG_HEIGHT) {
        return false;
    }

    if (unit_at(x, y)) {
        return false;
    }

    // TODO: If it's a teleporter, return false
    // TODO: If it's a mine, return false if we're defending

    return true;
}

Direction LunarBattle::ai_decide_move_direction() {
    BattleUnit *u = active_unit;

    if (is_in_cover(u) && onein(2)) {
        // 50% chance that units in cover will stay there
        return DIR_None;
    }

    Direction dir = u->defending ? DIR_Left : DIR_Right;

    bool move_up = false;
    bool move_down = false;

    if (   (dir == DIR_Right && u->x < BG_WIDTH - 1)
        || (dir == DIR_Left  && u->x > 0)) {
        int h_off = dir == DIR_Right ? 1 : -1;
        // If we can't move in the desired horizontal direction
        if (!ai_can_move_to(u, u->x + h_off, u->y)) {
            bool can_move_up   = (u->y >             0) && (ai_can_move_to(u, u->x, u->y - 1));
            bool can_move_down = (u->y < BG_HEIGHT - 1) && (ai_can_move_to(u, u->x, u->y + 1));
            if (!(can_move_up || can_move_down)) {
                // Don't move at all (although backwards might be possible)
                return DIR_None;
            }
            if (can_move_down && !can_move_up) {
                move_down = true;
            }

            if (!can_move_down && can_move_up) {
                move_up = true;
            }

            if (can_move_up && can_move_down) {
                if (onein(2)) {
                    move_up = true;
                } else {
                    move_down = true;
                }
            }
        }
    }

    bool move = true;

    // The column 'behind' us - or our column if we're at the edge
    int col_behind = 0;
    if (dir == DIR_Right) {
        col_behind = u->x > 0 ? u->x-1 : u->x;
    } else {
        col_behind = (u->x < BG_WIDTH - 1) ? u->x+1 : u->x;
    }

    // If there are enemy units in the column 'behind' us, unset the move flag
    for (int j = 0; j < BG_WIDTH; ++j) {
        BattleUnit *other = unit_at(col_behind, j);
        if (other && other->defending != u->defending) {
            move = false;
            break;
        }
    }

    if (!move) {
        for (int j = 0; j < BG_WIDTH; ++j) {
            BattleUnit *other = unit_at(col_behind, j);
            if (other && other->defending != u->defending) {
                if (other->y > u->y) {
                    if (ai_can_move_to(u, u->x, u->y + 1)) {
                        move_down = true;
                    }
                } else {
                    if (ai_can_move_to(u, u->x, u->y - 1)) {
                        move_up = true;
                    }
                }
            }
        }

        if (move_up || move_down) {
            move = true;
        }
    }

    if (move) {
        if (!(move_up || move_down)) {
            if ((dir == DIR_Right) && (u->x >= BG_WIDTH - 1 || !ai_can_move_to(u, u->x + 1, u->y))) {
                return DIR_None;
            }
            if ((dir == DIR_Left) && (u->x <= 0 || !ai_can_move_to(u, u->x - 1, u->y))) {
                return DIR_None;
            }
            if (move_up && (u->y <= 0 || !ai_can_move_to(u, u->x, u->y - 1))) {
                return DIR_None;

            }
            if (move_down && (u->y >= BG_HEIGHT - 1 || !ai_can_move_to(u, u->x, u->y + 1))) {
                return DIR_None;
            }
        }

        if (!(move_up || move_down)) {
            return dir;
        }

        if (move_up) {
            return DIR_Up;
        }

        if (move_down) {
            return DIR_Down;
        }
    }

    return DIR_None;
}

bool LunarBattle::set_target_unit() {
    target_unit = nullptr;

    bool killbase = false;
    BattleUnit *lbctl = nullptr;

    if (!defender_turn) {
        int aat = 0;
        int adf = 0;
        int inc = 0;
        for (int i = 0; i < n_units; ++i) {
            if (units[i].hp > 0) {
                BattleUnitType t = units[i].type;

                if (t == UNIT_LBCtl) {
                    lbctl = &units[i];
                }

                inc = 0;
                if (t == UNIT_Inf || t == UNIT_Rebel || t == UNIT_AInf) {
                    inc = units[i].hp;
                }
                if (t == UNIT_Gli) {
                    inc = units[i].hp * 2;
                }
                if (t == UNIT_Art || t == UNIT_AArt) {
                    inc = units[i].hp * 3;
                }
                if (t == UNIT_LBGun) {
                    inc = units[i].hp * 4;
                }

                if (units[i].defending) {
                    adf += inc;
                } else {
                    aat += inc;
                }
            }
        }

        if (lbctl) {
            killbase = ((float)aat * 1.7f < (float)adf);
            L.debug("WILL%s kill base if possible", killbase ? "" : " NOT");
        }
    }

    // 1. If we're the aggressor, consider targetting the lunar control
    if (killbase && lbctl && in_range(lbctl->x, lbctl->y)) {
        target_unit = lbctl;
        return true;
    }

    int rng_start_x = 0;
    int rng_end_x   = 0;
    int rng_start_y = 0;
    int rng_end_y   = 0;
    int rng_step    = 0;

    int range = infinite_range() ? BG_WIDTH : active_unit->fire_range;
    BattleUnit *au = active_unit;
    if (active_unit->defending) {
        rng_start_x = au->x + range;
        rng_end_x   = au->can_shoot_behind ? (au->x - range - 1) : (au->x - 1);
        rng_start_y = au->y + range;
        rng_end_y   = au->y - range - 1;
        rng_step = -1;
    } else {
        rng_start_x = au->can_shoot_behind ? (au->x - range) : au->x;
        rng_end_x   = au->x + range + 1;
        rng_start_y = au->y - range;
        rng_end_y   = au->y + range + 1;
         // TODO: Is y inverted in orig? If so, we to invert y (and need rng_y_step)
        rng_step = 1;
    }

    // 2. If we're the aggressor, target LBGuns if possible
    if (!defender_turn) {
        for (int i = rng_start_x; i != rng_end_x; i += rng_step) {
            for (int j = rng_start_y; j != rng_end_y; j += rng_step) {
                BattleUnit *u = unit_at(i, j);
                if (u && u->type == UNIT_LBGun) {
                    target_unit = u;
                    return true;
                }
            }
        }
    }

    // 3. Pick the most advanced unit that we find
    for (int i = rng_start_x; i != rng_end_x; i += rng_step) {
        for (int j = rng_start_y; j != rng_end_y; j += rng_step) {
            BattleUnit *u = unit_at(i, j);
            if (!u) {
                continue;
            }
            bool enemy = defender_turn ^ u->defending;
            if (enemy && u->hp > 0 && in_range(u->x, u->y)) {
                target_unit = u;
                return true;
            }
        }
    }

    return false;
}

bool LunarBattle::check_viable_targets() {
    for (int i = 0; i < n_units; ++i) {
        if (check_viable_target(&units[i])) {
            return true;
        }
    }
    return false;
}

bool LunarBattle::check_viable_target(BattleUnit* u) {
    if (u && active_unit != u && in_range(u->x, u->y)) {
        if (u->defending != active_unit->defending) {
            if (u->hp > 0) {
                return true;
            }
        }
    }
    return false;
}

bool LunarBattle::infinite_range() {
    for (int i = 0; i < n_units; ++i) {
        if (units[i].hp > 0) {
            if (units[i].type == UNIT_Inf)   return false;
            if (units[i].type == UNIT_Gli)   return false;
            if (units[i].type == UNIT_Rebel) return false;
            if (units[i].type == UNIT_AInf)  return false;
        }
    }

    return true;
}

bool LunarBattle::in_range(int x, int y) {
    if (infinite_range()) {
        return true;
    }

    int x_min = active_unit->x - active_unit->fire_range;
    int x_max = active_unit->x + active_unit->fire_range;

    if (!active_unit->can_shoot_behind) {
        if (active_unit->defending) {
            x_max = active_unit->x;
        } else {
            x_min = active_unit->x;
        }
    }

    if (x < x_min) return false;
    if (x > x_max) return false;
    if (y < active_unit->y - active_unit->fire_range) return false;
    if (y > active_unit->y + active_unit->fire_range) return false;
    return true;
}

void LunarBattle::update_arrows() {
    bool should_draw = true;
    if (!human_turn) should_draw = false;
    if (stage != LB_Move) should_draw = false;
    if (unit_moving) should_draw = false;
    if (active_unit->move == 0) should_draw = false;
    if (active_unit->moves_remaining == 0) should_draw = false;

    if (should_draw) {
        int base_x = SURF_X + BLK_SZ * active_unit->x + BLK_SZ/2;
        int base_y = SURF_Y + BLK_SZ * active_unit->y + BLK_SZ/2;

        char valid_dirs = get_valid_move_directions();

        // Up
        if (valid_dirs & 0x8) {
            draw_manager.draw(
                id(ID::ARROW_UP),
                IMG_GF4_MA1,
                {base_x, base_y - BLK_SZ, 0.5, 0.5, 1, 1});
        }

        // Down
        if (valid_dirs & 0x4) {
            draw_manager.draw(
                id(ID::ARROW_DOWN),
                IMG_GF4_MA2,
                {base_x, base_y + BLK_SZ, 0.5, 0.5, 1, 1});
        }

        // Left
        if (valid_dirs & 0x2) {
            draw_manager.draw(
                id(ID::ARROW_LEFT),
                IMG_GF4_MA3,
                {base_x - BLK_SZ, base_y, 0.5, 0.5, 1, 1});
        }

        // Right
        if (valid_dirs & 0x1) {
            draw_manager.draw(
                id(ID::ARROW_RIGHT),
                IMG_GF4_MA4,
                {base_x + BLK_SZ, base_y, 0.5, 0.5, 1, 1});
        }
    } else {
        draw_manager.draw(id(ID::ARROW_UP), nullptr);
        draw_manager.draw(id(ID::ARROW_DOWN), nullptr);
        draw_manager.draw(id(ID::ARROW_LEFT), nullptr);
        draw_manager.draw(id(ID::ARROW_RIGHT), nullptr);
    }
}

bool viable(BattleUnit u, bool def) {
    return u.defending == def
        && u.can_act
        && !u.turn_taken
        && u.hp > 0;
}

bool LunarBattle::select_unit() {
    // Switch whose turn it is
    defender_turn = !defender_turn;

    int n_viable;

    // Are there any units for the current player?
    n_viable = 0;
    for (int i = 0; i < n_units; ++i) {
        if (viable(units[i], defender_turn)) {
            n_viable++;
        }
    }

    if (n_viable == 0) {
        L.debug("No units: %s", defender_turn ? "DEF" : "AGG");
        defender_turn = !defender_turn;

        n_viable = 0;
        for (int i = 0; i < n_units; ++i) {
            if (viable(units[i], defender_turn)) {
                n_viable++;
            }
        }

        if (n_viable == 0) {
            L.debug("Neither side has viable units");
            return false;
        }
    }

    int viable_idx = rand() % n_viable;
    for (int i = 0; i < n_units; ++i) {
        if (viable(units[i], defender_turn)) {
            if (viable_idx == 0) {
                active_unit = &units[i];
                break;
            }
            --viable_idx;
        }
    }

    active_unit->moves_remaining = active_unit->move;
    active_unit->shots_remaining = active_unit->hp;

    return true;
}

void LunarBattle::reset_round() {
    L.debug("< RESETTING ROUND >");
    for (int i = 0; i < n_units; ++i) {
        units[i].turn_taken = false;
    }
}

bool LunarBattle::is_in_cover(BattleUnit* u) {
    if (!u || !u->can_use_cover) {
        return false;
    }

    for (int i = 0; i < n_cover; ++i) {
        if (cover[i].x == u->x && cover[i].y == u->y) {
            return true;
        }
    }

    return false;
}

BattleUnit::BattleUnit(BattleUnitType _type) : type(_type) {
    name = STR_None;
    x = 0;
    y = 0;
    tgt_x = 0;
    tgt_y = 0;
    move = 0;
    fire_range = 0;
    fire_rate = 4.f;
    fire_power = 0;
    moves_remaining = 0;
    shots_remaining = 0;
    can_act = true;
    can_use_cover = false;
    is_alien = false;
    idle = nullptr;
    walk = nullptr;
    fire = nullptr;
    dead = nullptr;
    move_sfx = nullptr;
    shoot_sfx = nullptr;
    can_shoot_behind = true;
    turn_taken = false;
}

BattleUnit& BattleUnit::init(int _x, int _y) {
    x = _x;
    y = _y;

    tgt_x = x;
    tgt_y = y;

    turn_taken = false;

    switch (type) {
        case UNIT_Inf:
            name = STR_Inf;
            move = 3;
            fire_range = 3;
            fire_power = 1;
            can_use_cover = true;
            // TODO: SFX
            if (defending) {
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
            break;
        case UNIT_Gli:
            name = STR_Gli;
            move = 4;
            fire_range = 4;
            fire_power = 2;
            // TODO: SFX
            // TODO: Glider crashing sprite
            if (defending) {
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
            break;
        case UNIT_Art:
            name = STR_Art;
            move = 0;
            fire_range = 7;
            fire_power = 3;
            can_shoot_behind = false;
            if (defending) {
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
            break;
        case UNIT_LBGun:
            name = STR_LBGun;
            move = 0;
            fire_range = 100;
            fire_power = 4;
            hp = 6;
            defending = true;
            idle = IMG_GF4_21;
            walk = IMG_GF4_21;
            fire = IMG_GF4_24;
            dead = IMG_GF4_23;
            break;
        case UNIT_LBCtl:
            name = STR_LBCtl;
            move = 0;
            fire_range = 0;
            hp = 10;
            defending = true;
            can_act = false;
            idle = IMG_GF4_20;
            walk = IMG_GF4_20;
            fire = IMG_GF4_20;
            dead = IMG_GF4_22;
            break;
        case UNIT_Rebel:
            name = STR_Inf;
            // TODO: Check these
            move = 3;
            fire_range = 3;
            fire_power = 1;
            defending = false;
            can_use_cover = true;
            idle = IMG_RF1_1;
            walk = IMG_RF1_1_2;
            fire = IMG_RF1_13;
            dead = IMG_RF1_7;
            break;
        case UNIT_AInf:
            name = STR_Inf;
            move = 3;
            fire_range = 3;
            fire_power = 1;
            is_alien = true;
            defending = false;
            can_use_cover = true;
            // TODO: Alt aliens
            if (true) {
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
            break;
        case UNIT_AArt:
            name = STR_Art;
            move = 0;
            fire_range = 7;
            fire_power = 3;
            is_alien = true;
            can_shoot_behind = false;
            defending = false;
            // TODO: Alt aliens
            if (true) {
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
            break;
    }

    return *this;
}

BattleUnit& BattleUnit::init(int _x, int _y, int _hp) {
    hp = _hp;
    return init(_x, _y);
}

BattleUnit& BattleUnit::init(int _x, int _y, int _hp, bool _def) {
    defending = _def;
    return init(_x, _y, _hp);
}

// No validation here - LunarBattle is responsible for correct puppeteering
void BattleUnit::do_move(Direction d) {
    if (d == DIR_Up)    tgt_y--;
    if (d == DIR_Down)  tgt_y++;
    if (d == DIR_Left)  tgt_x--;
    if (d == DIR_Right) tgt_x++;
}
