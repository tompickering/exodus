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
    MousePos click_pos;
    return ExodusMode::MODE_None;
}
