#include "guild_hq.h"

#include <cmath>

#include "assetpaths.h"

static const DrawArea AREA_BOT  = {100, 200, 120, 120};
static const DrawArea AREA_DOOR = {200, 200, 120, 120};
static const DrawArea AREA_EXIT = {RES_X - 120, 0, 120, RES_Y};

static const int PANEL_X = 280;
static const int PANEL_Y = 40;
static const int PANEL_W = 332;
static const int PANEL_H = 318;

static const float ENDING_DELAY = 2.f;

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
    END,
};

GuildHQ::GuildHQ() : ModeBase("GuildHQ") {
}

void GuildHQ::enter() {
    ModeBase::enter(ID::END);
    draw_manager.draw(IMG_SG2_INS1);
    draw_manager.save_background();
    draw_manager.show_cursor(true);
    audio_manager.target_music(MUS_GUILD);
    guildbot_active = false;
    guildbot_interp = 0.f;
    eyes_loop = 0;
    ending_delay = 0;
    stage = HQ_Idle;
    draw_manager.set_selectable(id(ID::BOT_MISSIONINFO));
    draw_manager.set_selectable(id(ID::BOT_SGJOIN));
    draw_manager.set_selectable(id(ID::BOT_SGQUIT));
    draw_manager.set_selectable(id(ID::BOT_REP));
    draw_manager.set_selectable(id(ID::BOT_QUIT));
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

    Player *player = exostate.get_active_player();

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
                        "Become Guildmaster",
                        Justify::Left,
                        PANEL_X + 4, PANEL_Y + 4,
                        COL_TEXT2);

                    // TODO: Check mission completion (exostate function)
                    bool worthy = false;
                    bool member = player->is_guild_member();
                    // TODO: Check whether we have outstanding violations
                    bool punish = false;
                    int line = 60;

                    stage = HQ_ClaimGuildmaster;

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

                        // TODO: "But you have cheated" / chcount - sets 'claim' false

                        if (claim) {
                            stage = HQ_GuildmasterClaimed;
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
                stage = HQ_Guildbot;
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

            if (draw_manager.query_click(id(ID::BOT_QUIT)).id) {
                draw_manager.draw(id(ID::PANEL_PATTERN), nullptr);
                draw_manager.draw(id(ID::PANEL), nullptr);
                draw_manager.draw(id(ID::BOT_MISSIONINFO), nullptr);
                draw_manager.draw(id(ID::BOT_SGQUIT), nullptr);
                draw_manager.draw(id(ID::BOT_SGJOIN), nullptr);
                draw_manager.draw(id(ID::BOT_REP), nullptr);
                draw_manager.draw(id(ID::BOT_QUIT), nullptr);
                guildbot_active = false;
                stage = HQ_Idle;
            }

            break;
        case HQ_ClaimGuildmaster:
            if (draw_manager.clicked()) {
                draw_manager.draw(id(ID::PANEL_PATTERN), nullptr);
                draw_manager.draw(id(ID::PANEL), nullptr);
                stage = HQ_Idle;
            }
            break;
        case HQ_GuildmasterClaimed:
            if (ending_delay > ENDING_DELAY) {
                draw_manager.fade_black(1.2f, 24);
                audio_manager.fade_out(1000);
                stage = HQ_FadeToEnding;
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
