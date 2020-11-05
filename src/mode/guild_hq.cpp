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

static DrawArea REPAIR_AREA = {PANEL_X - BORDER, 0,
                               RES_X - (PANEL_X - BORDER), RES_Y};

enum ID {
    BOT,
    EYES,
    EYES_REF,
    TEXT,
    PANEL,
    BOT_MISSIONINFO,
    BOT_SGJOIN,
    BOT_SGQUIT,
    BOT_REP,
    BOT_QUIT,
    BG_REPAIR,
    END,
};

GuildHQ::GuildHQ() : ModeBase("GuildHQ") {
}

void GuildHQ::enter() {
    ModeBase::enter(ID::END);
    draw_manager.draw(IMG_SG2_INS1);
    draw_manager.save_background();
    draw_manager.show_cursor(true);
    guildbot_active = false;
    guildbot_interp = 0.f;
    eyes_loop = 0;
    stage = HQ_Idle;
    draw_manager.set_selectable(id(ID::BOT_MISSIONINFO));
    draw_manager.set_selectable(id(ID::BOT_SGJOIN));
    draw_manager.set_selectable(id(ID::BOT_SGQUIT));
    draw_manager.set_selectable(id(ID::BOT_REP));
    draw_manager.set_selectable(id(ID::BOT_QUIT));
    draw_manager.set_source_region(id(ID::BG_REPAIR), &REPAIR_AREA);
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
        if (eyes_loop > 5.4) {
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
                draw_manager.save_background(REPAIR_AREA);
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
                draw_manager.draw(
                    id(ID::BG_REPAIR),
                    IMG_SG2_INS1,
                    {REPAIR_AREA.x, REPAIR_AREA.y,
                     0, 0, 1, 1});
                draw_manager.save_background(REPAIR_AREA);
                guildbot_active = false;
                stage = HQ_Idle;
            }

            break;
    }

    return ExodusMode::MODE_None;
}
