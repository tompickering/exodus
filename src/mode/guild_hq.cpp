#include "guild_hq.h"

#include "assetpaths.h"

enum ID {
    BOT,
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
}

ExodusMode GuildHQ::update(float delta) {
    bool click = draw_manager.clicked();

    if (click) {
        MousePos pos = input_manager.get_mouse_pos();
        if (pos.x > 100 && pos.x < 220 && pos.y > 200 && pos.y < 320) {
            guildbot_active = true;
        } else if (pos.x > RES_X - 120) {
            return ExodusMode::MODE_GuildExterior;
        }
    }

    if (guildbot_active) {
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
    }

    return ExodusMode::MODE_None;
}
