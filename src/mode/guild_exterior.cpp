#include "guild_exterior.h"

#include <cmath>

#include "assetpaths.h"

enum ID {
    LIGHTCAP_TL,
    LIGHTCAP_TR,
    LIGHTCAP_BL,
    LIGHTCAP_BR,
    END,
};

GuildExterior::GuildExterior() : ModeBase("GuildExterior") {
}

void GuildExterior::enter() {
    ModeBase::enter(ID::END);
    draw_manager.draw(IMG_SG1_GUILD);
    draw_manager.save_background();
    draw_manager.show_cursor(true);
    blink_cycle = 0;
}

ExodusMode GuildExterior::update(float delta) {
    SpriteClick click;

    if (draw_manager.fade_active()) {
        return ExodusMode::MODE_None;
    }

    blink_cycle = fmod(blink_cycle + delta, 2.f);

    if (blink_cycle > 0.15) {
        draw_manager.draw(LIGHTCAP_TL, IMG_SG1_LGHT1, { 18,  48, 0.5, 0.5, 1, 1});
        draw_manager.draw(LIGHTCAP_TR, IMG_SG1_LGHT2, {572,  21, 0.5, 0.5, 1, 1});
    } else {
        draw_manager.draw(LIGHTCAP_TL, nullptr);
        draw_manager.draw(LIGHTCAP_TR, nullptr);
    }

    if (blink_cycle < 0.65 || blink_cycle > 0.8) {
        draw_manager.draw(LIGHTCAP_BL, IMG_SG1_LGHT3, {200, 263, 0.5, 0.5, 1, 1});
        draw_manager.draw(LIGHTCAP_BR, IMG_SG1_LGHT4, {474, 360, 0.5, 0.5, 1, 1});
    } else {
        draw_manager.draw(LIGHTCAP_BL, nullptr);
        draw_manager.draw(LIGHTCAP_BR, nullptr);
    }

    return ExodusMode::MODE_None;
}
