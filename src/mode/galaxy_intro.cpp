#include "galaxy_intro.h"

#include "assetpaths.h"

#define GALAXY_INTRO_TIME 5

static const char* text[] = {
    "Our fleet has arrived at the new galaxy.",
    "We have found an institution here",
    "that calls itself the 'Space Guild'.",
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
    progress = 0;
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
        IMG_INTRO_SH3_CRUISER1,
        {420, 50, 0, 0, 2.4, 2.4});
    draw_manager.pixelswap_start();
    progress = 0;
}

ExodusMode GalaxyIntro::update(float delta) {
    if (draw_manager.pixelswap_active()) {
        return ExodusMode::MODE_None;
    }

    progress += delta / GALAXY_INTRO_TIME;

    //int this_text_idx = progress / 13;

    return ExodusMode::MODE_None;
}
