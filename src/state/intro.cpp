#include "intro.h"

#include <cmath>

#include "../assetpaths.h"
#include "../draw/draw.h"
#include "../shared.h"

static const char* intro_text[] = {
    "It is the year 3011.",
    "Civilization has failed.",
    "The last living creatures on Earth",
    "starve in rotten cellars",
    "or fight against each other",
    "for the last food rations.",
    "But a few bosses of once great concerns",
    "still live in wealth,",
    "protected by killer commandos.",
    "These mighty men have united",
    "to fulfil a secret plan.",
    "A gigantic starship has been built by them",
    "to find a new world to civilize",
    "and leave the homeless people behind.",
    "But a group of outlaws owns secret information",
    "that may help to steal this vessel",
    "and escape from Earth",
    "together with many other homeless people.",
    "Enter the secret code at offset",
    "Access denied.",
    "Access admitted.",
    "The people have succeeded.",
    "So they have a chance to escape from this planet",
    "and find a new home",
    "out in space.",
    "This is the beginning of a long",
    "and dangerous journey.",
};

const int text_time = 6;

Intro::Intro() : StateBase("Intro", false), text_idx(0) {
}

void Intro::enter() {
    StateBase::enter();
    stage = Stage::None;
    stage_started = false;
    timer.start();
}

void Intro::exit() {
    StateBase::exit();
}

void Intro::update(float delta) {
    float time = timer.get_delta();
    unsigned char text_brightness = 0;

    switch(stage) {
        case None:
            next_stage(); return;
            break;
        case Init:
            next_stage(); return;
            break;
        case Artex:
            next_stage(); return;
            break;
        case Earth:
            if (!stage_started) {
                draw_manager.draw(IMG_INTRO_EARTH);
            }
            text_idx = time < text_time ? 0 : 1;
            text_brightness = (int)((fmod(time, (float)text_time) * 0xFF) / text_time);
            draw_manager.draw_text(
                    intro_text[text_idx],
                    Justify::Centre,
                    SCREEN_WIDTH / 2,
                    SCREEN_HEIGHT - 26,
                    {text_brightness, text_brightness, text_brightness});
            if (time > text_time * 2) {
                next_stage(); return;
            }
            break;
        case City:
            if (!stage_started) {
                draw_manager.draw(IMG_INTRO_CITY);
                draw_manager.save_background();
            }
            if (time > 2) {
                next_stage(); return;
            }
            break;
        default:
            break;
    }

    stage_started = true;
}

void Intro::next_stage() {
    if (stage != Stage::End) {
        stage = (Stage)((int)stage + 1);
        stage_started = false;
    }
    timer.start();
}
