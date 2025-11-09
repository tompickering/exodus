#include "lunar_battle.h"

#include <cmath>

#include "anim.h"
#include "assetpaths.h"
#include "exodus_features.h"

#include "util/value.h"

static const int SURF_X =  0;
static const int SURF_Y = 72;
static const int BLK_SZ = 40;
static const float MOVE_RATE = 1.5f;
static const float EXP_RATE = 2.f;
static const int BG_WIDTH = 16;
static const int BG_HEIGHT = 11;

static const int PANEL_W = 328;
static const int PANEL_H = 120;
static const int PANEL_X = RES_X/2 - (PANEL_W/2);
static const int PANEL_Y = 150;

const char* STR_None   = "";
const char* STR_Ground = "Ground";
const char* STR_Inf    = "Infantry";
const char* STR_Gli    = "Gliders";
const char* STR_Art    = "Artillery";
const char* STR_LBGun  = "Base, Gun";
const char* STR_LBCtl  = "Base, Control";
const char* STR_Tele   = "Rescue Beam";
const char* STR_Mine   = "Mine";

static const int LUNAR_BASE_GUN_HP =  6;
static const int LUNAR_BASE_CTL_HP = 10;

static const float DYING_TIME = .1f;

static const float FAST_SHOT_MULTIPLIER = 4.f;

enum ID {
    TOP_PANEL,
    TOP_PANEL_MOVES,
    TOP_PANEL_RANGE,
    TOP_PANEL_UNITNAME,
    TOP_PANEL_COVER,
    TOP_PANEL_PROMOTED,
    PANEL,
    PANEL_PATTERN,
    PLACEMENT_SELECTED,
    PLACEMENT_SELECTED_NAME,
    PLACEMENT_SELECTED_DESC,
    BG,
    BTN_INFO,
    BTN_SPEED,
    BTN_TALK,
    BTN_QUIT,
    BTN_MAN,
    PANEL_UNIT_BG,
    PANEL_UNIT,
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    CURSOR,
    EXPLOSION,
    HIGHLIGHT,
    TELE0,
    TELE1,
    TELE2,
    TELELIGHTS,
    TELESTAR,
    SURRENDER_YESNO,
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
    aggressor = nullptr;
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
    aggressor_can_discover_mines = false;
    panel_unit = nullptr;
    panel_hp = -1;
    panel_moves = -1;
    manual_placement = false;
    placement_def = true;
    placement_item = 0;
    to_place_inf = 0;
    to_place_gli = 0;
    to_place_art = 0;
    to_place_msc = 0;
    agg_officer = OFFQ_Poor;
    def_officer = OFFQ_Poor;
}

void LunarBattle::enter() {
    ModeBase::enter(ID::END);
    stage = LB_Move;
    panel_mode = LBPM_None;

    LunarBattleParams &b = ephstate.lunar_battle;

    agg_officer = b.aggressor_officer;
    def_officer = b.defender_officer;

    Planet *p = exostate().get_active_planet();

    aggressor = nullptr;
    defender = nullptr;

    if (b.aggressor_type == AGG_Player) {
        aggressor = exostate().get_player(b.aggressor_idx);
    }

    if (p->is_owned()) {
        defender = exostate().get_player(p->get_owner());
    }

    aggressor_can_discover_mines = false;

    if (FEATURE(EF_LUNAR_BATTLE_INF_DISCOVER_MINES)) {
        if (aggressor) {
            if (agg_officer != OFFQ_Poor) {
                Race r = aggressor->get_race();
                aggressor_can_discover_mines = (r == RACE_Human || r == RACE_Urkash);
            }
        }
    }

    LunarBattleReport &rpt = ephstate.lunar_battle_report;
    rpt.clear();
    rpt.defender_idx = p->get_owner();
    rpt.agg_type     = b.aggressor_type;
    rpt.agg_odds     = b.aggressor_odds;
    rpt.agg_init.inf = b.aggressor_inf;
    rpt.agg_init.gli = b.aggressor_gli;
    rpt.agg_init.art = b.aggressor_art;
    rpt.def_init.inf = b.defender_inf;
    rpt.def_init.gli = b.defender_gli;
    rpt.def_init.art = b.defender_art;
    rpt.agg_surf.inf = b.aggressor_inf;
    rpt.agg_surf.gli = b.aggressor_gli;
    rpt.agg_surf.art = b.aggressor_art;
    rpt.def_surf.inf = b.defender_inf;
    rpt.def_surf.gli = b.defender_gli;
    rpt.def_surf.art = b.defender_art;
    if (p->has_lunar_base()) {
        rpt.def_init.base = LUNAR_BASE_GUN_HP*4;
        rpt.def_surf.base = LUNAR_BASE_GUN_HP*4;
    }

    if (b.aggressor_gli == 0 && b.aggressor_art == 0) {
        if (b.aggressor_inf <= b.aggressor_group_size) {
            rpt.agg_one_inf_stack = true;
        }
    }

    n_units = 0;
    unit_moving = false;
    move_interp = 0;
    shot_interp = 0;
    exp_interp = 0;
    fire_time = 0;
    n_mines = 0;
    n_tele = 0;

    cursor_x = -1;
    cursor_y = -1;
    cursor_prev_x = -1;
    cursor_prev_y = -1;

    mine_timer = 0;
    mine_beeps = 0;
    mine_damage = false;
    active_mine = nullptr;

    tele_timer = 0;
    active_tele = nullptr;
    tele_done = false;

    BattleUnit::use_alt_aliens = (bool)(rand() % 2);

    human_turn = true;

    bool defending = !(aggressor && aggressor->is_human());

    manual_placement = false;
    placement_item = 0;
    to_place_inf = 0;
    to_place_gli = 0;
    to_place_art = 0;
    to_place_msc = 0;

    comm_ctx.battle_first_comms = true;

    if (b.auto_battle) {
        stage = LB_Auto;
    } else {
        for (int i = 0; i < 8; ++i) {
            placement_ids[i] = draw_manager.new_sprite_id();
        }
        for (int i = 0; i < 20; ++i) {
            hp_ids[i] = draw_manager.new_sprite_id();
        }
        for (int i = 0; i < 88; ++i) {
            fog_ids[i] = draw_manager.new_sprite_id();
        }

        // TODO_MP: If we never support true multiplayer battles, coalesce into one flag
        manual_placement = defending ? b.defender_manual_placement
                                     : b.aggressor_manual_placement;

        place_cover();

        // From PROCb_ground - set up lunar base for defender
        if (p->has_lunar_base()) {
            place_unit(BattleUnit(UNIT_LBCtl).init(14, 5));
            place_unit(BattleUnit(UNIT_LBGun).init(13, 5));
            place_unit(BattleUnit(UNIT_LBGun).init(13, 4));
            place_unit(BattleUnit(UNIT_LBGun).init(13, 6));
            place_unit(BattleUnit(UNIT_LBGun).init(12, 5));
        }

        // Place the units for the non-'acting' side
        place_units(!defending);

        if (manual_placement) {
            placement_def = defending;
            if (placement_def) {
                to_place_inf = b.defender_inf;
                to_place_gli = b.defender_gli;
                to_place_art = b.defender_art;
                to_place_msc = b.defender_mines;
            } else {
                to_place_inf = b.aggressor_inf;
                to_place_gli = b.aggressor_gli;
                to_place_art = b.aggressor_art;
                to_place_msc = b.aggressor_tele;
            }
        } else {
            place_units(defending);
        }

        for (int i = 0; i < COVER_MAX; ++i) {
            cover[i].spr_id = draw_manager.new_sprite_id();
        }

        draw_ground();

        if (manual_placement) {
            for (int i = 0; i < 88; ++i) {
                draw_manager.draw(
                    fog_ids[i],
                    IMG_GF4_SHADE,
                    {SURF_X + ((i/BG_HEIGHT) + (placement_def ? 0 : 8)) * BLK_SZ,
                     SURF_Y + (i%BG_HEIGHT) * BLK_SZ,
                     0, 0, 1, 1});
            }
            stage = LB_Placement;
        } else {
            stage = LB_StartBattle;
        }
    }

    turn = 0;

    draw_manager.show_cursor(!b.auto_battle);

    // Modified at the start of combat, but best always pointing to something valid
    active_unit = &units[0];
    target_unit = nullptr;
    panel_unit = nullptr;
    panel_hp = -1;
    panel_moves = -1;

    fast = false;

    img_arrow_up = IMG_GF4_MA1;
    img_arrow_down = IMG_GF4_MA2;
    img_arrow_left = IMG_GF4_MA3;
    img_arrow_right = IMG_GF4_MA4;

    if (p->get_moon_class() == MOON_Ice) {
        img_arrow_up = IMG_DARKARROW_UP;
        img_arrow_down = IMG_DARKARROW_DOWN;
        img_arrow_left = IMG_DARKARROW_LEFT;
        img_arrow_right = IMG_DARKARROW_RIGHT;
    }
}

void LunarBattle::draw_ground() {
    Planet *p = exostate().get_active_planet();

    draw_manager.draw(
        id(ID::BG),
        p->moon_sprites()->bg,
        {SURF_X, SURF_Y, 0, 0, 1, 1});

    draw_manager.save_background({SURF_X, SURF_Y, RES_X, RES_Y});

    for (int i = 0; i < COVER_MAX; ++i) {
        if (!cover[i].exists) {
            continue;
        }
        const char *spr = p->moon_sprites()->cover0;
        if (cover[i].alt) spr = p->moon_sprites()->cover1;
        draw_manager.draw(
            cover[i].spr_id,
            spr,
            {SURF_X + cover[i].x * BLK_SZ,
             SURF_Y + cover[i].y * BLK_SZ,
             0, 0, 1, 1});
    }
}

void LunarBattle::exit() {
    comm_ensure_closed();

    LunarBattleParams &b = ephstate.lunar_battle;

    if (!b.auto_battle) {
        for (int i = 0; i < 88; ++i) {
            draw_manager.release_sprite_id(fog_ids[i]);
        }
        for (int i = 0; i < 8; ++i) {
            draw_manager.release_sprite_id(placement_ids[i]);
        }
        for (int i = 0; i < 20; ++i) {
            draw_manager.release_sprite_id(hp_ids[i]);
        }
        for (int i = 0; i < BATTLE_UNITS_MAX; ++i) {
            units[i].release_spr_id();
        }
        for (int i = 0; i < COVER_MAX; ++i) {
            draw_manager.release_sprite_id(cover[i].spr_id);
        }
    }

    ModeBase::exit();
}

ExodusMode LunarBattle::update(float delta) {
    LunarBattleParams &b = ephstate.lunar_battle;
    LunarBattleReport &rpt = ephstate.lunar_battle_report;

    Planet *p = exostate().get_active_planet();

    bool defending = !(aggressor && aggressor->is_human());

    if (defender_turn) {
        human_turn = defender->is_human();
    } else {
        human_turn = (bool)(aggressor && aggressor->is_human());
    }

    for (int i = 0; i < BATTLE_UNITS_MAX; ++i) {
        if (units[i].dying_timer > 0) {
            units[i].dying_timer -= delta;
            if (units[i].dying_timer < 0) {
                units[i].dying_timer = 0;
            }
        }
    }

    switch (stage) {
        case LB_Auto:
            {
                auto_run();
                L.debug("Auto battle: %s WON", rpt.aggressor_won ? "AGG" : "DEF");
                ephstate.set_ephemeral_state(EPH_LunarBattleReport);
                return ephstate.get_appropriate_mode();
            }
        case LB_Placement:
            {
                for (int i = 0; i < 4; ++i) {
                    if (draw_manager.query_click(placement_ids[i]).id) {
                        placement_item = i;
                    }
                }

                if (draw_manager.query_click(id(ID::BG)).id) {
                    bool do_place = cursor_x >= 0 && cursor_y >= 0;
                    if (do_place) {
                        if (placement_def) {
                            do_place = cursor_x >= 8;
                        } else {
                            do_place = cursor_x < 8;
                        }
                    }
                    if (do_place) {
                        do_place = valid_placement(cursor_x, cursor_y);
                    }
                    if (do_place) {
                        int group = placement_def ? b.defender_group_size
                                                  : b.aggressor_group_size;
                        int stack = 1;
                        switch (placement_item) {
                            case 0:
                                stack = to_place_inf < group ? to_place_inf : group;
                                if (stack > 0) {
                                    place_unit(BattleUnit(UNIT_Inf).init(
                                                   cursor_x, cursor_y,
                                                   stack,
                                                   placement_def));
                                    to_place_inf -= stack;
                                }
                                break;
                            case 1:
                                stack = to_place_gli < group ? to_place_gli : group;
                                if (stack > 0) {
                                    place_unit(BattleUnit(UNIT_Gli).init(
                                                   cursor_x, cursor_y,
                                                   stack,
                                                   placement_def));
                                    to_place_gli -= stack;
                                }
                                break;
                            case 2:
                                stack = to_place_art < group ? to_place_art : group;
                                if (stack > 0) {
                                    place_unit(BattleUnit(UNIT_Art).init(
                                                   cursor_x, cursor_y,
                                                   stack,
                                                   placement_def));
                                    to_place_art -= stack;
                                }
                                break;
                            case 3:
                                if (to_place_msc > 0) {
                                    if (placement_def) {
                                        mines[n_mines].x = cursor_x;
                                        mines[n_mines].y = cursor_y;
                                        mines[n_mines].live = true;
                                        mines[n_mines].discovered = false;
                                        mines[n_mines].spr_id = draw_manager.new_sprite_id();
                                        n_mines++;
                                    } else {
                                        tele[n_tele].x = cursor_x;
                                        tele[n_tele].y = cursor_y;
                                        tele[n_tele].spr_id = draw_manager.new_sprite_id();
                                        n_tele++;
                                    }
                                    to_place_msc--;
                                }
                                break;
                        }
                        /*
                         * At the moment, the cursor will 'hide' the unit sprite.
                         * This is because the new sprite's ID will be appended
                         * to the drawn sprite info records when it's drawn for
                         * the first time - and this will be on top of the cursor.
                         * As the cursor is drawn subsequently, it only repairs
                         * things below it, before it itself is drawn.
                         *
                         * To correct this, we need to first draw the unit, to
                         * have its ID added to the drawn info vector, and THEN
                         * shuffle the cursor to the top of the pile.
                         */
                        draw_units();
                        draw_manager.refresh_sprite_id(id(ID::CURSOR));
                        return ExodusMode::MODE_None;
                    }
                }

                int to_place = to_place_inf
                             + to_place_gli
                             + to_place_art
                             + to_place_msc;

                if (to_place == 0) {
                    draw_manager.fill(
                        id(ID::PANEL),
                        {PANEL_X - BORDER, PANEL_Y - BORDER,
                        PANEL_W + 2*BORDER, PANEL_H + 2*BORDER},
                        COL_BORDERS);
                    draw_manager.fill_pattern(
                        id(ID::PANEL_PATTERN),
                        {PANEL_X, PANEL_Y,
                        PANEL_W, PANEL_H});

                    draw_manager.draw_text(
                        "As the last battle unit",
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 4,
                        COL_TEXT);
                    draw_manager.draw_text(
                        "is positioned onto the battlefield",
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 24,
                        COL_TEXT);
                    draw_manager.draw_text(
                        "the radio disturbances stop",
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 44,
                        COL_TEXT);
                    draw_manager.draw_text(
                        "allowing a clear view of",
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 64,
                        COL_TEXT);
                    draw_manager.draw_text(
                        "the enemy's machines.",
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 84,
                        COL_TEXT);

                    stage = LB_PlacementEnd;

                    // Skip remaining drawing etc until this is closed
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case LB_PlacementEnd:
            if (draw_manager.clicked()) {
                draw_manager.draw(id(ID::PANEL_PATTERN), nullptr);
                draw_manager.draw(id(ID::PANEL), nullptr);
                stage = LB_StartBattle;
                for (int i = 0; i < 88; ++i) {
                    draw_manager.draw(fog_ids[i], nullptr);
                }
                // First draw for enemy units, so neeed to put cursor on top afterwards
                draw_units();
                draw_manager.refresh_sprite_id(id(ID::CURSOR));
                return ExodusMode::MODE_None;
            }
            // Skip remaining drawing etc until this is closed
            return ExodusMode::MODE_None;
        case LB_StartBattle:
            {
                audio_manager.fade_out(1000);
                Player *owner = nullptr;
                if (p->is_owned()) owner = exostate().get_player(p->get_owner());
                defender_turn = (bool)(owner && owner->is_human());
                reset_round();
                stage = LB_SelectUnit;
            }
            break;
        case LB_CommHuman:
            {
                CommAction action = comm_update(delta);
                switch (action) {
                    case CA_None:
                        return ExodusMode::MODE_None;
                    case CA_Abort:
                        comm_close();
                        stage = button_prev_stage;
                        comm_ctx.battle_first_comms = false;
                        break;
                    case CA_CallOffAttack:
                        comm_close();
                        rpt.aggressor_won = false;
                        rpt.aggressor_retreated = true;
                        if (b.aggressor_type == AGG_Rebels) {
                            rpt.rebel_peace = true;
                        }
                        stage = LB_Won;
                        break;
                    default:
                        L.error("Unexpected human comm action in battle: %d", (int)action);
                        comm_close();
                        stage = button_prev_stage;
                        comm_ctx.battle_first_comms = false;
                        break;
                }
            }
            break;
        case LB_CommAI:
            {
                comm_update(delta);
                CommAction action = comm_update(delta);
                switch (action) {
                    case CA_None:
                        return ExodusMode::MODE_None;
                    case CA_Abort:
                        comm_close();
                        stage = LB_SelectUnit;
                        break;
                    case CA_CallOffAttack:
                        comm_close();
                        rpt.aggressor_won = false;
                        rpt.aggressor_retreated = true;
                        if (b.aggressor_type == AGG_Rebels) {
                            rpt.rebel_peace = true;
                        }
                        stage = LB_Won;
                        break;
                    default:
                        L.error("Unexpected AI comm action in battle: %d", (int)action);
                        comm_close();
                        stage = LB_SelectUnit;
                        break;
                }
            }
            break;
        case LB_SelectUnit:
            if (select_unit()) {
                turn++;
            } else {
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
            // Handle button clicks that require a stage change
            if (human_turn) {
                stage = update_buttons();

                if (stage != LB_Move) {
                    return ExodusMode::MODE_None;
                }
            }

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

                int on_mine = -1;

                for (int i = 0; i < n_mines; ++i) {
                    if (mines[i].x == active_unit->x && mines[i].y == active_unit->y) {
                        if (mines[i].live) {
                            on_mine = i;
                            break;
                        }
                    }
                }

                if (on_mine >= 0) {
                    if (!active_unit->defending && aggressor_can_discover_mines) {
                        if (active_unit->type == UNIT_Inf) {
                            audio_manager.play_sfx(SFX_BEEP);
                            mines[on_mine].discovered = true;
                        }
                    }

                    // If we're a glider, check if we've on a mine
                    if (active_unit->type == UNIT_Gli) {
                        active_mine = &mines[on_mine];
                        stage = LB_Mine;
                        return ExodusMode::MODE_None;
                    }
                }

                // If we're an attacking unit, check if we're on a teleporter
                if (!active_unit->defending) {
                    for (int i = 0; i < n_tele; ++i) {
                        if (tele[i].x == active_unit->x && tele[i].y == active_unit->y) {
                            active_tele = &tele[i];
                            tele_timer = 0;
                            tele_done = false;
                            stage = LB_Tele;
                            return ExodusMode::MODE_None;
                        }
                    }
                }

                // Check if we've reached the far side
                update_reached_far_side(*active_unit);

                break;
            }

            if (unit_moving) {
                move_interp += delta * MOVE_RATE;
                if (move_interp > 1) move_interp = 1;
            } else {
                Direction move_dir = DIR_None;

                char valid_dirs = get_valid_move_directions();

                if (valid_dirs == 0) {
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

                    if (   (move_dir == DIR_Right && !(valid_dirs & 1))
                        || (move_dir == DIR_Left && !(valid_dirs & 2))
                        || (move_dir == DIR_Down && !(valid_dirs & 4))
                        || (move_dir == DIR_Up && !(valid_dirs & 8))) {
                        move_dir = DIR_None;
                    }
                } else {
                    if (active_unit->random_moves > 0) {
                        move_dir = ai_decide_move_direction_random();

                        if (move_dir == DIR_None) {
                            // We can't move at all - might as well abandon random moving
                            active_unit->random_moves = 0;
                        } else {
                            --active_unit->random_moves;
                        }
                    } else if (active_unit->reached_far_side && FEATURE(EF_FIX_LUNAR_BATTLE_STALEMATE)) {
                        move_dir = ai_decide_move_direction2();

                        if (move_dir == DIR_None) {
                            if (!check_viable_targets()) {
                                /*
                                 * We can't move following this strategy - and we have
                                 * no viable targets.
                                 *
                                 * Take a few random movements to 'shake' things a bit
                                 *
                                 * Makes as sure as possible we don't completely stalemate
                                */
                                move_dir = ai_decide_move_direction_random();

                                if (move_dir != DIR_None) {
                                    active_unit->random_moves = 3;
                                }
                            }
                        }
                    } else {
                        move_dir = ai_decide_move_direction();
                    }

                    if (FEATURE(EF_PREVENT_AI_BATTLE_BACKTRACK)) {
                        if (move_dir == DIR_Up && active_unit->last_move == DIR_Down) {
                            move_dir = DIR_None;
                        }

                        if (move_dir == DIR_Down && active_unit->last_move == DIR_Up) {
                            move_dir = DIR_None;
                        }

                        if (move_dir == DIR_Left && active_unit->last_move == DIR_Right) {
                            move_dir = DIR_None;
                        }

                        if (move_dir == DIR_Right && active_unit->last_move == DIR_Left) {
                            move_dir = DIR_None;
                        }
                    }

                    if (move_dir == DIR_None) {
                        // DIR_None from AI function indicates we can't move
                        active_unit->moves_remaining = 0;
                    }
                }

                if (move_dir != DIR_None) {
                    active_unit->do_move(move_dir);
                    unit_moving = true;
                    move_interp = 0;

                    if (fast) {
                        move_interp = 1;
                    }

                    int r = rand() % 3;
                    const char* sfx = active_unit->move0_sfx;
                    if (r == 1) sfx = active_unit->move1_sfx;
                    if (r == 2) sfx = active_unit->move2_sfx;
                    audio_manager.play_sfx(sfx);
                }
            }
            break;
        case LB_Mine:
            if (!active_mine) {
                L.fatal("Entered LB_Mine mode with no active mine");
            }

            if (mine_beeps < 5) {
                if (mine_timer > .05f) {
                    // Final 'beep' is silent
                    if (mine_beeps < 4) {
                        audio_manager.play_sfx(SFX_BEEP);
                    }
                    mine_timer = 0;
                    mine_beeps++;
                    return ExodusMode::MODE_None;
                }
                mine_timer += delta;
                return ExodusMode::MODE_None;
            } else {
                mine_beeps = 0;
                mine_timer = 0;
                // Mine explosion
                target_unit = active_unit;
                damage_to_apply = active_unit->hp;
                mine_damage = true;

                active_mine->live = false;
                draw_manager.draw(active_mine->spr_id, nullptr);
                active_mine = nullptr;

                stage = LB_Damage;
            }
            break;
        case LB_Tele:
            {
                if (!active_unit) {
                    L.fatal("Entered LB_Tele mode with no active unit");
                }
                if (!active_tele) {
                    L.fatal("Entered LB_Tele mode with no active tele");
                }

                if (!tele_done && tele_timer > .5f) {
                    switch (active_unit->type) {
                        case UNIT_Inf:
                            rpt.agg_surf.inf -= active_unit->hp;
                            if (aggressor) {
                                aggressor->transfer_inf(active_unit->hp);
                            }
                            break;
                        case UNIT_Gli:
                            rpt.agg_surf.gli -= active_unit->hp;
                            if (aggressor) {
                                aggressor->transfer_gli(active_unit->hp);
                            }
                            break;
                        case UNIT_Rebel:
                            rpt.agg_surf.inf -= active_unit->hp;
                            break;
                        default:
                            L.warn("Invalid unit for tele: %d", (int)active_unit->type);
                            break;
                    }

                    active_unit->hp = 0;
                    active_unit->teleported = true;
                    draw_manager.draw(active_unit->spr_id, nullptr);

                    tele_done = true;
                }

                draw_manager.draw(
                    id(ID::TELELIGHTS),
                    IMG_GF4_25_2,
                    {SURF_X + active_tele->x * BLK_SZ,
                     SURF_Y + active_tele->y * BLK_SZ,
                     0, 0, 1, 1});

                draw_manager.draw(
                    id(ID::TELE0),
                    IMG_GF4_BEAM1,
                    {SURF_X + active_tele->x * BLK_SZ,
                     SURF_Y + active_tele->y * BLK_SZ,
                     0, 0, 1, 1});

                if (tele_timer > .2f && tele_timer < .8f) {
                    draw_manager.draw(
                        id(ID::TELE1),
                        IMG_GF4_BEAM2,
                        {SURF_X + active_tele->x * BLK_SZ,
                         SURF_Y + active_tele->y * BLK_SZ,
                         0, 0, 1, 1});
                } else {
                    draw_manager.draw(id(ID::TELE1), nullptr);
                }

                if (tele_timer > .4f && tele_timer < .6f) {
                    draw_manager.draw(
                        id(ID::TELE2),
                        IMG_GF4_BEAM3,
                        {SURF_X + active_tele->x * BLK_SZ,
                         SURF_Y + active_tele->y * BLK_SZ,
                         0, 0, 1, 1});
                    draw_manager.draw(
                        id(ID::TELESTAR),
                        IMG_GF4_BEAM4,
                        {SURF_X + active_tele->x * BLK_SZ,
                         SURF_Y + active_tele->y * BLK_SZ,
                         0, 0, 1, 1});
                } else {
                    draw_manager.draw(id(ID::TELE2), nullptr);
                    draw_manager.draw(id(ID::TELESTAR), nullptr);
                }

                tele_timer += delta * 1.4f;

                if (tele_timer < 1) {
                    return ExodusMode::MODE_None;
                } else {
                    draw_manager.draw(id(ID::TELE0), nullptr);
                    draw_manager.draw(id(ID::TELE1), nullptr);
                    draw_manager.draw(id(ID::TELE2), nullptr);
                    draw_manager.draw(id(ID::TELESTAR), nullptr);
                    draw_manager.draw(id(ID::TELELIGHTS), nullptr);
                    stage = LB_CheckWon;
                }
            }
            break;
        case LB_Fire:
            // Handle button clicks that require a stage change
            if (human_turn) {
                stage = update_buttons();

                if (stage != LB_Fire) {
                    return ExodusMode::MODE_None;
                }
            }

            if (shot_interp > 0) {
                shot_interp -= (fast ? FAST_SHOT_MULTIPLIER : 1.f) * delta * active_unit->fire_rate;
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
                    L.debug("%s FIRING AT %s", active_unit->debug_info(), target_unit->debug_info());
                    active_unit->shots_remaining--;
                    shot_interp = 1;
                    audio_manager.play_sfx(active_unit->shot_sfx);
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
                /*
                 * Slight deviation from orig here - when CPU attacks us,
                 * or rebels / alients attack us, both armies are given
                 * average-quality officers (see params to PROCb_start()).
                 * This seems off, however. Give each player their hired
                 * officer quality.
                 */
                OfficerQuality offq = OFFQ_Average;
                if (defender_turn) {
                    offq = def_officer;
                } else if (aggressor) {
                    offq = agg_officer;
                }
                switch (offq) {
                    case OFFQ_Poor:
                        power -= 1;
                        break;
                    case OFFQ_Good:
                        power += 1;
                        break;
                    default:
                        break;
                }
                power = clamp(power, 0, 4);

                if (active_unit->promoted) {
                    L.info("Promotion power bonus");
                    // Careful not to raise power beyond 5!
                    ++power;
                }

                if (active_unit->is_alien) {
                    if (cover) {
                        power = 1;
                    } else {
                        alienhit = true;
                    }
                }
                L.info("Power: %d", power);
                for (int i = 0; i < active_unit->hp; ++i) {
                    bool hit = false;

                    if (alienhit) {
                        // Aliens shooting on units not in cover always hit
                        hit = true;
                    } else {
                        if (power < 5) {
                            hit = onein(6 - power);
                        } else {
                            hit = !(onein(3 + power-5));
                        }
                    }

                    if (hit) {
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

                    UnitCount *uc_lost = nullptr;
                    UnitCount *uc_surf = nullptr;
                    if (target_unit->defending) {
                        uc_lost = &rpt.def_lost;
                        uc_surf = &rpt.def_surf;
                    } else {
                        uc_lost = &rpt.agg_lost;
                        uc_surf = &rpt.agg_surf;
                    }

                    int lost_scrap = 0;
                    int surf_scrap = 0;

                    int *lost_tracker = &lost_scrap;
                    int *surf_tracker = &surf_scrap;

                    switch (target_unit->type) {
                        case UNIT_Inf:
                            lost_tracker = &uc_lost->inf;
                            surf_tracker = &uc_surf->inf;
                            break;
                        case UNIT_Gli:
                            lost_tracker = &uc_lost->gli;
                            surf_tracker = &uc_surf->gli;
                            break;
                        case UNIT_Art:
                            lost_tracker = &uc_lost->art;
                            surf_tracker = &uc_surf->art;
                            break;
                        case UNIT_LBGun:
                            lost_tracker = &uc_lost->base;
                            surf_tracker = &uc_surf->base;
                            break;
                        case UNIT_Rebel:
                            lost_tracker = &uc_lost->inf;
                            surf_tracker = &uc_surf->inf;
                            break;
                        default:
                            break;
                    }

                    if (mine_damage) {
                        (*lost_tracker) += target_unit->hp;
                        (*surf_tracker) -= target_unit->hp;
                        damage_to_apply = 0;
                        target_unit->hp = 0;
                    } else {
                        int to_subtract = 0;
                        if (fast) {
                            // In fast mode, when >=1 damage is incurred, play one explosion and apply all of it
                            to_subtract = min(damage_to_apply, target_unit->hp);
                            damage_to_apply = 0;
                        } else {
                            to_subtract = min(1, target_unit->hp);
                            --damage_to_apply;
                        }

                        (*lost_tracker) += to_subtract;
                        (*surf_tracker) -= to_subtract;
                        target_unit->hp = max(0, target_unit->hp - to_subtract);
                    }
                }
                break;
            } else if (damage_to_apply > 0) {
                exp_interp = 1;
                if (FEATURE(EF_LUNAR_BATTLE_LOUD_MINES) && mine_damage) {
                    audio_manager.play_sfx(SFX_EXPLOSION_F);
                } else {
                    audio_manager.play_sfx(SFX_EXPLOSION);
                }
                break;
            }

            if (target_unit->hp <= 0) {
                L.info("Unit dead: %s", target_unit->debug_info());

                if (FEATURE(EF_LUNAR_BATTLE_PROMOTION) && active_unit && !mine_damage) {
                    OfficerQuality offq = active_unit->defending ? def_officer : agg_officer;

                    if (active_unit->try_promote(*target_unit, offq)) {
                        L.info("Unit promoted: %s", active_unit->debug_info());
                        audio_manager.play_sfx(SFX_PROMOTION);
                    }
                }

                target_unit->dying_timer = DYING_TIME;

                // Ensure that dead units are at the bottom of the draw stack,
                // but this one is moved to the top of the dead pile.
                // (Move this back to the top, then all live ones)
                draw_manager.refresh_sprite_id(target_unit->spr_id);
                for (int i = 0; i < n_units; ++i) {
                    if (units[i].hp > 0) {
                        if (!units[i].spr_id_set) {
                            L.error("Unit has no sprite ID on refresh");
                        }
                        draw_manager.refresh_sprite_id(units[i].spr_id);
                        draw_manager.refresh_sprite_id(units[i].fire_spr_id);
                        draw_manager.refresh_sprite_id(units[i].marker_spr_id);
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
                        rpt.def_lost.base += units[i].hp;
                        rpt.def_surf.base -= units[i].hp;
                        units[i].hp = 0;
                    }
                }

                // The base guns are destroyed permanently
                p->destroy_lunar_base();
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

                if (rpt.human_surrendered) {
                    rpt.aggressor_won = !(aggressor && aggressor->is_human());

                    // Result of PROCquerybattle

                    /*
                     * What's the point of surrendering? In the original you seem to lose
                     * all of your units either way - you might as well take out as many
                     * of the opponent's units as possible...
                     */

                    // SUGGEST: Give surrendering some advantage?

                    if (aggressor && aggressor->is_human()) {
                        rpt.agg_lost = rpt.agg_init;
                        rpt.agg_surf.clear();
                    } else {
                        rpt.def_lost = rpt.def_init;
                        rpt.def_surf.clear();
                    }
                    rpt.agg_lost.gli++;
                    stage = LB_Won;
                } else if (agg_units == 0 || def_units == 0) {
                    rpt.aggressor_won = def_units == 0;
                    stage = LB_Won;
                } else {
                    // Do this by default, if we don't decide to open comms
                    stage = LB_SelectUnit;

                    // If human turn has just ended, see if AI wants to open comms
                    if (human_turn) {
                        if (onein(40)) {
                            if (b.aggressor_type == AGG_Player) {
                                // If CPU is attacking us, only some lords will open comms
                                if (defending && aggressor->get_flag(0) == AI_Hi && aggressor->get_flag(4) == AI_Hi) {
                                    comm_open(DIA_S_B_CPU_OpenCommsAttacker);
                                    stage = LB_CommAI;
                                    // Skip battle drawing etc
                                    return ExodusMode::MODE_None;
                                }
                                // If we are attacking, open comms if >=100MC, strong attack force and defender's only planet
                                if (!defending && defender->can_afford(100)) {
                                    int aat = 0;
                                    int adf = 0;
                                    calc_force_strength(aat, adf);

                                    if ((aat*2 > adf*3) && (exostate().get_n_planets(defender) == 1)) {
                                        comm_open(DIA_S_B_CPU_OpenCommsDefender);
                                        stage = LB_CommAI;
                                        // Skip battle drawing etc
                                        return ExodusMode::MODE_None;
                                    }
                                }
                            } else if (b.aggressor_type == AGG_Rebels) {
                                comm_open(DIA_S_B_CPU_OpenCommsRebels);
                                stage = LB_CommAI;
                                // Skip battle drawing etc
                                return ExodusMode::MODE_None;
                            }
                        }
                    }
                }
            }
            break;
        case LB_Won:
            L.debug("BATTLE WINNER: %s", rpt.aggressor_won ? "AGG" : "DEF");
            ephstate.set_ephemeral_state(EPH_LunarBattleReport);
            batrpt_draw(true);
            stage = LB_BatRpt;
            return ExodusMode::MODE_None;
        case LB_BatRpt:
            if (draw_manager.clicked()) {
                if (!batrpt_draw(false)) {
                    return ephstate.get_appropriate_mode();
                }
            }
            // Skip drawing / cursor updating etc
            return ExodusMode::MODE_None;
        case LB_Info:
            {
                if (draw_manager.clicked()) {
                    hide_info();
                    stage = button_prev_stage;
                }

                return ExodusMode::MODE_None;
            }
            break;
        case LB_Surrender:
            {
                SpriteClick clk = draw_manager.query_click(id(ID::SURRENDER_YESNO));
                if (clk.id) {
                    draw_manager.draw(id(ID::SURRENDER_YESNO), nullptr);
                    draw_manager.draw(id(ID::PANEL_PATTERN), nullptr);
                    draw_manager.draw(id(ID::PANEL), nullptr);
                    if (clk.x < 0.5f) {
                        // Yes - surrender
                        surrender_timer = 0;
                        stage = LB_SurrenderDelay;
                    } else {
                        // No - don't surrender
                        stage = button_prev_stage;
                    }
                }
                return ExodusMode::MODE_None;
            }
            break;
        case LB_SurrenderDelay:
            surrender_timer += delta;
            if (surrender_timer > 1) {
                rpt.human_surrendered = true;
                stage = LB_CheckWon;
            }
            return ExodusMode::MODE_None;
        case LB_Manual:
            bulletin_update(delta);
            switch (bulletin_get_praction()) {
                case BPR_Close:
                    bulletin_ensure_closed();
                    stage = LB_Move;
                default:
                    break;
            }
            return ExodusMode::MODE_None;
    }

    if (FEATURE(EF_LUNAR_BATTLE_NEW_PANELS)) {
        update_panel_new();
    } else {
        update_panel();
    }

    draw_units();
    draw_markers();
    update_cursor();
    update_arrows();
    draw_explosion();

    if (stage == LB_Fire) {
        fire_time += delta;
    }

    return ExodusMode::MODE_None;
}

// PROCb_calc
void LunarBattle::auto_run() {
    LunarBattleParams &b = ephstate.lunar_battle;
    LunarBattleReport &rpt = ephstate.lunar_battle_report;

    Planet *p = exostate().get_active_planet();
    /*
     * FIXME: Should this be 12 or 24?
     * Base constitutes 4*LUNAR_BASE_GUN_HP = 4*6 = 24 units
     * For now, I'm going to adjust rpt.def_init to suggest that we only used 12 units...
     */
    auto_base = p->has_lunar_base() ? 12 : 0;
    rpt.def_init.base = 12;

    bool resolved = false;
    int iter = 0;
    while (!resolved) {
        L.debug("Auto battle: Iteration %d", iter++);

        int &a_inf = b.aggressor_inf;
        int &a_gli = b.aggressor_gli;
        int &a_art = b.aggressor_art;
        int &d_inf = b.defender_inf;
        int &d_gli = b.defender_gli;
        int &d_art = b.defender_art;
        int ra = a_inf + a_gli + a_art;
        // FIXME: Orig doesn't include bs (auto_base) here - should we?
        int rd = d_inf + d_gli + d_art;

        int more = 0;
        int round = 0;
        bool agg_more = false;

        if (ra > rd) {
            more = ra - rd;
            round = ra - more;
            agg_more = true;  // Orig: which=1
        } else {
            more = rd - ra;
            round = rd - more;
            agg_more = false; // Orig: which=2
        }

        ra = a_inf + a_gli + a_art;
        rd = d_inf + d_gli + d_art + auto_base;

        for (int phase = 0; phase < round; ++phase) {
            if (ra > 0) {
                auto_act(true);
                ra = a_inf + a_gli + a_art;
                rd = d_inf + d_gli + d_art + auto_base;
            }
            if (rd > 0) {
                auto_act(false);
                ra = a_inf + a_gli + a_art;
                rd = d_inf + d_gli + d_art + auto_base;
            }
        }

        for (int phase = 0; phase < more; ++phase) {
            if (agg_more) {
                if (ra > 0) {
                    auto_act(true);
                    ra = a_inf + a_gli + a_art;
                    rd = d_inf + d_gli + d_art + auto_base;
                }
            } else {
                if (rd > 0) {
                    auto_act(false);
                    ra = a_inf + a_gli + a_art;
                    rd = d_inf + d_gli + d_art + auto_base;
                }
            }
        }

        if (rd <= 0) {
            rpt.aggressor_won = true;
            resolved = true;
        } else if (ra <= 0) {
            rpt.aggressor_won = false;
            resolved = true;
        }
    }
}

// PROCbc_act
void LunarBattle::auto_act(bool agg) {
    LunarBattleParams &b = ephstate.lunar_battle;
    LunarBattleReport &rpt = ephstate.lunar_battle_report;

    int base = 0;
    int &a_inf = b.aggressor_inf;
    int &a_gli = b.aggressor_gli;
    int &a_art = b.aggressor_art;
    // Teleporters never used in auto battles
    int &d_inf = b.defender_inf;
    int &d_gli = b.defender_gli;
    int &d_art = b.defender_art;
    int &mines = b.defender_mines;
    int i, g, a;

    if (agg) {
        base = 0;
        i = a_inf;
        g = a_gli;
        a = a_art;
    } else {
        base = auto_base;
        i = d_inf;
        g = d_gli;
        a = d_art;
    }

    // Proportion
    int prop = i+g+a+base;
    int r = prop > 1 ? RND(prop) : 1;
    int an = 0;
    if (r <= i) an = 1;
    if (r <= i+g && an == 0) an = 2;
    if (r <= i+g+a && an == 0) an = 3;
    if (r > i+g+a) an = 4;

    if (an == 2 && agg && mines>0 && onein(20)) {
        /*
         * Mines unaltered - however it only destroys 1 unit as
         * opposed to 1 stack, so perhaps this is balanced.
         */
        a_gli--;
        an = 0;
        rpt.agg_lost.gli++;
        rpt.agg_surf.gli--;
    }

    if (an == 1 && onein(4)) an = 0;
    if (an == 2 && onein(5)) an = 0;
    if (an == 3 && onein(7)) an = 0;

    if (an > 0) {
        int power = an;
        if (agg && b.aggressor_type == AGG_Rebels) {
            power = 1;
        }

        /*
         * Slight deviation from orig here - when CPU attacks us,
         * or rebels / alients attack us, both armies are given
         * average-quality officers (see params to PROCb_start()).
         * This seems off, however. Give each player their hired
         * officer quality.
         */
        OfficerQuality offq = OFFQ_Average;
        if (!agg) {
            offq = def_officer;
        } else if (aggressor) {
            offq = agg_officer;
        }
        switch (offq) {
            case OFFQ_Poor:
                power -= 1;
                break;
            case OFFQ_Good:
                power += 1;
                break;
            default:
                break;
        }

        power = min(power, 4);
        power = max(power, 0);

        if ((agg && b.aggressor_type == AGG_Aliens) || onein(6-power)) {
            auto_kill(agg);
        }
    }
}

// PROCbc_kill
void LunarBattle::auto_kill(bool agg) {
    LunarBattleParams &b = ephstate.lunar_battle;
    LunarBattleReport &rpt = ephstate.lunar_battle_report;

    int kbase = 0;
    int &a_inf = b.aggressor_inf;
    int &a_gli = b.aggressor_gli;
    int &a_art = b.aggressor_art;
    int &d_inf = b.defender_inf;
    int &d_gli = b.defender_gli;
    int &d_art = b.defender_art;
    int ki, kg, ka;

    if (agg) {
        kbase = auto_base;
        ki = d_inf;
        kg = d_gli;
        ka = d_art;
    } else {
        kbase = 0;
        ki = a_inf;
        kg = a_gli;
        ka = a_art;
    }

    if (ki+kg+ka+kbase <= 0) {
        // No opposing units left to attack
        return;
    }

    // Proportion
    int kprop = ki+kg+ka+kbase;

    int r = (kprop > 1 ? RND(kprop) : 1);

    int kan = 0;

    if (r <= ki) kan = 1;
    if (r <= ki+kg && kan == 0) kan = 2;
    if (r <= ki+kg+ka && kan == 0) kan = 3;
    if (r > ki+kg+ka) kan = 4;

    int *tgt = nullptr;

    int lost_scrap = 0;
    int surf_scrap = 0;

    int *lost_tracker = &lost_scrap;
    int *surf_tracker = &surf_scrap;

    if (agg) {
        if (kan == 1) {
            tgt = &d_inf;
            lost_tracker = &rpt.def_lost.inf;
            surf_tracker = &rpt.def_surf.inf;
        }
        if (kan == 2) {
            tgt = &d_gli;
            lost_tracker = &rpt.def_lost.gli;
            surf_tracker = &rpt.def_surf.gli;
        }
        if (kan == 3) {
            tgt = &d_art;
            lost_tracker = &rpt.def_lost.art;
            surf_tracker = &rpt.def_surf.art;
        }
        if (kan == 4) {
            tgt = &auto_base;
            lost_tracker = &rpt.def_lost.base;
            surf_tracker = &rpt.def_surf.base;
        }
    } else {
        if (kan == 1) {
            tgt = &a_inf;
            lost_tracker = &rpt.agg_lost.inf;
            surf_tracker = &rpt.agg_surf.inf;
        }
        if (kan == 2) {
            tgt = &a_gli;
            lost_tracker = &rpt.agg_lost.gli;
            surf_tracker = &rpt.agg_surf.gli;
        }
        if (kan == 3) {
            tgt = &a_art;
            lost_tracker = &rpt.agg_lost.art;
            surf_tracker = &rpt.agg_surf.art;
        }
    }

    if (tgt) {
        if (*tgt > 0) {
            (*tgt)--;
            (*lost_tracker)++;
            (*surf_tracker)--;
        } else {
            L.error("Invalid target in auto battle: %s %d", agg?"AGG":"DEF", kan);
        }
    }
}

void LunarBattle::place_cover() {
    int n_cover = RND(COVER_MAX); // 1 - 10 cover obstacles
    int i;
    for (i = 0; i < n_cover; ++i) {
        cover[i] = Cover(rand() % BG_WIDTH, rand() % BG_HEIGHT, (bool)(rand() % 2));
    }
    for (; i < COVER_MAX; ++i) {
        cover[i].exists = false;
    }
}

// PROCb_esetup
void LunarBattle::place_units(bool def) {
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
                if (unit_at(x, y) || cover_at(x, y)) {
                    continue;
                }
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

    BattleUnit u_inf = UNIT_Inf;
    if (!def) {
        if (b.aggressor_type == AGG_Aliens) u_inf = UNIT_AInf;
        if (b.aggressor_type == AGG_Rebels) u_inf = UNIT_Rebel;
    }

    BattleUnit u_gli = UNIT_Gli;

    if (!onein(3)) {
        // We use a particular inf/gli placement strategy 2/3 of the time...
        int lim = onein(3) ? 1 : 0;
        int g = onein(3) ? 5 : 7;
        bool done = (inf <= 0) && (gli <= 0) && (art <= 0);
        for (int x = g-1; g >= 0; --g) {
            int _x = x;
            if (def) {
                _x = BG_WIDTH - x;
            }
            for (int y = 0; y < 11; ++y) {
                int l = 0;
                if (lim == 1 && (y == 4 || y == 5)) {
                    l = 1;
                }

                if (l == 0 && !unit_at(_x, y)) {
                    bool tel = false;
                    for (int i = 0; i < n_tele; ++i) {
                        if (tele[i].x == _x && tele[i].y == y) {
                            tel = true;
                            break;
                        }
                    }

                    if (tel) {
                        continue;
                    }

                    if (inf > 0) {
                        place_unit(BattleUnit(u_inf).init(_x, y, min(stack, inf), def));
                        inf -= min(stack, inf);
                    } else if (gli > 0) {
                        place_unit(BattleUnit(u_gli).init(_x, y, min(stack, gli), def));
                        gli -= min(stack, gli);
                    } else if (art > 0) {
                        // Shouldn't happen, as we already placed artillery...
                        place_unit(BattleUnit(UNIT_Art).init(_x, y, min(stack, art), def));
                        art -= min(stack, art);
                    }

                    done = (inf <= 0) && (gli <= 0) && (art <= 0);
                }
                if (done) {
                    break;
                }
            }
            if (done) {
                break;
            }
        }
    } else {
        while (inf > 0) {
            int x = rand() % 7;
            int y = rand() % 11;
            if (def) {
                x = (rand() % 8) + 8;
            }
            if (unit_at(x, y) || cover_at(x, y)) {
                continue;
            }
            place_unit(BattleUnit(u_inf).init(x, y, min(stack, inf), def));
            inf -= min(stack, inf);
        }

        while (gli > 0) {
            int x = rand() % 7;
            int y = rand() % 11;
            if (def) {
                x = (rand() % 8) + 8;
            }
            if (unit_at(x, y) || cover_at(x, y)) {
                continue;
            }
            place_unit(BattleUnit(u_gli).init(x, y, min(stack, gli), def));
            gli -= min(stack, gli);
        }
    }

    // Clear up cover with a unit on it
    for (int i = 0; i < COVER_MAX; ++i) {
        if (unit_at(cover[i].x, cover[i].y)) {
            cover[i].exists = false;
        }
    }

    // Place defender mines
    if (def) {
        for (int i = 0; i < b.defender_mines; ++i) {
            for (int attempts = 0; attempts < 10000; ++attempts) {
                int mine_x = (RND(3) + 9) - 1;
                int mine_y = RND(11) - 1;
                bool is_suitable = valid_placement(mine_x, mine_y);
                if (is_suitable) {
                    for (int j = 0; j < COVER_MAX; ++j) {
                        if (cover[j].exists && cover[j].x == mine_x && cover[j].y == mine_y) {
                            is_suitable = false;
                            break;
                        }
                    }
                }
                if (is_suitable) {
                    // Place mine
                    mines[n_mines].x = mine_x;
                    mines[n_mines].y = mine_y;
                    mines[n_mines].live = true;
                    mines[n_mines].discovered = false;
                    mines[n_mines].spr_id = draw_manager.new_sprite_id();
                    n_mines++;
                    L.debug("Placed mine at %d %d", mine_x, mine_y);
                    break;
                }
            }
        }
    }

    // Place aggressor teleporters
    // FIXME: It looks like orig places these for non-player aggressors - should we?
    if (!def) {
        for (int i = 0; i < b.aggressor_tele; ++i) {
            for (int attempts = 0; attempts < 10000; ++attempts) {
                int tele_x = RND(6) - 1;
                int tele_y = RND(11) - 1;
                bool is_suitable = valid_placement(tele_x, tele_y);
                if (is_suitable) {
                    for (int j = 0; j < COVER_MAX; ++j) {
                        if (cover[j].exists && cover[j].x == tele_x && cover[j].y == tele_y) {
                            is_suitable = false;
                            break;
                        }
                    }
                }
                if (is_suitable) {
                    // Place tele
                    tele[n_tele].x = tele_x;
                    tele[n_tele].y = tele_y;
                    tele[n_tele].spr_id = draw_manager.new_sprite_id();
                    n_tele++;
                    L.debug("Placed tele at %d %d", tele_x, tele_y);
                    break;
                }
            }
        }
    }
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

bool LunarBattle::cover_at(int x, int y) {
    for (int i = 0; i < COVER_MAX; ++i) {
        if (cover[i].exists && cover[i].x == x && cover[i].y == y) {
            return true;
        }
    }

    return false;
}

// Commonly-used force stength metric
void LunarBattle::calc_force_strength(int& aat, int& adf) {
    for (int i = 0; i < n_units; ++i) {
        int hp = max(0, units[i].hp);
        if (units[i].defending) {
            switch (units[i].type) {
                case UNIT_Inf:
                    adf += hp;
                    break;
                case UNIT_Gli:
                    adf += 2*hp;
                    break;
                case UNIT_Art:
                    adf += 3*hp;
                    break;
                case UNIT_LBGun:
                    adf += 4*hp;
                    break;
                default:
                    break;
            }
        } else {
            switch (units[i].type) {
                case UNIT_Inf:
                    aat += hp;
                    break;
                case UNIT_Gli:
                    aat += 2*hp;
                    break;
                case UNIT_Art:
                    aat += 3*hp;
                    break;
                case UNIT_LBGun:
                    L.warn("Attacker shouldn't have LBGuns");
                    aat += 4*hp;
                    break;
                default:
                    break;
            }
        }
    }
}

enum DrawMineRule : uint8_t {
    None,
    InfOnly,
    Discovered,
    All
};

void LunarBattle::draw_mines() {
    DrawMineRule draw_mine_rule = DrawMineRule::None;

    if (!(aggressor && aggressor->is_human())) {
        draw_mine_rule = DrawMineRule::All;
    }

    if (FEATURE(EF_LUNAR_BATTLE_INF_DISCOVER_MINES)) {
        if (aggressor && aggressor->is_human()) {
            OfficerQuality offq = agg_officer;
            if (offq == OFFQ_Average) draw_mine_rule = DrawMineRule::InfOnly;
            if (offq == OFFQ_Good) draw_mine_rule = DrawMineRule::Discovered;
        }
    }

    if (draw_mine_rule != DrawMineRule::None) {
        for (int i = 0; i < n_mines; ++i) {
            bool draw_mine = false;

            switch (draw_mine_rule) {
                case DrawMineRule::None:
                    break;
                case DrawMineRule::InfOnly:
                    {
                        BattleUnit *u = unit_at(mines[i].x, mines[i].y);
                        if (u && !(u->defending) && (u->type == UNIT_Inf)) {
                            draw_mine = mines[i].live;
                        }
                    }
                    break;
                case DrawMineRule::Discovered:
                    draw_mine = mines[i].live && mines[i].discovered;
                    break;
                case DrawMineRule::All:
                    draw_mine = mines[i].live;
                    break;
            }

            if (draw_mine) {
                // FIXME: It's a hack to refresh mine IDs every time we draw them!
                // Ensure this mine draws above anything else
                draw_manager.refresh_sprite_id(mines[i].spr_id);
                // Except the cursor and explosion
                draw_manager.refresh_sprite_id(id(ID::CURSOR));
                draw_manager.refresh_sprite_id(id(ID::EXPLOSION));
                draw_manager.draw(
                    mines[i].spr_id,
                    IMG_GF4_19,
                    {SURF_X + mines[i].x * BLK_SZ,
                     SURF_Y + mines[i].y * BLK_SZ,
                     0, 0, 1, 1});
            } else {
                draw_manager.draw(
                    mines[i].spr_id,
                    nullptr);
            }
        }
    }
}

void LunarBattle::draw_units() {
    bool hide_enemies = false;
    if (stage == LB_Placement || stage == LB_PlacementEnd) {
        hide_enemies = true;
    }

    for (int i = 0; i < n_tele; ++i) {
        draw_manager.draw(
            tele[i].spr_id,
            IMG_GF1_25,
            {SURF_X + tele[i].x * BLK_SZ,
             SURF_Y + tele[i].y * BLK_SZ,
             0, 0, 1, 1});
    }

    for (int pass = 0; pass < 2; ++pass) {
        bool draw_dead = pass == 0;
        for (int i = 0; i < n_units; ++i) {
            // If in placement mode, don't draw enemies
            if (hide_enemies) {
                if (units[i].defending != placement_def) {
                    continue;
                }
            }

            if (draw_dead && units[i].hp > 0)
                continue;
            if (!draw_dead && units[i].hp <= 0)
                continue;
            if (units[i].teleported)
                continue;

            if (!units[i].spr_id_set) {
                L.error("Unit has no sprite ID on draw");
                continue;
            }

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

            bool draw_fire = false;
            if (&units[i] == active_unit) {
                if (shot_interp > 0.5f) {
                    draw_fire = true;
                }
            }

            if (units[i].hp <= 0) {
                spr = units[i].dead;
                if ((units[i].dying_timer) > 0 && units[i].dying) {
                    spr = units[i].dying;
                }
            }

            int dx = draw_x;
            float anchor_x = 0;
            if (units[i].defending) {
                dx = draw_x + BLK_SZ;
                anchor_x = 1;
            }

            draw_manager.draw(
                units[i].spr_id,
                spr,
                {dx, draw_y,
                 anchor_x, 0, 1, 1});

            // Draw the firing sprite on top of the idle sprite
            if (draw_fire) {
                draw_manager.draw(
                    units[i].fire_spr_id,
                    units[i].fire,
                    {dx, draw_y,
                     anchor_x, 0, 1, 1});
            } else {
                draw_manager.draw(units[i].fire_spr_id, nullptr);
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

    draw_mines();
}

void LunarBattle::draw_markers() {
    for (int i = 0; i < n_units; ++i) {
        int x = units[i].x;
        int y = units[i].y;
        int draw_x = SURF_X + x * BLK_SZ;
        int draw_y = SURF_Y + y * BLK_SZ;

        int dx = draw_x;
        if (units[i].defending) {
            dx = draw_x + BLK_SZ;
        }

        if (human_turn && stage == LB_Fire) {
            if (check_viable_target(&units[i])) {
                draw_manager.draw(
                    units[i].marker_spr_id,
                    IMG_MARKER_FIRE,
                    {dx + (units[i].defending ? -BLK_SZ/2 : BLK_SZ/2), draw_y - 14,
                     0.5f, 0, 1, 1});
            }
        } else {
            draw_manager.draw(
                units[i].marker_spr_id,
                nullptr);
        }
    }
}

void LunarBattle::draw_explosion() {
    if (exp_interp > 0 && target_unit) {
        int x = target_unit->x;
        int y = target_unit->y;
        int draw_x = SURF_X + x * BLK_SZ;
        int draw_y = SURF_Y + y * BLK_SZ;
            draw_manager.draw(
                id(ID::EXPLOSION),
                anim_explode.interp(1 - exp_interp),
                {draw_x + BLK_SZ/2, draw_y + BLK_SZ/2,
                 0.5, 0.5, 1, 1});
    } else {
        draw_manager.draw(id(ID::EXPLOSION), nullptr);
    }
}

void LunarBattle::update_cursor() {
    if (human_turn && unit_moving) {
        cursor_x = -1;
        cursor_y = -1;
        draw_manager.draw(id(ID::CURSOR), nullptr);
        return;
    }

    SpriteClick mouseover = draw_manager.query_mouseover(id(ID::BG));

    cursor_prev_x = cursor_x;
    cursor_prev_y = cursor_y;

    if (mouseover.id) {
        cursor_x = min((int)(mouseover.x * BG_WIDTH), BG_WIDTH - 1);
        cursor_y = min((int)(mouseover.y * BG_HEIGHT), BG_HEIGHT - 1);
    } else {
        cursor_x = -1;
        cursor_y = -1;
    }

    bool over_target = false;
    bool over_aiming = false;

    BattleUnit *u = unit_at(cursor_x, cursor_y);

    if (stage == LB_Fire) {
        over_target = check_viable_target(u);
        over_aiming = !target_unit && unit_at(cursor_x, cursor_y) == active_unit;
    }

    if (over_aiming) {
        return;
    }

    //Planet *p = exostate().get_active_planet();
    //bool ice = p->get_moon_class() == MOON_Ice;

    const char* cursor_img = IMG_CURSOR_BATTLE_B;

    if (stage == LB_Fire || stage == LB_CalcDamage || stage == LB_Damage) {
        if (over_target) {
            cursor_img = IMG_CURSOR_BATTLE_G;
        } else if (u) {
            if (stage == LB_Damage) {
                cursor_img = IMG_CURSOR_BATTLE_G;
            } else {
                cursor_img = IMG_CURSOR_BATTLE_R;
            }
        }
    }

    if (cursor_x >= 0 && cursor_y >= 0) {
        draw_manager.refresh_sprite_id(id(ID::CURSOR));

        draw_manager.draw(
            id(ID::CURSOR),
            cursor_img,
            {SURF_X + BLK_SZ * cursor_x,
             SURF_Y + BLK_SZ * cursor_y,
             0, 0, 1, 1});
    }
}

void LunarBattle::update_panel() {
    LBPanelMode target_mode = (stage == LB_Placement || stage == LB_PlacementEnd)
                              ? LBPM_Placement
                              : LBPM_Battle;

    if (panel_mode != target_mode) {
        draw_manager.fill({0, SURF_Y - 1, RES_X, 1}, {0, 0, 0});
        draw_manager.fill({0, 0, RES_X, SURF_Y - 1}, COL_BORDERS);

        if (target_mode == LBPM_Placement) {
            for (int i = 0; i < 4; ++i) {
                draw_manager.fill({8 + i*96, 16, 42, 42}, {0, 0, 0});
                draw_manager.fill_pattern({54 + i*96, 37, 35, 21});
            }
            draw_manager.fill_pattern({390, 16, 240, 42});
        }

        if (target_mode == LBPM_Battle) {
            Planet *p = exostate().get_active_planet();

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

            draw_manager.fill_pattern({226, 12, 210, 50});

            const char* fast_button = nullptr;

            if (FEATURE(EF_FLIP_BUTTONS)) {
                fast_button = fast ? IMG_GF4_HMENU5 : IMG_GF4_HMENU3;
            } else {
                fast_button = fast ? IMG_GF4_HMENU3 : IMG_GF4_HMENU5;
            }

            draw_manager.draw(
                id(ID::BTN_INFO),
                IMG_GF4_HMENU1,
                {447, 8, 0, 0, 1, 1});
            draw_manager.draw(
                id(ID::BTN_SPEED),
                fast_button,
                {447, 38, 0, 0, 1, 1});
            draw_manager.draw(
                id(ID::BTN_TALK),
                IMG_GF4_HMENU2,
                {541, 8, 0, 0, 1, 1});
            draw_manager.draw(
                id(ID::BTN_QUIT),
                IMG_GF4_HMENU4,
                {541, 38, 0, 0, 1, 1});
        }
        draw_manager.save_background({0, 0, RES_X, SURF_Y - 1});
    }

    panel_mode = target_mode;

    switch (panel_mode) {
        case LBPM_Placement:
            update_panel_setup();
            break;
        case LBPM_Battle:
            update_panel_battle();
            break;
        case LBPM_None:
            break;
    }
}

// The panel as drawn during unit placement
void LunarBattle::update_panel_setup() {
    LunarBattleParams &b = ephstate.lunar_battle;
    const char* spr_inf = placement_def ? IMG_GF4_4  : IMG_GF4_1;
    const char* spr_gli = placement_def ? IMG_GF4_5  : IMG_GF4_2;
    const char* spr_art = placement_def ? IMG_GF4_6  : IMG_GF4_3;
    const char* spr_msc = placement_def ? IMG_GF4_19 : IMG_GF1_25;

    for (int i = 0; i < 4; ++i) {
        const char* spr = spr_inf;
        int n = 0;
        int n_groups = 0;
        if (i == 0) { spr = spr_inf; n = to_place_inf; }
        if (i == 1) { spr = spr_gli; n = to_place_gli; }
        if (i == 2) { spr = spr_art; n = to_place_art; }
        if (i == 3) { spr = spr_msc; n = to_place_msc; }

        if (i < 3) {
            if (placement_def) {
                n_groups = n / b.defender_group_size;
                if ((n % b.defender_group_size) > 0) {
                    n_groups++;
                }
            } else {
                n_groups = n / b.aggressor_group_size;
                if ((n % b.aggressor_group_size) > 0) {
                    n_groups++;
                }
            }
        } else {
            n_groups = n;
        }

        draw_manager.draw(
            placement_ids[i],
            spr,
            {9 + i*96, 17,
             0, 0, 1, 1});
        char n_str[8];
        snprintf(n_str, sizeof(n_str), "%d", n_groups);
        draw_manager.draw_text(
            placement_ids[4+i],
            n_str,
            Justify::Left,
            58 + i*96, 37,
            COL_TEXT);
    }

    const char* item_name = "Infantry";
    const char* spr_selected = spr_inf;
    char text[16];
    snprintf(text, sizeof(text),
             "(max %d/group)",
             placement_def ? b.defender_group_size : b.aggressor_group_size);

    if (placement_item == 1) {
        item_name = "Gliders";
        spr_selected = spr_gli;
    }
    if (placement_item == 2) {
        item_name = "Artillery",
        spr_selected = spr_art;
    }
    if (placement_item == 3) {
        item_name = placement_def ? "Mines" : (ENHANCED() ? "Rescue Pads" : "Teleporters"),
        spr_selected = spr_msc;
        text[0] = '\0';
    }
    draw_manager.draw(
        id(ID::PLACEMENT_SELECTED),
        spr_selected,
        {391, 17,
         0, 0, 1, 1});
    draw_manager.draw_text(
        id(ID::PLACEMENT_SELECTED_NAME),
        item_name,
        Justify::Left,
        437, 17,
        COL_TEXT2);
    draw_manager.draw_text(
        id(ID::PLACEMENT_SELECTED_DESC),
        text,
        Justify::Left,
        437, 35,
        COL_TEXT);
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

    bool redraw = (draw_unit != panel_unit);

    if (draw_unit && (draw_unit->hp != panel_hp)) {
        redraw = true;
    }

    if (redraw) {
        const char* text = STR_Ground;
        RGB text_col = {0, 0xFF, 0};  // TODO: Check exact colour
        panel_unit = draw_unit;
        panel_hp = draw_unit ? draw_unit->hp : -1;
        // Clears over drawn health info etc
        draw_manager.fill_pattern({226, 12, 210, 50});
        if (draw_unit) {
            text = draw_unit->name;

            bool our_side = (human_turn && active_unit->defending == draw_unit->defending);

            if (our_side) {
                text_col = {0, 0, 0xFF};  // TODO: Check exact colour
            }

            if (FEATURE(EF_LUNAR_BATTLE_ADVANCED_HP_INFO)) {
                if (human_turn) {
                    Player *player = defender_turn ? defender : aggressor;

                    OfficerQuality offq = OFFQ_Poor;

                    if (player) {
                        offq = defender_turn ? def_officer : agg_officer;
                    }

                    const char* text_hp = "HP:";
                    const char* text_hits = "HITS:";

                    if (our_side) {
                        draw_manager.draw_text(Font::Tiny, text_hp, Justify::Left, 234, 34, COL_TEXT3);

                        for (int i = 0; i < draw_unit->hp; ++i) {
                            draw_manager.draw(
                                IMG_GF4_SBR,
                                {232 + 10*i, 50, 0, 0, 1, 1});
                        }
                    } else if (offq != OFFQ_Poor) {
                        int dots_to_show = 0;
                        const char* text = text_hp;

                        if (offq == OFFQ_Average) {
                            text = text_hits;
                            dots_to_show = draw_unit->hp_initial - draw_unit->hp;
                        }

                        if (offq == OFFQ_Good) {
                            text = text_hp;
                            dots_to_show = draw_unit->hp;
                        }

                        draw_manager.draw_text(Font::Tiny, text, Justify::Left, 234, 34, COL_TEXT3);

                        for (int i = 0; i < dots_to_show; ++i) {
                            draw_manager.draw(
                                IMG_GF4_SBR_RED,
                                {232 + 10*i, 50, 0, 0, 1, 1});
                        }
                    }

                    if (is_in_cover(draw_unit)) {
                        draw_manager.draw(
                            IMG_GF4_COVER,
                            {430, 13, 1, 0, 1, 1});
                    }
                }
            } else {
                if (our_side) {
                    for (int i = 0; i < draw_unit->hp; ++i) {
                        draw_manager.draw(
                            IMG_GF4_SBR,
                            {232 + 10*i, 35, 0, 0, 1, 1});
                    }
                }

                if (is_in_cover(draw_unit)) {
                    draw_manager.draw(
                        IMG_GF4_COVER,
                        {232, 50, 0, 0, 1, 1});
                }
            }
        }
        draw_manager.draw_text(text, Justify::Left, 232, 13, text_col);
    }

    draw_manager.draw(id(ID::PANEL_UNIT), panel_spr, {177, 17, 0, 0, 1, 1});
}

LunarBattle::Stage LunarBattle::update_buttons() {
    LunarBattleParams &b = ephstate.lunar_battle;

    button_prev_stage = stage;

    bool defending = !(aggressor && aggressor->is_human());

    if (draw_manager.query_click(id(ID::BTN_INFO)).id) {
        show_info();
        return LB_Info;
    }

    if (draw_manager.query_click(id(ID::BTN_SPEED)).id) {
        fast = !fast;

        const char* fast_button = nullptr;

        if (FEATURE(EF_LUNAR_BATTLE_NEW_PANELS)) {
            fast_button = fast ? IMG_BATTLE_FF2 : IMG_BATTLE_FF;
        } else {
            if (FEATURE(EF_FLIP_BUTTONS)) {
                fast_button = fast ? IMG_GF4_HMENU5 : IMG_GF4_HMENU3;
            } else {
                fast_button = fast ? IMG_GF4_HMENU3 : IMG_GF4_HMENU5;
            }
        }

        if (FEATURE(EF_LUNAR_BATTLE_NEW_PANELS)) {
            draw_manager.draw(
                id(ID::BTN_SPEED),
                fast_button,
                {541, 8, 0, 0, 1, 1});
        } else {
            draw_manager.draw(
                id(ID::BTN_SPEED),
                fast_button,
                {447, 38, 0, 0, 1, 1});
        }
    }

    if (draw_manager.query_click(id(ID::BTN_TALK)).id) {
        calc_force_strength(comm_ctx.battle_strength_att, comm_ctx.battle_strength_def);
        switch (b.aggressor_type) {
            case AGG_Player:
                comm_open(defending ? DIA_S_B_OpenCommsDefender : DIA_S_B_OpenCommsAttacker);
                return LB_CommHuman;
            case AGG_Rebels:
                // (Same comms as when rebels contact us)
                comm_open(DIA_S_B_CPU_OpenCommsRebels);
                return LB_CommHuman;
            default:
                break;
        }
    }

    if (draw_manager.query_click(id(ID::BTN_MAN)).id) {
        bulletin_start_manual(BMP_LunarBattle);
        return LB_Manual;
    }

    if (draw_manager.query_click(id(ID::BTN_QUIT)).id) {
        draw_manager.fill(
            id(ID::PANEL),
            {PANEL_X - BORDER, PANEL_Y - BORDER,
            PANEL_W + 2*BORDER, PANEL_H + 2*BORDER},
            COL_BORDERS);
        draw_manager.fill_pattern(
            id(ID::PANEL_PATTERN),
            {PANEL_X, PANEL_Y,
            PANEL_W, PANEL_H});

        draw_manager.draw_text(
            "SURRENDER?",
            Justify::Centre,
            PANEL_X + PANEL_W/2, PANEL_Y + 24,
            COL_TEXT);

        int fill_w = 40;
        draw_manager.fill({PANEL_X+4, PANEL_Y+PANEL_H-50, fill_w, 26}, COL_BORDERS);
        draw_manager.fill({PANEL_X+PANEL_W-4-fill_w, PANEL_Y+PANEL_H-50, fill_w, 26}, COL_BORDERS);

        draw_manager.draw(
            id(ID::SURRENDER_YESNO),
            IMG_BR14_EXPORT,
            {PANEL_X + PANEL_W/2, PANEL_Y + PANEL_H - 24, 0.5, 1, 1, 1});

        return LB_Surrender;
    }

    return stage;
}

void LunarBattle::update_panel_new() {
    LBPanelMode target_mode = (stage == LB_Placement || stage == LB_PlacementEnd)
                              ? LBPM_Placement
                              : LBPM_Battle;

    if (panel_mode != target_mode) {
        draw_manager.draw(id(ID::TOP_PANEL), nullptr);

        if (target_mode == LBPM_Placement) {
            draw_manager.draw(
                id(ID::TOP_PANEL),
                IMG_BATTLE_PBAR,
                {0, 0, 0, 0, 1, 1});
        }

        if (target_mode == LBPM_Battle) {
            draw_manager.draw(
                id(ID::TOP_PANEL),
                IMG_BATTLE_BAR,
                {0, 0, 0, 0, 1, 1});

            Planet *p = exostate().get_active_planet();

            draw_manager.draw_text("The battle of", Justify::Left, 12, 18, COL_TEXT);
            draw_manager.draw_text(p->get_name(), Justify::Left, 12, 36, COL_TEXT);

            DrawArea a = {0, 0, 40, 40};
            draw_manager.set_source_region(id(ID::PANEL_UNIT_BG), &a);
            draw_manager.draw(
                id(ID::PANEL_UNIT_BG),
                p->moon_sprites()->bg,
                {178, 16, 0, 0, 1, 1});

            const char* fast_button = fast ? IMG_BATTLE_FF2 : IMG_BATTLE_FF;

            draw_manager.draw(
                id(ID::BTN_INFO),
                IMG_BATTLE_INFO,
                {447, 8, 0, 0, 1, 1});
            draw_manager.draw(
                id(ID::BTN_SPEED),
                fast_button,
                {541, 8, 0, 0, 1, 1});
            draw_manager.draw(
                id(ID::BTN_MAN),
                IMG_BATTLE_MAN,
                {627, 8, 1, 0, 1, 1});
            draw_manager.draw(
                id(ID::BTN_TALK),
                IMG_BATTLE_TALK,
                {447, 38, 0, 0, 1, 1});
            draw_manager.draw(
                id(ID::BTN_QUIT),
                IMG_BATTLE_QUIT,
                {541, 38, 0, 0, 1, 1});
        }
        draw_manager.save_background({0, 0, RES_X, SURF_Y - 1});
    }

    panel_mode = target_mode;

    switch (panel_mode) {
        case LBPM_Placement:
            update_panel_setup_new();
            break;
        case LBPM_Battle:
            update_panel_battle_new();
            break;
        case LBPM_None:
            break;
    }
}

// The panel as drawn during unit placement
void LunarBattle::update_panel_setup_new() {
    LunarBattleParams &b = ephstate.lunar_battle;
    const char* spr_inf = placement_def ? IMG_GF4_4  : IMG_GF4_1;
    const char* spr_gli = placement_def ? IMG_GF4_5  : IMG_GF4_2;
    const char* spr_art = placement_def ? IMG_GF4_6  : IMG_GF4_3;
    const char* spr_msc = placement_def ? IMG_GF4_19 : IMG_GF1_25;

    for (int i = 0; i < 4; ++i) {
        const char* spr = spr_inf;
        int n = 0;
        int n_groups = 0;
        if (i == 0) { spr = spr_inf; n = to_place_inf; }
        if (i == 1) { spr = spr_gli; n = to_place_gli; }
        if (i == 2) { spr = spr_art; n = to_place_art; }
        if (i == 3) { spr = spr_msc; n = to_place_msc; }

        if (i < 3) {
            if (placement_def) {
                n_groups = n / b.defender_group_size;
                if ((n % b.defender_group_size) > 0) {
                    n_groups++;
                }
            } else {
                n_groups = n / b.aggressor_group_size;
                if ((n % b.aggressor_group_size) > 0) {
                    n_groups++;
                }
            }
        } else {
            n_groups = n;
        }

        draw_manager.draw(
            placement_ids[i],
            spr,
            {9 + i*96, 22,
             0, 0, 1, 1});
        char n_str[8];
        snprintf(n_str, sizeof(n_str), "%d", n_groups);
        draw_manager.draw_text(
            placement_ids[4+i],
            n_str,
            Justify::Left,
            58 + i*96, 38,
            COL_TEXT);
    }

    const char* item_name = "Infantry";
    const char* spr_selected = spr_inf;
    char text[16];
    snprintf(text, sizeof(text),
             "(max %d/group)",
             placement_def ? b.defender_group_size : b.aggressor_group_size);

    if (placement_item == 1) {
        item_name = "Gliders";
        spr_selected = spr_gli;
    }
    if (placement_item == 2) {
        item_name = "Artillery",
        spr_selected = spr_art;
    }
    if (placement_item == 3) {
        item_name = placement_def ? "Mines" : (ENHANCED() ? "Rescue Pads" : "Teleporters"),
        spr_selected = spr_msc;
        text[0] = '\0';
    }
    draw_manager.draw(
        id(ID::PLACEMENT_SELECTED),
        spr_selected,
        {391, 22,
         0, 0, 1, 1});
    draw_manager.draw_text(
        id(ID::PLACEMENT_SELECTED_NAME),
        item_name,
        Justify::Left,
        437, 18,
        COL_TEXT2);
    draw_manager.draw_text(
        id(ID::PLACEMENT_SELECTED_DESC),
        text,
        Justify::Left,
        437, 36,
        COL_TEXT);
}

enum HPMODE {
    HPMODE_None,
    HPMODE_Hits,
    HPMODE_HP,
};

// The panel as drawn during battle
void LunarBattle::update_panel_battle_new() {
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

    bool redraw = (draw_unit != panel_unit);

    if (draw_unit && (draw_unit->hp != panel_hp)) {
        redraw = true;
    }

    if (draw_unit && (draw_unit->moves_remaining != panel_moves)) {
        redraw = true;
    }

    if (redraw) {
        const char* text = STR_Ground;
        RGB text_col = {0, 0xFF, 0};  // TODO: Check exact colour
        panel_unit = draw_unit;
        panel_hp = draw_unit ? draw_unit->hp : -1;
        panel_moves = draw_unit ? draw_unit->moves_remaining : -1;

        for (int i = 0; i < 20; ++i) {
            draw_manager.draw(hp_ids[i], nullptr);
        }

        draw_manager.draw(id(ID::TOP_PANEL_COVER), nullptr);
        draw_manager.draw(id(ID::TOP_PANEL_PROMOTED), nullptr);

        if (draw_unit) {
            text = draw_unit->name;

            bool our_side = (human_turn && active_unit->defending == draw_unit->defending);

            if (our_side) {
                text_col = {0, 0, 0xFF};  // TODO: Check exact colour
            }

            if (FEATURE(EF_LUNAR_BATTLE_ADVANCED_HP_INFO)) {
                if (human_turn) {
                    Player *player = defender_turn ? defender : aggressor;

                    OfficerQuality offq = OFFQ_Poor;

                    if (player) {
                        offq = defender_turn ? def_officer : agg_officer;
                    }

                    HPMODE hp_mode = HPMODE_None;

                    if (our_side) {
                        hp_mode = HPMODE_HP;
                    } else if (offq != OFFQ_Poor) {
                        hp_mode = ((offq == OFFQ_Good) ? HPMODE_HP : HPMODE_Hits);
                    }

                    const char* hit_img = nullptr;

                    switch (hp_mode) {
                        case HPMODE_Hits:
                            hit_img = (our_side ? IMG_GF4_SBR : IMG_GF4_SBR_RED);
                            break;
                        case HPMODE_HP:
                            hit_img = (our_side ? IMG_BATTLE_SHIELD : IMG_BATTLE_SHIELD_RED);
                            break;
                        default:
                            break;
                    }

                    int dots_to_show = 0;

                    if (hp_mode == HPMODE_Hits) {
                        dots_to_show = draw_unit->hp_initial - draw_unit->hp;
                    }

                    if (hp_mode == HPMODE_HP) {
                        dots_to_show = draw_unit->hp;
                    }

                    if (dots_to_show > 0) {
                        int x_off = 0;
                        int y_off = (dots_to_show <= 10) ? 4 : 0;

                        for (int i = 0; i < dots_to_show; ++i) {
                            if (i == 10) {
                                x_off = 0;
                                y_off += 14;
                            }

                            draw_manager.draw(
                                hp_ids[i],
                                hit_img,
                                {234 + x_off, 35 + y_off, 0, 0, 1, 1});

                            x_off += 10;
                        }
                    }

                    if (is_in_cover(draw_unit)) {
                        draw_manager.draw(
                            id(ID::TOP_PANEL_COVER),
                            IMG_BATTLE_COVER,
                            {372, 38, 1, 0, 1, 1});
                    }

                    if (draw_unit->promoted) {
                        draw_manager.draw(
                            id(ID::TOP_PANEL_PROMOTED),
                            IMG_BATTLE_PROMOTION,
                            {372, 16, 1, 0, 1, 1});
                    }

                    char text[8];
                    snprintf(text, sizeof(text), "%d", draw_unit->moves_remaining);
                    draw_manager.draw_text(id(ID::TOP_PANEL_MOVES), text, Justify::Centre, 414, 14, COL_TEXT);
                    snprintf(text, sizeof(text), "%d", draw_unit->fire_range);
                    draw_manager.draw_text(id(ID::TOP_PANEL_RANGE), text, Justify::Centre, 414, 36, COL_TEXT);
                }
            } else {
                // TODO
                // (we probably won't ever use this case)
                L.warn("Case of EF_LUNAR_BATTLE_NEW_PANELS and not EF_LUNAR_BATTLE_ADVANCED_HP_INFO not implemented");
            }
        } else {
            draw_manager.draw(id(ID::TOP_PANEL_MOVES), nullptr);
            draw_manager.draw(id(ID::TOP_PANEL_RANGE), nullptr);
        }

        draw_manager.draw_text(id(ID::TOP_PANEL_UNITNAME), text, Justify::Left, 232, 13, text_col);
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

    // AI never use their own teleporters
    for (int i = 0; i < n_tele; ++i) {
        if (tele[i].x == x && tele[i].y == y) {
            return false;
        }
    }

    // If it's a mine, return false if we're defending or we're a glider and the mine is discovered
    for (int i = 0; i < n_mines; ++i) {
        if (mines[i].x == x && mines[i].y == y) {
            if (u->defending) {
                return false;
            }
            if (mines[i].discovered && u->type == UNIT_Gli) {
                if (!onein(10)) {
                    return false;
                }
            }
        }
    }

    return true;
}

void LunarBattle::update_reached_far_side(BattleUnit& u) {
    if (!u.reached_far_side) {
        int step = 1;
        int lim = 0;
        if (u.defending) {
            step = -1;
            lim = 0;
        } else {
            step = 1;
            lim = BG_WIDTH - 1;
        }

        if (u.x == lim) {
            u.reached_far_side = true;
        } else {
            // Also consider us to have reached the far side if there are 0 navigable
            // spaces between our target position and the end of the field
            bool found_clear_space = false;
            for (int x = u.x + step; x != (lim + step); x += step) {
                if (ai_can_move_to(&u, x, u.y)) {
                    found_clear_space = true;
                    break;
                }
            }

            if (!found_clear_space) {
                u.reached_far_side = true;
            }
        }
    }
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

Direction LunarBattle::ai_decide_move_direction2() {
    if (!active_unit) {
        return DIR_None;
    }

    BattleUnit &u = *active_unit;

    BattleUnit *tgt = nullptr;
    int min_dist = 999;

    // Try to reduce distance to the closest enemy unit
    for (int i = 0; i < n_units; ++i) {
        BattleUnit &t = units[i];
        if (t.hp > 0 && (t.defending) != u.defending) {
            int dist = abs(t.x - u.x) + abs(t.y - u.y);
            if (dist < min_dist) {
                min_dist = dist;
                tgt = &t;
            }
        }
    }

    if (!tgt) {
        return DIR_None;
    }

    BattleUnit &t = *tgt;
    int dx = t.x - u.x;
    int dy = t.y - u.y;

    // Prefer moving up/down, as moving back looks weird
    if (abs(dy) > 0) {
        int ny = u.y + ((dy > 0) ? 1 : -1);
        if (ai_can_move_to(&u, u.x, ny)) {
            return (dy > 0) ? DIR_Down : DIR_Up;
        }
    }

    if (abs(dx > 0)) {
        int nx = u.x + ((dx > 0) ? 1 : -1);
        if (ai_can_move_to(&u, nx, u.y)) {
            return (dx > 0) ? DIR_Right : DIR_Left;
        }
    }

    return DIR_None;
}

Direction LunarBattle::ai_decide_move_direction_random() {
    if (!active_unit) {
        return DIR_None;
    }

    BattleUnit &u = *active_unit;

    Direction valid_dirs[4];
    int head = 0;

    if (u.last_move != DIR_Down && ai_can_move_to(&u, u.x, u.y-1)) {
        valid_dirs[head++] = DIR_Up;
    }

    if (u.last_move != DIR_Up && ai_can_move_to(&u, u.x, u.y+1)) {
        valid_dirs[head++] = DIR_Down;
    }

    if (u.last_move != DIR_Right && ai_can_move_to(&u, u.x-1, u.y)) {
        valid_dirs[head++] = DIR_Left;
    }

    if (u.last_move != DIR_Left && ai_can_move_to(&u, u.x+1, u.y)) {
        valid_dirs[head++] = DIR_Right;
    }

    if (head == 0) {
        return DIR_None;
    }

    return valid_dirs[rand() % head];
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
        rng_start_x = au->can_shoot_behind ? au->x + range : au->x;
        rng_end_x   = au->x - range - 1;
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

bool LunarBattle::valid_placement(int x, int y) {
    if (unit_at(x, y)) {
        return false;
    }
    for (int j = 0; j < n_mines; ++j) {
        if (mines[j].x == x && mines[j].y == y) {
            return false;
        }
    }
    for (int j = 0; j < n_tele; ++j) {
        if (tele[j].x == x && tele[j].y == y) {
            return false;
        }
    }
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
                img_arrow_up,
                {base_x, base_y - BLK_SZ, 0.5, 0.5, 1, 1});
        }

        // Down
        if (valid_dirs & 0x4) {
            draw_manager.draw(
                id(ID::ARROW_DOWN),
                img_arrow_down,
                {base_x, base_y + BLK_SZ, 0.5, 0.5, 1, 1});
        }

        // Left
        if (valid_dirs & 0x2) {
            draw_manager.draw(
                id(ID::ARROW_LEFT),
                img_arrow_left,
                {base_x - BLK_SZ, base_y, 0.5, 0.5, 1, 1});
        }

        // Right
        if (valid_dirs & 0x1) {
            draw_manager.draw(
                id(ID::ARROW_RIGHT),
                img_arrow_right,
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
                mine_damage = false;
                L.debug("SET ACTIVE UNIT: %s", active_unit->debug_info());
                break;
            }
            --viable_idx;
        }
    }

    active_unit->moves_remaining = active_unit->move;
    active_unit->shots_remaining = active_unit->hp;
    active_unit->last_move = DIR_None;

    return true;
}

void LunarBattle::reset_round() {
    debug_dump_units();
    L.debug("< RESETTING ROUND >");
    for (int i = 0; i < n_units; ++i) {
        units[i].turn_taken = false;
    }
    debug_dump_units();
}

bool LunarBattle::is_in_cover(BattleUnit* u) {
    if (!u || !u->can_use_cover) {
        return false;
    }

    if (cover_at(u->x, u->y)) {
        return true;
    }

    if (FEATURE(EF_WRECKAGE_COVER)) {
        for (int i = 0; i < n_units; ++i) {
            if (units[i].type == UNIT_Gli
             || units[i].type == UNIT_Art
             || units[i].type == UNIT_LBGun
             || units[i].type == UNIT_LBCtl
             || units[i].type == UNIT_AArt) {
                if (units[i].hp <= 0) {
                    if (units[i].x == u->x && units[i].y == u->y) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void LunarBattle::show_info() {
    Planet *p = exostate().get_active_planet();
    Player *defender = exostate().get_player(p->get_owner());
    LunarBattleReport &rpt = ephstate.lunar_battle_report;

    draw_manager.fill(
        id(ID::PANEL),
        {PANEL_X - BORDER, PANEL_Y - BORDER,
        PANEL_W + 2*BORDER, PANEL_H + 60 + 2*BORDER},
        COL_BORDERS);
    draw_manager.fill_pattern(
        id(ID::PANEL_PATTERN),
        {PANEL_X, PANEL_Y,
        PANEL_W, PANEL_H + 60});

    char text[64 + PLANET_MAX_NAME];

    snprintf(text, sizeof(text), "The battle of %s, Status", p->get_name());

    draw_manager.draw_text(
        text,
        Justify::Centre,
        PANEL_X + PANEL_W/2, PANEL_Y + 4,
        COL_TEXT2);

    draw_manager.fill({PANEL_X, PANEL_Y + 28, PANEL_W, 4}, COL_BORDERS);

    RGB col_att = COL_TEXT2;
    RGB col_def = COL_TEXT2;

    if (!defender_turn && aggressor && aggressor->is_human()) {
        col_att = COL_TEXT;
    }

    if (defender_turn && defender->is_human()) {
        col_def = COL_TEXT;
    }

    draw_manager.draw_text(
        "Attackers",
        Justify::Left,
        PANEL_X + 4, PANEL_Y + 36,
        col_att);

    draw_manager.draw_text(
        "Defenders",
        Justify::Left,
        PANEL_X + 172, PANEL_Y + 35,
        col_def);

    snprintf(text, sizeof(text), "Active: %d", rpt.agg_surf.total());
    draw_manager.draw_text(
        text,
        Justify::Left,
        PANEL_X + 4, PANEL_Y + 56,
        COL_TEXT);

    snprintf(text, sizeof(text), "Active: %d", rpt.def_surf.total());
    draw_manager.draw_text(
        text,
        Justify::Left,
        PANEL_X + 172, PANEL_Y + 56,
        COL_TEXT);

    snprintf(text, sizeof(text), "Killed: %d", rpt.agg_lost.total());
    draw_manager.draw_text(
        text,
        Justify::Left,
        PANEL_X + 4, PANEL_Y + 76,
        COL_TEXT);

    snprintf(text, sizeof(text), "Killed: %d", rpt.def_lost.total());
    draw_manager.draw_text(
        text,
        Justify::Left,
        PANEL_X + 172, PANEL_Y + 76,
        COL_TEXT);

    snprintf(text, sizeof(text), "Move: %d", turn);
    draw_manager.draw_text(
        text,
        Justify::Left,
        PANEL_X + 4, PANEL_Y + 102,
        COL_TEXT);

    // Chances are tsa:tsb in PROCb_stat

    int aat = max(1, rpt.agg_surf.total_weighted());
    int adf = max(1, rpt.def_surf.total_weighted());

    int tsa = 0;
    int tsb = 0;

    if (aat > adf) {
        tsa = (int)(100.f*(float)aat/(float)adf) / 100;
        tsb = 1;
    } else {
        tsa = 1;
        tsb = (int)(100.f*(float)adf/(float)aat) / 100;
    }

    if (defender_turn && defender->is_human()) {
        int tmp = tsa;
        tsa = tsb;
        tsb = tmp;
    }

    snprintf(text, sizeof(text), "Our chances are %d : %d", tsa, tsb);
    draw_manager.draw_text(
        text,
        Justify::Left,
        PANEL_X + 4, PANEL_Y + 128,
        COL_TEXT);
}

void LunarBattle::hide_info() {
    draw_manager.draw(id(ID::PANEL_PATTERN), nullptr);
    draw_manager.draw(id(ID::PANEL), nullptr);
}

void LunarBattle::debug_dump_units() {
    L.debug("--- UNIT DUMP ---");
    for (int i = 0; i < n_units; ++i) {
        L.debug(units[i].debug_info());
    }
    L.debug("-----------------");
}

bool BattleUnit::use_alt_aliens = false;

BattleUnit::BattleUnit(BattleUnitType _type) : type(_type) {
    name = STR_None;
    x = 0;
    y = 0;
    tgt_x = 0;
    tgt_y = 0;
    move = 0;
    reached_far_side = false;
    random_moves = 0;
    fire_range = 0;
    fire_rate = 4.f;
    fire_power = 0;
    promotion_category = 0;
    may_be_promoted = false;
    promoted = false;
    shot_sfx = SFX_SHOT;
    move0_sfx = SFX_WALK0;
    move1_sfx = SFX_WALK1;
    move2_sfx = SFX_WALK2;
    moves_remaining = 0;
    shots_remaining = 0;
    can_act = true;
    can_use_cover = false;
    is_alien = false;
    idle = nullptr;
    walk = nullptr;
    fire = nullptr;
    dying = nullptr;
    dead = nullptr;
    move_sfx = nullptr;
    shoot_sfx = nullptr;
    can_shoot_behind = true;
    turn_taken = false;
    teleported = false;
    spr_id_set = false;
    dying_timer = 0;
    last_move = DIR_None;

#ifdef DBG
    dbg_id = -1;
#endif
}

#ifdef DBG
static int BATTLE_UNIT_ID = 0;
#endif

BattleUnit& BattleUnit::init(int _x, int _y) {
#ifdef DBG
    dbg_id = (++BATTLE_UNIT_ID);
#endif

    x = _x;
    y = _y;

    reached_far_side = false;
    random_moves = 0;

    tgt_x = x;
    tgt_y = y;

    turn_taken = false;
    teleported = false;
    dying_timer = 0;

    promotion_category = 0;
    may_be_promoted = false;

    switch (type) {
        case UNIT_Inf:
            name = STR_Inf;
            move = 3;
            fire_range = 3;
            fire_power = 1;
            can_use_cover = true;
            promotion_category = 1;
            may_be_promoted = true;
            if (defending) {
                idle = IMG_GF4_4;
                walk = IMG_GF4_4_2;
                fire = IMG_GF4_16;
                dead = IMG_GF4_10;
            } else {
                idle = IMG_GF4_1;
                walk = IMG_GF4_1_2;
                fire = IMG_GF4_13;
                dead = IMG_GF4_7;
            }
            break;
        case UNIT_Gli:
            name = STR_Gli;
            move = 4;
            fire_range = 4;
            fire_power = 2;
            move0_sfx = SFX_GLIDE_LOW;
            move1_sfx = SFX_GLIDE_MED;
            move2_sfx = SFX_GLIDE_HIGH;
            promotion_category = 2;
            may_be_promoted = true;
            if (defending) {
                // Blue
                idle = IMG_GF4_5;
                walk = IMG_GF4_5;
                fire = IMG_GF4_17;
                dying = IMG_GF4_27;
                dead = IMG_GF4_11;
            } else {
                // Green
                idle = IMG_GF4_2;
                walk = IMG_GF4_2;
                fire = IMG_GF4_14;
                dying = IMG_GF4_26;
                dead = IMG_GF4_8;
            }
            break;
        case UNIT_Art:
            name = STR_Art;
            move = 0;
            fire_range = 7;
            fire_power = 3;
            shot_sfx = SFX_HEAVYSHOT;
            can_shoot_behind = false;
            promotion_category = 3;
            may_be_promoted = true;
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
            shot_sfx = SFX_HEAVYSHOT;
            hp = LUNAR_BASE_GUN_HP;
            defending = true;
            promotion_category = 3;
            idle = IMG_GF4_21;
            walk = IMG_GF4_21;
            fire = IMG_GF4_24;
            dead = IMG_GF4_23;
            break;
        case UNIT_LBCtl:
            name = STR_LBCtl;
            move = 0;
            fire_range = 0;
            hp = LUNAR_BASE_CTL_HP;
            defending = true;
            can_act = false;
            promotion_category = 4;
            idle = IMG_GF4_20;
            walk = IMG_GF4_20;
            fire = IMG_GF4_20;
            dead = IMG_GF4_22;
            break;
        case UNIT_Rebel:
            name = STR_Inf;
            move = 3;
            fire_range = 3;
            fire_power = 1;
            defending = false;
            can_use_cover = true;
            promotion_category = 1;
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
            promotion_category = 1;
            if (use_alt_aliens) {
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
            shot_sfx = SFX_HEAVYSHOT;
            is_alien = true;
            can_shoot_behind = false;
            defending = false;
            promotion_category = 3;
            if (use_alt_aliens) {
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

    hp_initial = hp;

    spr_id = draw_manager.new_sprite_id();
    fire_spr_id = draw_manager.new_sprite_id();
    marker_spr_id = draw_manager.new_sprite_id();
    spr_id_set = true;

    promoted = false;

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

void BattleUnit::release_spr_id() {
    if (spr_id_set) {
        draw_manager.release_sprite_id(marker_spr_id);
        draw_manager.release_sprite_id(fire_spr_id);
        draw_manager.release_sprite_id(spr_id);
        spr_id_set = false;
    }
}

// No validation here - LunarBattle is responsible for correct puppeteering
void BattleUnit::do_move(Direction d) {
    if (d == DIR_Up)    tgt_y--;
    if (d == DIR_Down)  tgt_y++;
    if (d == DIR_Left)  tgt_x--;
    if (d == DIR_Right) tgt_x++;
    last_move = d;
}

bool BattleUnit::try_promote(const BattleUnit& killed_unit, OfficerQuality offq) {
    if (promoted || !may_be_promoted) {
        return false;
    }

    switch (offq) {
        case OFFQ_Poor:
            if (promotion_category < killed_unit.promotion_category) {
                promoted = true;
            }
            break;
        case OFFQ_Average:
            if (promotion_category <= killed_unit.promotion_category) {
                promoted = true;
            }
            break;
        case OFFQ_Good:
            promoted = true;
            break;
        default:
            break;
    }

    return promoted;
}

const char* BattleUnit::get_type_str() {
    switch (type) {
        case UNIT_Inf:
            return "INF";
        case UNIT_Gli:
            return "GLI";
        case UNIT_Art:
            return "ART";
        case UNIT_LBGun:
            return "LBGun";
        case UNIT_LBCtl:
            return "LBCtl";
        case UNIT_Rebel:
            return "Rebel";
        case UNIT_AInf:
            return "AInf";
        case UNIT_AArt:
            return "AArt";
    }

    return "<UNKNOWN>";
}

const char* BattleUnit::debug_info() {
#ifdef DBG
    snprintf(dbg_info, sizeof(dbg_info), "[%d %s %s%s HP:%d (%d,%d) REM_MV:%d REM_SHOT:%d (%s)]",
                                         dbg_id,
                                         defending ? "D" : "A",
                                         get_type_str(),
                                         promoted ? "+" : "",
                                         hp,
                                         x, y,
                                         moves_remaining,
                                         shots_remaining,
                                         turn_taken ? "turn taken" : "turn not taken");
    return dbg_info;
#else
    return "";
#endif
}
