#include "guild_exterior.h"

#include "assetpaths.h"

enum ID {
    END,
};

GuildExterior::GuildExterior() : ModeBase("GuildExterior") {
}

void GuildExterior::enter() {
    ModeBase::enter(ID::END);
    draw_manager.draw(IMG_SG1_GUILD);
    draw_manager.save_background();
    draw_manager.show_cursor(true);
}

ExodusMode GuildExterior::update(float delta) {
    SpriteClick click;

    if (draw_manager.fade_active()) {
        return ExodusMode::MODE_None;
    }

    return ExodusMode::MODE_None;
}
