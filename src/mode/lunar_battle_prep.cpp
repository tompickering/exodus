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
    MINES_TO_BUY,
    MINES_ADJUST,
    OPT_COMMAND,
    OPT_WAIT,
    OPT_GROUP_AUTO,
    OPT_GROUP_MAN,
    OPT_GROUP_NUM,
    OPT_GROUP_ADJUST,
    OPT_PLACE_AUTO,
    OPT_PLACE_MAN,
    END,
};

LunarBattlePrep::LunarBattlePrep() : ModeBase("LunarBattlePrep") {
    stage = LBP_Auto;
    stage_started = false;
    pause = 0;
}

void LunarBattlePrep::enter() {
    ModeBase::enter(ID::END);
    set_stage(LBP_Auto);

    stage_started = false;
    pause = 0;

    LunarBattleParams &b = ephstate.lunar_battle;
    LunarBattleReport &rpt = ephstate.lunar_battle_report;

    Planet *p = exostate.get_active_planet();

    if (ephstate.get_ephemeral_state() == EPH_LunarBattleReport) {
        // LunarBattlePrep does NOT clear ephemeral state - that's the
        // responsibility of whatever invoked LunarBattlePrep.
        L.info("--- BATTLE REPORT ---");
        const char *inv = "";
        switch (b.aggressor_type) {
            case AGG_Rebels:
                inv = "Rebels";
                break;
            case AGG_Aliens:
                inv = "Aliens";
                break;
            case AGG_Player:
                {
                    Player *p_agg = exostate.get_player(b.aggressor_idx);
                    inv = p_agg->get_full_name();
                }
                break;
        }

        Player *owner = exostate.get_player(p->get_owner());
        L.info("%s ATTACKING %s (%s)", inv, p->get_name(), owner->get_full_name());
        L.info("");
        L.info("AGG INIT: %d %d %d", rpt.agg_init.inf, rpt.agg_init.gli, rpt.agg_init.art);
        L.info("DEF INIT: %d %d %d", rpt.def_init.inf, rpt.def_init.gli, rpt.def_init.art);
        L.info("AGG LOST: %d %d %d", rpt.agg_lost.inf, rpt.agg_lost.gli, rpt.agg_lost.art);
        L.info("DEF LOST: %d %d %d", rpt.def_lost.inf, rpt.def_lost.gli, rpt.def_lost.art);
        L.info("AGG SURF: %d %d %d", rpt.agg_surf.inf, rpt.agg_surf.gli, rpt.agg_surf.art);
        L.info("DEF SURF: %d %d %d", rpt.def_surf.inf, rpt.def_surf.gli, rpt.def_surf.art);
        L.info("");
        L.info("WON: %s", rpt.aggressor_won ? "AGG" : "DEF");
        L.info("---------------------");
        set_stage(LBP_Conclude);
        return;
    }

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

    b.aggressor_tele = (b.aggressor_type == AGG_Player) ? 3 : 0;

    // Get numbers of defending units
    p->get_army(b.defender_inf, b.defender_gli, b.defender_art);
    p->clear_army();

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
            {
                const Freight &f = aggressor->get_fleet().freight;
                b.aggressor_inf = aggressor->transfer_inf(-f.infantry);
                b.aggressor_gli = aggressor->transfer_gli(-f.gliders);
                b.aggressor_art = aggressor->transfer_art(-f.artillery);
            }
            break;
        case AGG_Rebels:
            // TODO
            break;
        case AGG_Aliens:
            b.aggressor_inf = RND(m/2);
            b.aggressor_art = RND(m);
            break;
    }

    // TODO: I think this only happens for humans
    if (get_def_total() == 0) {
        b.defender_inf = 1;
    }

    // Default group sizes - humans can override.
    b.aggressor_group_size = 12;
    if (get_agg_total() <= 200) b.aggressor_group_size = 8;
    if (get_agg_total() <=  90) b.aggressor_group_size = 5;
    if (get_agg_total() <=  30) b.aggressor_group_size = 3;

    b.defender_group_size = 12;
    if (get_def_total() <= 200) b.defender_group_size = 8;
    if (get_def_total() <=  90) b.defender_group_size = 5;
    if (get_def_total() <=  30) b.defender_group_size = 3;

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

    Planet *p = exostate.get_active_planet();
    Player *owner = exostate.get_player(p->get_owner());
    Player *aggressor = nullptr;
    if (b.aggressor_type == AGG_Player) {
        aggressor = exostate.get_player(b.aggressor_idx);
    }

    bool defending = !(aggressor && aggressor->is_human());

    switch (stage) {
        case LBP_Auto:
            if (!b.human_battle) {
                // We've done all prep in enter()
                ephstate.set_ephemeral_state(EPH_LunarBattle);
                return ephstate.get_appropriate_mode();
            }
            break;
        case LBP_InitialPause:
            if (pause > 0.8f) {
                if (defending) {
                    // Show 'We have spotted X invading units'
                    // only if attacker is CPU
                    set_stage(LBP_InvaderReport);
                } else {
                    set_stage(LBP_CommandOrWait);
                }
            }
            pause += delta;
            break;
        case LBP_InvaderReport:
            if (!defending) {
                L.error("LBP_InvaderReport invalid for attacker");
                set_stage(LBP_CommandOrWait);
            }

            if (!stage_started) {
                stage_started = true;

                char text[32];
                int invaders_approx = max(((int)(get_agg_total()/10))*10, 5);
                draw_panel();
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
                snprintf(text, 31, "%d machines defend", get_def_total());
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
            if (!defending) {
                L.error("LBP_AllySupport invalid for attacker");
                set_stage(LBP_CommandOrWait);
            }

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

                draw_panel();
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
            if (!defending) {
                L.error("LBP_GuildSupport invalid for attacker");
                set_stage(LBP_CommandOrWait);
            }

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

                draw_panel();
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
            if (!defending) {
                L.error("LBP_BuyMines invalid for attacker");
                set_stage(LBP_CommandOrWait);
            }

            if (!stage_started) {
                stage_started = true;

                if (!(owner->get_mc() > 4 && onein(3))) {
                    set_stage(LBP_CommandOrWait);
                    break;
                }

                char text[32];

                draw_panel();
                draw_manager.draw_text(
                    "A private concern offers",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 4,
                    COL_TEXT);
                snprintf(text, sizeof(text), "%d AntiGrav mines for", mines_available);
                draw_manager.draw_text(
                    text,
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 24,
                    COL_TEXT);
                snprintf(text, sizeof(text), "%dMC each.", mines_price);
                draw_manager.draw_text(
                    text,
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 44,
                    COL_TEXT);
                draw_manager.draw_text(
                    "Buy:",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 84,
                    COL_TEXT);
                draw_manager.draw(
                    id(ID::MINES_ADJUST),
                    IMG_BR3_EXPORT2,
                    {PANEL_X + PANEL_W - 4, PANEL_Y + PANEL_H - 4,
                     1, 1,
                     1, 1});

                mines_to_buy = 0;
                break;
            }

            {
                char n[3];
                snprintf(n, sizeof(n), "%d", mines_to_buy);
                draw_manager.draw_text(
                    id(ID::MINES_TO_BUY),
                    n,
                    Justify::Left,
                    PANEL_X + 50, PANEL_Y + 84,
                    COL_TEXT2);

                // TODO: Orig in PROCb_ready looks like it can allow us to
                // buy one more mine than we can afford - check this
                SpriteClick clk = draw_manager.query_click(id(ID::MINES_ADJUST));
                if (clk.id) {
                    if (clk.y > .5f) {
                        if (owner->attempt_spend(mines_price * mines_to_buy)) {
                            b.defender_mines = mines_to_buy;
                            set_stage(LBP_CommandOrWait);
                        }
                    } else if (clk.x < .5f) {
                        mines_to_buy = max(mines_to_buy-1, 0);
                    } else {
                        mines_to_buy = min(mines_to_buy+1, mines_available);
                    }
                }
            }

            break;
        case LBP_CommandOrWait:
            if (!stage_started) {
                stage_started = true;

                draw_panel();
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

                draw_panel();
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
            {
                int &size = defending ? b.defender_group_size
                                      : b.aggressor_group_size;

                char n[3];
                snprintf(n, sizeof(n), "%d", size);

                if (!stage_started) {
                    stage_started = true;

                    draw_panel();
                    draw_manager.draw_text(
                        "Group size recommended:",
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 4,
                        COL_TEXT);
                    draw_manager.draw_text(
                        n,
                        Justify::Left,
                        PANEL_X + 240, PANEL_Y + 4,
                        COL_TEXT);
                    draw_manager.draw_text(
                        "Your choice:",
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 44,
                        COL_TEXT);
                    draw_manager.draw_text(
                        "(Min: 2 Max: 20)",
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 84,
                        COL_TEXT);
                    draw_manager.draw(
                        id(ID::OPT_GROUP_ADJUST),
                        IMG_BR3_EXPORT2,
                        {PANEL_X + PANEL_W - 4, PANEL_Y + PANEL_H - 4,
                         1, 1,
                         1, 1});
                        break;
                }

                draw_manager.draw_text(
                    id(ID::OPT_GROUP_NUM),
                    n,
                    Justify::Left,
                    PANEL_X + 130, PANEL_Y + 44,
                    COL_TEXT2);

                SpriteClick clk = draw_manager.query_click(id(ID::OPT_GROUP_ADJUST));
                if (clk.id) {
                    if (clk.y > .5f) {
                        set_stage(LBP_OptionPlacement);
                    } else if (clk.x < .5f) {
                        size = max(size-1, 2);
                    } else {
                        size = min(size+1, 20);
                    }
                }
            }
            break;
        case LBP_OptionPlacement:
            if (!stage_started) {
                stage_started = true;

                draw_panel();
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
                set_stage(LBP_StartBattle);
            } else if (draw_manager.query_click(id(ID::OPT_PLACE_AUTO)).id) {
                if (defending) {
                    b.defender_manual_placement = false;
                } else {
                    b.aggressor_manual_placement = false;
                }
                set_stage(LBP_StartBattle);
            }
            break;
        case LBP_AutoBattleWait:
            if (!stage_started) {
                stage_started = true;
                draw_manager.draw(TGT_Secondary, IMG_BATTLE);
                draw_manager.pixelswap_start(nullptr);
                pause = 0;
            }

            if (draw_manager.pixelswap_active()) {
                break;
            }

            if (pause > 0.8) {
                set_stage(LBP_AutoBattleWait1);
                break;
            }

            pause += delta;

            break;
        case LBP_AutoBattleWait1:
            if (!stage_started) {
                stage_started = true;

                draw_panel();

                char text[15 + PLANET_MAX_NAME];
                snprintf(text, sizeof(text), "The battle of %s", p->get_name());
                draw_manager.draw_text(
                    text,
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 4,
                    COL_TEXT);
                draw_manager.draw_text(
                    "has begun...",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 24,
                    COL_TEXT);

                pause = 0;

                break;
            }

            if (pause > 4.f) {
                set_stage(LBP_AutoBattleWait2);
            }

            pause += delta;

            break;
        case LBP_AutoBattleWait2:
            if (!stage_started) {
                stage_started = true;
                draw_manager.draw(TGT_Secondary, IMG_BATTLE);
                draw_manager.pixelswap_start(nullptr);
                pause = 0;
            }

            if (draw_manager.pixelswap_active()) {
                break;
            }

            if (pause > 2.f) {
                set_stage(LBP_StartBattle);
                break;
            }

            pause += delta;

            break;
        case LBP_StartBattle:
            ephstate.set_ephemeral_state(EPH_LunarBattle);
            return ephstate.get_appropriate_mode();
        case LBP_Conclude:
            {
                // Handle battle outcome
                LunarBattleReport &rpt = ephstate.lunar_battle_report;

                Planet *p = exostate.get_active_planet();
                int cap = p->get_resource_cap();

                // Station surviving defending units back on the planet
                p->adjust_army(rpt.def_surf.inf, rpt.def_surf.gli, rpt.def_surf.art);

                if (b.aggressor_type == AGG_Player) {
                    Player *agg = exostate.get_player(b.aggressor_idx);
                    int i = rpt.agg_surf.inf;
                    int g = rpt.agg_surf.gli;
                    int a = rpt.agg_surf.art;

                    if (rpt.aggressor_won) {
                        // Transfer planet
                        p->set_owner(b.aggressor_idx);

                        // Of the units still on the surface, station as many as possible
                        if (a > cap) a += agg->transfer_art(a - cap);
                        if (g > cap) g += agg->transfer_gli(g - cap);
                        if (i > cap) i += agg->transfer_inf(i - cap);
                        p->clear_army();
                        p->adjust_army(i, g, a);
                    } else {
                        // Transfer any surviving units back to the fleet
                        agg->transfer_art(a);
                        agg->transfer_gli(g);
                        agg->transfer_inf(i);
                    }
                }

                // If this is an auto-battle involving a human, we have a
                // results sequence to present. Otherwise, pop immediately.
                if (b.auto_battle && b.human_battle) {
                    set_stage(LBP_AutoBattleConclude);
                } else {
                    return ExodusMode::MODE_Pop;
                }
            }
            break;
        case LBP_AutoBattleConclude:
            if (!stage_started) {
                stage_started = true;

                draw_manager.show_cursor(true);
                LunarBattleReport &rpt = ephstate.lunar_battle_report;
                bool won = (defending ^ rpt.aggressor_won);
                // TODO: Music

                draw_panel();

                // TODO
                draw_manager.draw_text(
                    won ? "WON" : "LOST",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 4,
                    COL_TEXT);
                break;
            }

            if (draw_manager.clicked()) {
                return ExodusMode::MODE_Pop;
            }
            break;
    }

    return ExodusMode::MODE_None;
}

void LunarBattlePrep::set_stage(Stage new_stage) {
    stage = new_stage;
    stage_started = false;
}

int LunarBattlePrep::get_agg_total() {
    LunarBattleParams &b = ephstate.lunar_battle;
    return b.aggressor_inf + b.aggressor_gli + b.aggressor_art;
}

int LunarBattlePrep::get_def_total() {
    LunarBattleParams &b = ephstate.lunar_battle;
    // FIXME: Include lunar base guns here? (4*LUNAR_BASE_GUN_HP)
    return b.defender_inf  + b.defender_gli  + b.defender_art;
}

void LunarBattlePrep::draw_panel() {
    draw_manager.refresh_sprite_id(id(ID::PANEL));
    draw_manager.refresh_sprite_id(id(ID::PANEL_PATTERN));
    draw_manager.fill(
        id(ID::PANEL),
        {PANEL_X - BORDER, PANEL_Y - BORDER,
         PANEL_W + 2*BORDER, PANEL_H + 2*BORDER},
        COL_BORDERS);
    draw_manager.fill_pattern(
        id(ID::PANEL_PATTERN),
        {PANEL_X, PANEL_Y,
         PANEL_W, PANEL_H});
}
