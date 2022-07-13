#include "lunar_battle_prep.h"

#include "util/iter.h"
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

    war_ally_idx = 0;
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
    // Orig was 'if attacker is human', but for multiplayer, we don't want
    // the defending player to have mines bought for them - so this should
    // only happen if the defender is a CPU.
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
    }
    // TODO: Check this condition. Orig was 'If attacker is human'.
    // I think the idea is if attacker is not human, then defender is human,
    // and therefore will have received guild support in LBP_GuildSupport.
    // This doesn't account for multiplayer however, where the ACTIVE player
    // is the aggressor - the defending player doesn't get LBP_GuildSupport.
    // Criterion should probably be owner is CPU OR owner is human but not active.
    // Even then, they should probably get the +10 artillery if human.
    if (!owner->is_human()) {
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
            b.aggressor_inf = RND(10*p->get_n_cities());
            if (p->get_army_size() > b.aggressor_inf*3) {
                b.aggressor_inf *= 2;
            }
            break;
        case AGG_Aliens:
            b.aggressor_inf = RND(max(m/2, 1));
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
        audio_manager.target_music(mpart2mus(16));
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
    int owner_idx = p->get_owner();
    Player *owner = exostate.get_player(owner_idx);
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
                war_ally_idx = 0;
                set_stage(LBP_AllySupport);
            }
            break;
        case LBP_AllySupport:
            {
                bool presented_dialogue = false;

                if (!defending) {
                    L.error("LBP_AllySupport invalid for attacker");
                    set_stage(LBP_CommandOrWait);
                }

                int m = exostate.get_orig_month();
                int s = 11;
                if      (m <  30) s =  4;
                else if (m <  50) s =  6;
                else if (m < 100) s =  9;
                else              s = 11;

                Player *player;
                for (; war_ally_idx < N_PLAYERS; ++war_ally_idx) {
                    int &i = war_ally_idx;
                    player = exostate.get_player(i);

                    if (player == owner || !(player->is_participating())) {
                        continue;
                    }

                    /*
                     * TODO: What should we do about human players here?
                     * Orig makes no special case - but does read AI flags,
                     * which don't really apply to other humans.
                     */
                    if (player->is_human()) {
                        continue;
                    }

                    if (!exostate.has_alliance(owner_idx, i, ALLY_War)) {
                        continue;
                    }

                    if (player->get_flag(4) == AI_Hi) {
                        L.debug("%s offering no support (AI flags)", player->get_name());
                        continue;
                    }

                    if (player->is_hostile_to(owner_idx)) {
                        L.debug("%s offering no support (hostile)", player->get_name());
                        continue;
                    }

                    int support_inf = 0;
                    int support_gli = 0;
                    int support_art = 0;

                    /*
                     * FIXME: As in orig, but this is consistently biased towards the
                     * same planets, as they are always iterated in the same order. It
                     * would be nicer to randomise or select planet dynamically based
                     * on heuristics.
                     */
                    for (PlanetIterator piter(i); !piter.complete(); ++piter) {
                        int supp_inf = 0;
                        int supp_gli = 0;
                        int supp_art = 0;

                        int inf, gli, art;
                        piter.get()->get_army(inf, gli, art);

                        if (inf > s*2) {
                            supp_inf = RND(s-1);
                        }

                        if (gli > s*3) {
                            supp_gli = RND(s-1);
                        }

                        if (art > s*4) {
                            supp_art = RND(s-1);
                        }

                        support_inf += supp_inf;
                        support_gli += supp_gli;
                        support_art += supp_art;

                        piter.get()->adjust_army(-supp_inf, -supp_gli, -supp_art);

                        if (!onein(3)) {
                            break;
                        }
                    }

                    char text[64];

                    draw_panel();

                    int total = support_inf + support_gli + support_art;
                    if (total > 0) {
                        draw_manager.draw_text(
                            "As a support, war-ally",
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 4,
                            COL_TEXT);

                        snprintf(text, sizeof(text), "%s has sent you %d", player->get_full_name(), total);
                        draw_manager.draw_text(
                            text,
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 24,
                            COL_TEXT);

                        draw_manager.draw_text(
                            "units.",
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 44,
                            COL_TEXT);

                        /*
                         * TODO: Check we get to keep these after the battle.
                         * I believe so - we simply increment d_army and there is no
                         * state to record this ever happened.
                         */
                        b.defender_inf += support_inf;
                        b.defender_gli += support_gli;
                        b.defender_art += support_art;
                    } else {
                        snprintf(text, sizeof(text), "Your war-ally %s", player->get_full_name());
                        draw_manager.draw_text(
                            text,
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 4,
                            COL_TEXT);
                        draw_manager.draw_text(
                            "does not support you.",
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 24,
                            COL_TEXT);
                    }

                    presented_dialogue = true;

                    break;
                }

                if (presented_dialogue) {
                    // Ensure we resume from the next player when we return following click
                    ++war_ally_idx;
                    set_stage(LBP_AllySupportWaitForClick);
                } else {
                    war_ally_idx = 0;
                    set_stage(LBP_GuildSupport);
                }
            }
            break;
        case LBP_AllySupportWaitForClick:
            if (draw_manager.clicked()) {
                set_stage(LBP_AllySupport);
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

                if (b.aggressor_type == AGG_Rebels && rpt.aggressor_won) {
                    int i = rpt.agg_surf.inf;
                    p->reset_unrest();
                    p->clear_army();
                    p->adjust_army(max(0, i), 0, 0);
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
                batrpt_draw(true);
                break;
            }

            if (draw_manager.clicked()) {
                if (!batrpt_draw(false)) {
                    return ExodusMode::MODE_Pop;
                }
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
