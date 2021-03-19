#include "lunar_battle_prep.h"

#include "util/value.h"

#include "assetpaths.h"

static const int PANEL_W = 328;
static const int PANEL_H = 120;
static const int PANEL_X = RES_X/2 - (PANEL_W/2);
static const int PANEL_Y = 150;

enum ID {
    PANEL,
    PANEL_PATTERN,
    OPT_COMMAND,
    OPT_WAIT,
    OPT_GROUP_AUTO,
    OPT_GROUP_MAN,
    OPT_PLACE_AUTO,
    OPT_PLACE_MAN,
    END,
};

LunarBattlePrep::LunarBattlePrep() : ModeBase("LunarBattlePrep") {
    stackable = false;
    stage = LBP_Auto;
    stage_started = false;
    initial_pause = 0;
}

void LunarBattlePrep::enter() {
    ModeBase::enter(ID::END);
    set_stage(LBP_Auto);

    stage_started = false;
    initial_pause = 0;

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

    // TODO: Populate this for human players during setup (CPU done)
    b.defender_mines = 0;

    int r = RND(mines_available);
    if (!owner->is_human()) {
        if (owner->can_afford(3) && onein(3)) {
            int to_purchase = 0;
            do {
                to_purchase = min(RND(r) + 3, r);
                /*
                 * FIXME: Orig allows spend of 6MC more than we can afford.
                 * It allows -ve MC - we change the behaviour to ensure
                 * we can afford the full price.
                 * I wonder if the '6' was supposed to be added rather than
                 * subtracted, ensuring that the CPU wouldn't spend so much
                 * as to leave themselves with <6MC.
                 */
                int to_spend = max(to_purchase * mines_price /*- 6*/, 0);
                if (owner->attempt_spend(to_spend)) {
                    b.defender_mines = to_purchase;
                    break;
                }
            } while (RND(30) != 1);
        }
        if (owner->is_guild_member()) {
            b.defender_inf += 5;
            b.defender_gli += 5;
            b.defender_art += 5;
        }
    }

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

    agg_total = b.aggressor_inf + b.aggressor_gli + b.aggressor_art;
    def_total = b.defender_inf  + b.defender_gli  + b.defender_art;

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
        // TODO: Music
        set_stage(LBP_InitialPause);
    }

    draw_manager.set_selectable(id(ID::OPT_COMMAND));
    draw_manager.set_selectable(id(ID::OPT_WAIT));
    draw_manager.set_selectable(id(ID::OPT_GROUP_AUTO));
    draw_manager.set_selectable(id(ID::OPT_GROUP_MAN));
    draw_manager.set_selectable(id(ID::OPT_PLACE_AUTO));
    draw_manager.set_selectable(id(ID::OPT_PLACE_MAN));

    draw_manager.show_cursor(b.human_battle);
}

void LunarBattlePrep::exit() {
    draw_manager.unset_selectable(id(ID::OPT_COMMAND));
    draw_manager.unset_selectable(id(ID::OPT_WAIT));
    draw_manager.unset_selectable(id(ID::OPT_GROUP_AUTO));
    draw_manager.unset_selectable(id(ID::OPT_GROUP_MAN));
    draw_manager.unset_selectable(id(ID::OPT_PLACE_AUTO));
    draw_manager.unset_selectable(id(ID::OPT_PLACE_MAN));
}

ExodusMode LunarBattlePrep::update(float delta) {
    LunarBattleParams &b = ephstate.lunar_battle;

    if (!b.human_battle) {
        // We've done all prep in enter()
        ephstate.set_ephemeral_state(EPH_LunarBattle);
        return ephstate.get_appropriate_mode();
    }

    Planet *p = exostate.get_active_planet();
    Player *owner = exostate.get_player(p->get_owner());
    Player *aggressor = nullptr;
    if (b.aggressor_type == AGG_Player) {
        aggressor = exostate.get_player(b.aggressor_idx);
    }

    bool defending = b.aggressor_type != AGG_Player;

    switch (stage) {
        case LBP_Auto:
            break;
        case LBP_InitialPause:
            if (initial_pause > 0.8f) {
                if (defending) {
                    // Show 'We have spotted X invading units'
                    // only if attacker is CPU
                    set_stage(LBP_InvaderReport);
                } else {
                    set_stage(LBP_CommandOrWait);
                }
            }
            initial_pause += delta;
            break;
        case LBP_InvaderReport:
            if (!stage_started) {
                stage_started = true;

                char text[32];
                int invaders_approx = max(((int)(agg_total/10))*10, 5);
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
                    "We have spotted about",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 4,
                    COL_TEXT);
                snprintf(text, 31, "%d invading units.", invaders_approx);
                draw_manager.draw_text(
                    text,
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 24,
                    COL_TEXT);
                snprintf(text, 31, "%d machines defend", def_total);
                draw_manager.draw_text(
                    text,
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 64,
                    COL_TEXT);
                draw_manager.draw_text(
                    "our planet.",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 84,
                    COL_TEXT);
                break;
            }
            if (draw_manager.clicked()) {
                set_stage(LBP_AllySupport);
            }
            break;
        case LBP_AllySupport:
            if (!stage_started) {
                stage_started = true;
                int m = exostate.get_orig_month();
                int s = 11;
                if      (m <  30) s =  4;
                else if (m <  50) s =  6;
                else if (m < 100) s =  9;
                else              s = 11;

                Player *player;
                for (int i = 0; i < N_PLAYERS; ++i) {
                    player = exostate.get_player(i);
                    if (player == owner) {
                        continue;
                    }
                    // TODO
                }

                draw_manager.fill(
                    id(ID::PANEL),
                    {PANEL_X - BORDER, PANEL_Y - BORDER,
                     PANEL_W + 2*BORDER, PANEL_H + 2*BORDER},
                    COL_BORDERS);
                draw_manager.fill_pattern(
                    id(ID::PANEL_PATTERN),
                    {PANEL_X, PANEL_Y,
                     PANEL_W, PANEL_H});
                // TODO
                draw_manager.draw_text(
                    "WAR ALLY PLACEHOLDER",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 4,
                    COL_TEXT);

                break;
            }

            if (draw_manager.clicked()) {
                set_stage(LBP_GuildSupport);
            }
            break;
        case LBP_GuildSupport:
            if (!owner->is_guild_member()) {
                set_stage(LBP_BuyMines);
                break;
            }

            if (!stage_started) {
                stage_started = true;

                // TODO: Check if these need to be subtracted after battle
                b.aggressor_inf += 5;
                b.aggressor_gli += 5;
                b.aggressor_art += 10;

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
                    "The Space Guild sends",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 4,
                    COL_TEXT);
                draw_manager.draw_text(
                    "20 battle units.",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 24,
                    COL_TEXT);
                break;
            }

            if (draw_manager.clicked()) {
                set_stage(LBP_BuyMines);
            }
            break;
        case LBP_BuyMines:
            if (!stage_started) {
                stage_started = true;

                if (!(owner->get_mc() > 4 && onein(3))) {
                    set_stage(LBP_CommandOrWait);
                    break;
                }

                draw_manager.fill(
                    id(ID::PANEL),
                    {PANEL_X - BORDER, PANEL_Y - BORDER,
                     PANEL_W + 2*BORDER, PANEL_H + 2*BORDER},
                    COL_BORDERS);
                draw_manager.fill_pattern(
                    id(ID::PANEL_PATTERN),
                    {PANEL_X, PANEL_Y,
                     PANEL_W, PANEL_H});
                // TODO
                draw_manager.draw_text(
                    "MINE PURCHASE PLACEHOLDER",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 4,
                    COL_TEXT);

                break;
            }

            if (draw_manager.clicked()) {
                set_stage(LBP_CommandOrWait);
            }
            break;
        case LBP_CommandOrWait:
            if (!stage_started) {
                stage_started = true;

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
                    "Sir, do you wish to...",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 4,
                    COL_TEXT);
                break;
            }

            draw_manager.draw_text(
                id(ID::OPT_COMMAND),
                "...command our troops",
                Justify::Left,
                PANEL_X + 4, PANEL_Y + 44,
                COL_TEXT);
            draw_manager.draw_text(
                id(ID::OPT_WAIT),
                "(...wait for the report)",
                Justify::Left,
                PANEL_X + 4, PANEL_Y + 64,
                COL_TEXT);

            if (draw_manager.query_click(id(ID::OPT_COMMAND)).id) {
                b.auto_battle = false;
                set_stage(LBP_OptionGroupSize);
            } else if (draw_manager.query_click(id(ID::OPT_WAIT)).id) {
                b.auto_battle = true;
                set_stage(LBP_AutoBattleWait);
            }
            break;
        case LBP_OptionGroupSize:
            if (!stage_started) {
                stage_started = true;

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
                    "Please select:",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 4,
                    COL_TEXT);
                break;
            }

            draw_manager.draw_text(
                id(ID::OPT_GROUP_AUTO),
                "Automatic group size",
                Justify::Left,
                PANEL_X + 4, PANEL_Y + 44,
                COL_TEXT);
            draw_manager.draw_text(
                id(ID::OPT_GROUP_MAN),
                "(Manual group size)",
                Justify::Left,
                PANEL_X + 4, PANEL_Y + 64,
                COL_TEXT);

            if (draw_manager.query_click(id(ID::OPT_GROUP_AUTO)).id) {
                set_stage(LBP_OptionPlacement);
            } else if (draw_manager.query_click(id(ID::OPT_GROUP_MAN)).id) {
                set_stage(LBP_GroupSize);
            }
            break;
        case LBP_GroupSize:
            // TODO
            set_stage(LBP_OptionPlacement);
            break;
        case LBP_OptionPlacement:
            if (!stage_started) {
                stage_started = true;

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
                    "Please select:",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 4,
                    COL_TEXT);
                break;
            }

            draw_manager.draw_text(
                id(ID::OPT_PLACE_MAN),
                "Manual unit positioning",
                Justify::Left,
                PANEL_X + 4, PANEL_Y + 44,
                COL_TEXT);
            draw_manager.draw_text(
                id(ID::OPT_PLACE_AUTO),
                "(Automatic unit positioning)",
                Justify::Left,
                PANEL_X + 4, PANEL_Y + 64,
                COL_TEXT);

            if (draw_manager.query_click(id(ID::OPT_PLACE_MAN)).id) {
                if (defending) {
                    b.defender_manual_placement = true;
                } else {
                    b.aggressor_manual_placement = true;
                }
                set_stage(LBP_Close);
            } else if (draw_manager.query_click(id(ID::OPT_PLACE_AUTO)).id) {
                if (defending) {
                    b.defender_manual_placement = false;
                } else {
                    b.aggressor_manual_placement = false;
                }
                set_stage(LBP_Close);
            }
            break;
        case LBP_Close:
            // TODO: Set this based on preferences
            b.aggressor_manual_placement = false;
            b.defender_manual_placement = false;
            ephstate.set_ephemeral_state(EPH_LunarBattle);
            return ephstate.get_appropriate_mode();
        default:
            // TODO: Remove this
            set_stage(LBP_Close);
            break;
    }

    return ExodusMode::MODE_None;
}

void LunarBattlePrep::set_stage(Stage new_stage) {
    stage = new_stage;
    stage_started = false;
}
