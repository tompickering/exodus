#include "guild_bar.h"

#include <cmath>

#include "anim.h"
#include "assetpaths.h"

enum ID {
    TALK,
    LIGHT,
    END,
};

GuildBar::GuildBar() : ModeBase("GuildBar") {
}

void GuildBar::enter() {
    ModeBase::enter(ID::END);
    draw_manager.draw(IMG_SG3_DUST);
    draw_manager.save_background();
    draw_manager.show_cursor(true);
    talk_loop = 0.f;
    light_loop = 0.f;
}

Anim talk_anim(
    22,
    IMG_SG3_TALK1,
    IMG_SG3_TALK2,
    IMG_SG3_TALK3,
    IMG_SG3_TALK2,
    IMG_SG3_TALK1,
    IMG_SG3_TALK2,
    IMG_SG3_TALK1,
    IMG_SG3_TALK2,
    IMG_SG3_TALK3,
    IMG_SG3_TALK4,
    IMG_SG3_TALK3,
    IMG_SG3_TALK2,
    IMG_SG3_TALK1,
    IMG_SG3_TALK6,
    IMG_SG3_TALK3,
    IMG_SG3_TALK2,
    IMG_SG3_TALK1,
    IMG_SG3_TALK2,
    IMG_SG3_TALK3,
    IMG_SG3_TALK5,
    IMG_SG3_TALK3,
    IMG_SG3_TALK6
);

ExodusMode GuildBar::update(float delta) {
    bool click = draw_manager.clicked();

    talk_loop = fmod(talk_loop + delta * 0.2, 1);
    light_loop = fmod(light_loop + delta, 6);

    if (click) {
        MousePos pos = input_manager.get_mouse_pos();
        if (pos.x > 490 && pos.x < 540 && pos.y > 220 && pos.y < 280) {
            return ExodusMode::MODE_GuildExterior;
        }
    }

    draw_manager.draw(id(ID::TALK), talk_anim.interp(talk_loop), {225, 246, 0.5, 0.5, 2, 2});

    if (light_loop > 5.7) {
        draw_manager.draw(id(ID::LIGHT), IMG_SG3_LAMP0, {546, 166, 0.5, 0.5, 2, 2});
    } else {
        draw_manager.draw(id(ID::LIGHT), nullptr);
    }

    return ExodusMode::MODE_None;
}
