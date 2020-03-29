#include "guild_hq.h"

#include "assetpaths.h"

enum ID {
    END,
};

GuildHQ::GuildHQ() : ModeBase("GuildHQ") {
}

void GuildHQ::enter() {
    ModeBase::enter(ID::END);
    draw_manager.draw(IMG_SG2_INS1);
    draw_manager.save_background();
    draw_manager.show_cursor(true);
}

ExodusMode GuildHQ::update(float delta) {
    bool click = draw_manager.clicked();

    if (click) {
        MousePos pos = input_manager.get_mouse_pos();
        if (pos.x > 100 && pos.x < 220 && pos.y > 200 && pos.y < 320) {
            L.debug("GuildBot clicked");
        } else if (pos.x > RES_X - 120) {
            return ExodusMode::MODE_GuildExterior;
        }
    }

    return ExodusMode::MODE_None;
}
