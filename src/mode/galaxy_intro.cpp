#include "galaxy_intro.h"

#include "assetpaths.h"

#define GALAXY_INTRO_TIME 8

static const int n_lines = 14;
static const char* text[] = {
    "Our fleet has arrived at the new galaxy.",
    "We have found an institution here",
    "that calls itself the 'Space Guild'.",
    "",
    "Our ships have entered the orbit",
    "of the Space Guild Station,",
    "Our people need somebody who is able",
    "to lead and to protect them in this",
    "time of uncertainty and anxiety.",
    "We have chosen you because you have",
    "proved your skills before.",
    "",
    "We will call you <TITLE> as it is usual",
    "in this galaxy.",
};

enum ID {
    END,
};

GalaxyIntro::GalaxyIntro() : ModeBase("GalaxyIntro") {
    time = 0;
}

void GalaxyIntro::enter() {
    ModeBase::enter(ID::END);
    draw_manager.draw(
        TGT_Secondary,
        IMG_STARTGR_FL6_STARS);
    draw_manager.draw(
        TGT_Secondary,
        IMG_SG1_GUILD,
        {340, 50, 1, 0, 1, 1});
    draw_manager.draw(
        TGT_Secondary,
        IMG_SHIP_FLIPPED,
        {420, 50, 0, 0, 2.4, 2.4});
    DrawArea area = {0, 0, RES_X, 430};
    draw_manager.pixelswap_start(&area);
    time = 0;
}

const int INDENT_X = 40;
const int INDENT_Y = 40;
const int SEP_Y = 20;

ExodusMode GalaxyIntro::update(float delta) {
    if (draw_manager.pixelswap_active()) {
        return ExodusMode::MODE_None;
    }

    PlayerInfo *player = exostate.get_active_player();
    player->intro_seen = true;

    if (input_manager.consume(K_Space)) {
        return ExodusMode::MODE_GalaxyMap;
    }

    time += delta;

    float progress = time / GALAXY_INTRO_TIME;
    if (progress > 1) progress = 1;

    int this_text_idx = (int) (progress * n_lines);
    if (this_text_idx > n_lines - 1) this_text_idx = n_lines - 1;

    unsigned char grey;
    if (this_text_idx <= n_lines) {
        if (this_text_idx > 0) {
            draw_manager.draw_text(
                text[this_text_idx - 1],
                Justify::Left,
                INDENT_X,
                INDENT_Y + (this_text_idx - 1) * SEP_Y,
                {0xD0, 0xD0, 0xD0});
        }
        float this_text_interp = n_lines * fmod(progress, 1.f/n_lines);
        grey = (char)(this_text_interp * 0xD0);
        draw_manager.draw_text(
            text[this_text_idx],
            Justify::Left,
            INDENT_X,
            INDENT_Y + this_text_idx * SEP_Y,
            {grey, grey, grey});
    }

    if (time > 16) {
        player->intro_seen = true;
        return ExodusMode::MODE_GalaxyMap;
    }

    return ExodusMode::MODE_None;
}
