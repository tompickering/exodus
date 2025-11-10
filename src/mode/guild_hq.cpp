#include "guild_hq.h"

#include <cmath>

#include "assetpaths.h"

#define DESC_SIZE 13

#define GUILD_COST 5000
#define GUILD_COST_STR "5000"

static const DrawArea AREA_BOT  = {100, 200, 120, 120};
static const DrawArea AREA_DOOR = {200, 200, 160, 120};
static const DrawArea AREA_EXIT = {RES_X - 140, 0, 140, RES_Y};

static const int PANEL_X = 280;
static const int PANEL_Y = 40;
static const int PANEL_W = 332;
static const int PANEL_H = 318;

static const float ENDING_DELAY = 2.f;

static const char* GUILD_RULE_0 = "General protection by the Guild.";
static const char* GUILD_RULE_1 = "No attacking Guild members.";

static const char* DESC_AIM_MULTIPLAYER[DESC_SIZE] = {
    "To be worthy of the",
    "Guildmaster position, all human",
    "lords must be defeated.",
    "",
    "Your only mission is to",
    "survive longer than all your",
    "human opponents.",
    "",
    "Only then will you be worthy",
    "to receive the Guildmaster",
    "position",
    "",
    "",
};

static const char* DESC_AIM_MIGHT[DESC_SIZE] = {
    "To be worthy of the",
    "Guildmaster position, you must",
    "have annihilated all alien lords.",
    "",
    "That is, you have to conquer all",
    "inhabited planets and finally",
    "become the only existing lord in",
    "this galaxy.",
    "",
    "No-one else will be worthy then",
    "to become the new Guildmaster.",
    "",
    "",
};

static const char* DESC_AIM_MONEY[DESC_SIZE] = {
    "To be worthy of the",
    "Guildmaster position, you must",
    "earn a monthly tax sum of",
    "1000 Mega Credits.",
    "",
    "Your financial power will make",
    "you the only one being worthy",
    "to become the new",
    "Guildmaster.",
    "",
    "",
    "",
    "",
};

static const char* DESC_AIM_CIV[DESC_SIZE] = {
    "To be worthy of the",
    "Guildmaster position, you must",
    "rule a gigantic empire of at",
    "least 30 planets.",
    "",
    "Further, you must own all",
    "possible inventions and at",
    "least one artificial planet.",
    "",
    "Ruling such a great empire,",
    "you will be the only one being",
    "worthy to become the new",
    "Guildmaster."
};

enum ID {
    BOT,
    EYES,
    EYES_REF,
    TEXT,
    PANEL,
    PANEL_PATTERN,
    BOT_MISSIONINFO,
    BOT_SGJOIN,
    BOT_SGQUIT,
    BOT_REP,
    BOT_QUIT,
    MEMBER_CHOICE,
    END,
};

GuildHQ::GuildHQ() : ModeBase("GuildHQ") {
    guildbot_active = false;
    guildbot_interp = 0.f;
    eyes_loop = 0;
    ending_delay = 0;
    stage = HQ_Idle;
    fine = 0;
    text_y = 0;
}

void GuildHQ::enter() {
    ModeBase::enter(ID::END);
    draw_manager.draw(ENHANCED() ? IMG_SG2_INS1_NEW : IMG_SG2_INS1);
    draw_manager.save_background();
    draw_manager.show_cursor(true);
    audio_manager.target_music(MUS_GUILD);
    guildbot_active = false;
    guildbot_interp = 0.f;
    eyes_loop = 0;
    ending_delay = 0;
    set_stage(HQ_Idle);
    fine = 0;
    text_y = 0;
    draw_manager.set_selectable(id(ID::BOT_MISSIONINFO));
    draw_manager.set_selectable(id(ID::BOT_SGJOIN));
    draw_manager.set_selectable(id(ID::BOT_SGQUIT));
    draw_manager.set_selectable(id(ID::BOT_REP));
    draw_manager.set_selectable(id(ID::BOT_QUIT));
}

void GuildHQ::set_stage(Stage new_stage) {
    L.debug("Stage: %d -> %d", (int)stage, (int)new_stage);
    stage = new_stage;
}

ExodusMode GuildHQ::update(float delta) {
    bool click = draw_manager.clicked();

    eyes_loop = fmod(eyes_loop + delta, 6);

    if (guildbot_active) {
        eyes_loop = 0;
        guildbot_interp += delta * 2;
        if (guildbot_interp > 1)
            guildbot_interp = 1;
    } else {
        guildbot_interp -= delta * 2;
        if (guildbot_interp < 0)
            guildbot_interp = 0;
    }

    if (guildbot_interp > 0.8) {
        draw_manager.draw(
            id(ID::BOT),
            IMG_SG2_INS3,
            {0, 186, 0, 0, 1, 1});
    } else if (guildbot_interp > 0.3) {
        draw_manager.draw(
            id(ID::BOT),
            IMG_SG2_INS2,
            {0, 192, 0, 0, 1, 1});
    } else {
        draw_manager.draw(id(ID::BOT), nullptr);
        if ((stage != HQ_GuildmasterClaimed) && (stage != HQ_FadeToEnding) &&  eyes_loop > 5.4) {
            draw_manager.draw(id(ID::EYES),     IMG_SG2_EYES,  {185, 234, 0.5, 0.5, 1, 1});
            draw_manager.draw(id(ID::EYES_REF), IMG_SG2_EYES2, {182, 448, 0.5, 0.5, 1, 1});
        } else {
            draw_manager.draw(id(ID::EYES),     nullptr);
            draw_manager.draw(id(ID::EYES_REF), nullptr);
        }
    }

    Player *player = exostate().get_active_player();

    switch (stage) {
        case HQ_Idle:
            if (draw_manager.mouse_in_area(AREA_BOT)) {
                draw_manager.draw_text(
                    id(ID::TEXT),
                    "Infobot",
                    Justify::Left,
                    10, RES_Y - 30,
                    COL_TEXT2);
                if (click) {
                    guildbot_active = true;
                }
            } else if (draw_manager.mouse_in_area(AREA_DOOR)) {
                draw_manager.draw_text(
                    id(ID::TEXT),
                    "Guildmaster Entrance",
                    Justify::Left,
                    10, RES_Y - 30,
                    COL_TEXT2);
                if (click) {
                    draw_panel();

                    draw_manager.draw_text(
                        "Become Guildmaster",
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 4,
                        COL_TEXT2);

                    bool worthy = exostate().mission_complete();
                    bool member = player->is_guild_member();
                    bool punish = player->committed_any_infractions();
                    int line = 60;

                    set_stage(HQ_ClaimGuildmaster);

                    if (worthy) {
                        bool claim = true;
                        draw_manager.draw_text(
                            "You are worthy.",
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 4 + line,
                            COL_TEXT);
                        line += 40;
                        if (!member) {
                            claim = false;
                            draw_manager.draw_text(
                                "But you are no Guild member.",
                                Justify::Left,
                                PANEL_X + 4, PANEL_Y + 4 + line,
                                COL_TEXT);
                            line += 40;
                        }
                        if (punish) {
                            claim = false;
                            draw_manager.draw_text(
                                "Please watch your reputation!",
                                Justify::Left,
                                PANEL_X + 4, PANEL_Y + 4 + line,
                                COL_TEXT);
                        }

                        /*
                         * Orig has "But you have cheated" / chcount - sets 'claim' false
                         * I don't think we need this - you can always cheat an open source game!
                         */

                        if (claim) {
                            set_stage(HQ_GuildmasterClaimed);
                        }
                    } else {
                        draw_manager.draw_text(
                            "You are not worthy yet.",
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 4 + line,
                            COL_TEXT);
                        line += 40;
                        if (!member) {
                            draw_manager.draw_text(
                                "And you are no Guild member.",
                                Justify::Left,
                                PANEL_X + 4, PANEL_Y + 4 + line,
                                COL_TEXT);
                            line += 40;
                        }
                        if (punish) {
                            draw_manager.draw_text(
                                "Please watch your reputation!",
                                Justify::Left,
                                PANEL_X + 4, PANEL_Y + 4 + line,
                                COL_TEXT);
                        }
                    }
                }
            } else if (draw_manager.mouse_in_area(AREA_EXIT)) {
                draw_manager.draw_text(
                    id(ID::TEXT),
                    "Exit",
                    Justify::Left,
                    10, RES_Y - 30,
                    COL_TEXT2);
                if (click) {
                    return ExodusMode::MODE_Pop;
                }
            } else {
                draw_manager.draw(id(ID::TEXT), nullptr);
            }

            if (guildbot_interp == 1) {
                set_stage(HQ_Guildbot);
                draw_panel();
                draw_manager.draw_text(
                    "Welcome to the Space Guild.",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 4,
                    COL_TEXT2);
                draw_manager.draw_text(
                    "What do you desire?",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 24,
                    COL_TEXT2);
            }

            break;
        case HQ_Guildbot:
            draw_manager.draw_text(
                id(ID::BOT_MISSIONINFO),
                "Information about the mission",
                Justify::Left,
                PANEL_X + 4, PANEL_Y + 80,
                COL_TEXT);
            if (player->is_guild_member()) {
                draw_manager.draw_text(
                    id(ID::BOT_SGQUIT),
                    "Quit Guild membership",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 120,
                    COL_TEXT);
            } else {
                draw_manager.draw_text(
                    id(ID::BOT_SGJOIN),
                    "Become a Guild member",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 120,
                    COL_TEXT);
            }
            draw_manager.draw_text(
                id(ID::BOT_REP),
                "View personal reputation",
                Justify::Left,
                PANEL_X + 4, PANEL_Y + 160,
                COL_TEXT);
            draw_manager.draw_text(
                id(ID::BOT_QUIT),
                "Never mind...",
                Justify::Left,
                PANEL_X + 4, PANEL_Y + 220,
                COL_TEXT);

            if (draw_manager.query_click(id(ID::BOT_MISSIONINFO)).id) {
                clear_bot_options();
                draw_panel();
                draw_manager.draw_text(
                    "Information",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 4,
                    COL_TEXT2);

                const char** desc = &DESC_AIM_MIGHT[0];
                if (exostate().multiplayer()) {
                    desc = &DESC_AIM_MULTIPLAYER[0];
                } else {
                    switch (exostate().get_aim()) {
                        case AIM_Might:
                            desc = &DESC_AIM_MIGHT[0];
                            break;
                        case AIM_Money:
                            desc = &DESC_AIM_MONEY[0];
                            break;
                        case AIM_Civilization:
                            desc = &DESC_AIM_CIV[0];
                            break;
                    }
                }

                for (int i = 0; i < DESC_SIZE; ++i) {
                    draw_manager.draw_text(
                        desc[i],
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 44 + i*20,
                        COL_TEXT);
                }

                set_stage(HQ_GuildbotCloseOnClick);
            }

            if (draw_manager.query_click(id(ID::BOT_SGQUIT)).id) {
                clear_bot_options();
                draw_panel();
                draw_manager.draw_text(
                    "Quit membership",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 4,
                    COL_TEXT2);
                // Orig was "Are you willing to leave..." but that seemed a bit strange
                draw_manager.draw_text(
                    "Do you intend to leave the",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 44,
                    COL_TEXT);
                draw_manager.draw_text(
                    "Space Guild?",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 64,
                    COL_TEXT);

                draw_choice();
                set_stage(HQ_GuildbotQuitMembership);
            }

            if (draw_manager.query_click(id(ID::BOT_SGJOIN)).id) {
                clear_bot_options();
                draw_panel();
                draw_manager.draw_text(
                    "Membership",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 4,
                    COL_TEXT2);
                draw_manager.draw_text(
                    "Being a Guild member means:",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 44,
                    COL_TEXT);
                draw_manager.draw_text(
                    GUILD_RULE_0,
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 64,
                    COL_TEXT);
                draw_manager.draw_text(
                    GUILD_RULE_1,
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 84,
                    COL_TEXT);
                const char* text0 = "Guild Membership costs " GUILD_COST_STR;
                const char* text1 = "Mega Credits.";
                if (player->can_afford(GUILD_COST)) {
                    text0 = "Are you willing to pay " GUILD_COST_STR;
                    text1 = "Mega Credits?";
                }
                draw_manager.draw_text(
                    text0,
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 124,
                    COL_TEXT);
                draw_manager.draw_text(
                    text1,
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 144,
                    COL_TEXT);

                draw_choice();
                set_stage(HQ_GuildbotBecomeMember);
            }

            if (draw_manager.query_click(id(ID::BOT_REP)).id) {
                clear_bot_options();
                draw_panel();
                draw_manager.draw_text(
                    "Reputation amongst the lords:",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 4,
                    COL_TEXT2);

                const int rep = player->get_reputation();

                const char* rep_str = "very bad";
                if (rep >= 6) {
                    rep_str = "impeccable";
                } else if (rep >= 3) {
                    rep_str = "very good";
                } else if (rep >= 2) {
                    rep_str = "good";
                } else if (rep >= 1) {
                    rep_str = "bad";
                }

                char text[64];
                snprintf(text, sizeof(text), "Your reputation is %s", rep_str);

                draw_manager.draw_text(
                    text,
                    Justify::Left,
                    PANEL_X + 12, PANEL_Y + 44,
                    COL_TEXT);

                draw_manager.draw_text(
                    "Space Guild Entries:",
                    Justify::Left,
                    PANEL_X + 4, PANEL_Y + 84,
                    COL_TEXT2);

                fine = 0;
                text_y = 124;

                if (player->committed_infraction(INF_TradePlu)) {
                    draw_manager.draw_text(
                        "Dealing with Plutonium",
                        Justify::Left,
                        PANEL_X + 12, PANEL_Y + text_y,
                        COL_TEXT);
                    text_y += 20;
                    fine += 200;
                }

                if (player->committed_infraction(INF_AttackGuildShip)) {
                    draw_manager.draw_text(
                        "Attack of Guild Patrol Ships",
                        Justify::Left,
                        PANEL_X + 12, PANEL_Y + text_y,
                        COL_TEXT);
                    text_y += 20;
                    fine += 500;
                }

                if (player->committed_infraction(INF_BombAttack)) {
                    draw_manager.draw_text(
                        "Bomb Attacks",
                        Justify::Left,
                        PANEL_X + 12, PANEL_Y + text_y,
                        COL_TEXT);
                    text_y += 20;
                    fine += 300;
                }

                if (fine > 0) {
                    snprintf(text, sizeof(text), "Fine: %d MC", fine);
                    draw_manager.draw_text(
                        text,
                        Justify::Left,
                        PANEL_X + 12, PANEL_Y + text_y,
                        COL_TEXT);
                    text_y += 20;

                    if (player->can_afford(fine)) {
                        draw_manager.draw_text(
                            "Do you wish to pay?",
                            Justify::Left,
                            PANEL_X + 12, PANEL_Y + text_y,
                            COL_TEXT);
                        text_y += 20;

                        draw_choice();
                        set_stage(HQ_GuildbotPayFine);
                    } else {
                        set_stage(HQ_GuildbotCloseOnClick);
                    }
                } else {
                    draw_manager.draw_text(
                        "You are clean, Sir.",
                        Justify::Left,
                        PANEL_X + 12, PANEL_Y + text_y,
                        COL_TEXT);
                    set_stage(HQ_GuildbotCloseOnClick);
                }
            }

            if (draw_manager.query_click(id(ID::BOT_QUIT)).id) {
                close_bot_panel();
            }

            break;
        case HQ_GuildbotBecomeMember:
            {
                SpriteClick click = draw_manager.query_click(id(ID::MEMBER_CHOICE));
                if (click.id) {
                    if (click.x < .5f) {
                        if (player->attempt_spend(GUILD_COST, MC_GuildMembership)) {
                            player->set_guild_member(true);
                            draw_panel();
                            draw_manager.draw_text(
                                "Membership",
                                Justify::Left,
                                PANEL_X + 4, PANEL_Y + 4,
                                COL_TEXT2);
                            draw_manager.draw_text(
                                "You are now a Guild Member.",
                                Justify::Left,
                                PANEL_X + 4, PANEL_Y + 44,
                                COL_TEXT);
                            draw_manager.draw_text(
                                "This means:",
                                Justify::Left,
                                PANEL_X + 4, PANEL_Y + 84,
                                COL_TEXT);
                            draw_manager.draw_text(
                                GUILD_RULE_0,
                                Justify::Left,
                                PANEL_X + 4, PANEL_Y + 124,
                                COL_TEXT);
                            draw_manager.draw_text(
                                GUILD_RULE_1,
                                Justify::Left,
                                PANEL_X + 4, PANEL_Y + 164,
                                COL_TEXT);
                        } else {
                            draw_manager.draw_text(
                                "Your money is insufficient.",
                                Justify::Left,
                                PANEL_X + 4, PANEL_Y + 184,
                                COL_TEXT);
                        }
                        set_stage(HQ_GuildbotCloseOnClick);
                    } else {
                        close_bot_panel();
                    }
                }
            }
            break;
        case HQ_GuildbotQuitMembership:
            {
                SpriteClick click = draw_manager.query_click(id(ID::MEMBER_CHOICE));
                if (click.id) {
                    if (click.x < .5f) {
                        // Proceed
                        player->set_guild_member(false);
                        char text[12 + MAX_PLAYER_NAME];
                        snprintf(text, sizeof(text), "Goodbye, %s.", player->get_name());
                        draw_manager.draw_text(
                            text,
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 104,
                            COL_TEXT);
                    } else {
                        draw_manager.draw_text(
                            "A good decision.",
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 104,
                            COL_TEXT);
                    }

                    set_stage(HQ_GuildbotCloseOnClick);
                }
            }
            break;
        case HQ_GuildbotPayFine:
            {
                SpriteClick click = draw_manager.query_click(id(ID::MEMBER_CHOICE));
                if (click.id) {
                    if (click.x < .5f) {
                        // Proceed
                        if (player->attempt_spend(fine, MC_Fine)) {
                            player->clear_infractions();
                            player->adjust_reputation(1);
                            draw_manager.draw_text(
                                "Thank you, Sir.",
                                Justify::Left,
                                PANEL_X + 12, PANEL_Y + text_y,
                                COL_TEXT);
                            set_stage(HQ_GuildbotCloseOnClick);
                        } else {
                            L.error("Should have checked we can afford fine already");
                        }
                    } else {
                        draw_manager.draw_text(
                            "Think about it, Sir.",
                            Justify::Left,
                            PANEL_X + 12, PANEL_Y + text_y,
                            COL_TEXT);
                        set_stage(HQ_GuildbotCloseOnClick);
                    }
                }
            }
            break;
        case HQ_GuildbotCloseOnClick:
            {
                if (draw_manager.clicked()) {
                    close_bot_panel();
                }
            }
            break;
        case HQ_ClaimGuildmaster:
            if (draw_manager.clicked()) {
                draw_manager.draw(id(ID::PANEL_PATTERN), nullptr);
                draw_manager.draw(id(ID::PANEL), nullptr);
                set_stage(HQ_Idle);
            }
            break;
        case HQ_GuildmasterClaimed:
            if (ending_delay > ENDING_DELAY) {
                draw_manager.fade_black(1.2f, 24);
                audio_manager.fade_out(1000);
                set_stage(HQ_FadeToEnding);
            }
            ending_delay += delta;
            break;
        case HQ_FadeToEnding:
            if (!draw_manager.fade_active()) {
                return ExodusMode::MODE_Ending;
            }
            break;
    }

    return ExodusMode::MODE_None;
}

void GuildHQ::draw_panel() {
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

void GuildHQ::draw_choice() {
    draw_manager.fill(
        {PANEL_X + 242, PANEL_Y + PANEL_H - 30, 86, 26},
        COL_BORDERS);
    draw_manager.draw(
        id(ID::MEMBER_CHOICE),
        IMG_BR1_EXPORT,
        {PANEL_X + 4, PANEL_Y + PANEL_H - 4,
         0, 1, 1, 1});
}

void GuildHQ::clear_bot_options() {
    draw_manager.draw(id(ID::BOT_MISSIONINFO), nullptr);
    draw_manager.draw(id(ID::BOT_SGQUIT), nullptr);
    draw_manager.draw(id(ID::BOT_SGJOIN), nullptr);
    draw_manager.draw(id(ID::BOT_REP), nullptr);
    draw_manager.draw(id(ID::BOT_QUIT), nullptr);
    draw_manager.draw(id(ID::MEMBER_CHOICE), nullptr);
}

void GuildHQ::close_bot_panel() {
    clear_bot_options();
    draw_manager.draw(id(ID::PANEL_PATTERN), nullptr);
    draw_manager.draw(id(ID::PANEL), nullptr);
    guildbot_active = false;
    set_stage(HQ_Idle);
}
