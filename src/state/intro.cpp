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

const float MAX_TEXT_TIME = 1;

Intro::Intro() : StateBase("Intro", false), text_idx(0) {
}

void Intro::enter() {
    StateBase::enter();
    stage = Stage::None;
    stage_started = false;
}

void Intro::exit() {
    StateBase::exit();
}

void Intro::update(float delta) {
    float time = timer.get_delta();
    float text_time = text_timer.get_delta();
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
            if (text_time > MAX_TEXT_TIME) {
                if (text_idx == 0) {
                    ++text_idx;
                    text_timer.start();
                } else {
                    next_stage(); return;
                }
            }
            text_brightness = determine_text_brightness(0, text_time);
            draw_manager.draw_text(
                    intro_text[text_idx],
                    Justify::Centre,
                    SCREEN_WIDTH / 2,
                    SCREEN_HEIGHT - 26,
                    {text_brightness, text_brightness, text_brightness});
            break;
        case City:
            if (!stage_started) {
                draw_manager.pixelswap_clear();
                draw_manager.pixelswap_draw(IMG_INTRO_CITY);
                draw_manager.pixelswap_start(nullptr);
                break;
            }

            if (draw_manager.pixelswap_active()) {
                return;
            }

            if (text_idx == 1) {
                ++text_idx;
                text_timer.start();
            }

            if (text_idx >= 1 && text_time > MAX_TEXT_TIME) {
                if (text_idx < 5) {
                    ++text_idx;
                    text_timer.start();
                } else {
                    next_stage(); return;
                }
            }

            text_brightness = determine_text_brightness(0, text_timer.get_delta());
            draw_manager.draw_text(
                    intro_text[text_idx],
                    Justify::Centre,
                    SCREEN_WIDTH / 2,
                    SCREEN_HEIGHT - 26,
                    {text_brightness, text_brightness, text_brightness});
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
    text_timer.start();
}

/*
 * Given then number of seconds that we've been displaying the text,
 * determine how brightly the text should be rendered based on MAX_TEXT_TIME
 */
unsigned char Intro::determine_text_brightness(float started, float now) {
    float progress = (now - started) / MAX_TEXT_TIME;
    if (progress < 0.0 || progress > 1.0) {
        return 0x00;
    } else if (progress < 0.3) {
        float fade_in_progress = progress / 0.3;
        return (int)(fade_in_progress * 0xFF);
    } else if (progress > 0.7) {
        float fade_out_progress = (progress - 0.7) / 0.3;
        return (int)((1.f - fade_out_progress) * 0xFF);
    } else {
        return 0xFF;
    }
}
