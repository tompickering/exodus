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
    bool click = draw_manager.clicked();

    if (click) {
        MousePos pos = input_manager.get_mouse_pos();
        if (pos.x > 490 && pos.x < 540 && pos.y > 220 && pos.y < 280) {
            return ExodusMode::MODE_GuildExterior;
        }
    }

    return ExodusMode::MODE_None;
}
