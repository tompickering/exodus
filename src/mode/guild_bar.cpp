#include "guild_bar.h"

#include "assetpaths.h"

enum ID {
    END,
};

GuildBar::GuildBar() : ModeBase("GuildBar") {
}

void GuildBar::enter() {
    ModeBase::enter(ID::END);
    draw_manager.draw(IMG_SG3_DUST);
    draw_manager.save_background();
    draw_manager.show_cursor(true);
}

ExodusMode GuildBar::update(float delta) {
    MousePos click_pos;
    return ExodusMode::MODE_None;
}
