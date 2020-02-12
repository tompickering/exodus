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

float CITY_SHIP_START      = 5.f;
float CITY_SHIP_STOP       = 17.f;
float CITY_SHIP_MAX_SCALE  = 1.6f;
float CITY_SHIP_SHOT_1     = 22.0f;
float CITY_SHIP_SHOT_2     = 22.2f;
float CITY_SHIP_SHOT_3     = 24.5f;
float CITY_SHIP_SHOT_4     = 24.7f;
float CITY_SHIP_SHOT_TIME  = 0.1f;
float CITY_SHIP_START_Y    = 260;
float CITY_SHIP_END_Y      = 210;

float SP_SHIP_STOP         = 8.f;
float SP_SHIP_MAX_SCALE    = 1.4f;

const float MAX_TEXT_TIME = 3.8;

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
                break;
            }

            if (text_time > MAX_TEXT_TIME) {
                if (text_idx == 0) {
                    ++text_idx;
                    text_timer.start();
                } else {
                    next_stage(); return;
                }
            }

            draw_text();
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

            if (time > CITY_SHIP_START) {
                float fly_progress = (time - CITY_SHIP_START) / (CITY_SHIP_STOP - CITY_SHIP_START);
                fly_progress = fly_progress > 1 ? 1 : fly_progress;
                float ship_scale = fly_progress * CITY_SHIP_MAX_SCALE;
                const char *ship = IMG_INTRO_SH1_SHIP;
                if (   (time > CITY_SHIP_SHOT_1 && time < CITY_SHIP_SHOT_1 + CITY_SHIP_SHOT_TIME)
                    || (time > CITY_SHIP_SHOT_2 && time < CITY_SHIP_SHOT_2 + CITY_SHIP_SHOT_TIME)
                    || (time > CITY_SHIP_SHOT_3 && time < CITY_SHIP_SHOT_3 + CITY_SHIP_SHOT_TIME)
                    || (time > CITY_SHIP_SHOT_4 && time < CITY_SHIP_SHOT_4 + CITY_SHIP_SHOT_TIME)) {
                    ship = IMG_INTRO_SH1_SHIP2;
                }
                ship_scale = ship_scale < CITY_SHIP_MAX_SCALE ? ship_scale : CITY_SHIP_MAX_SCALE;

                int ship_y = CITY_SHIP_START_Y + (int)((CITY_SHIP_END_Y - CITY_SHIP_START_Y) * fly_progress);

                draw_manager.draw(ship, {10 + SCREEN_WIDTH / 2, ship_y, 0.5, 0.5, ship_scale});
            }

            if (text_idx >= 1 && text_time > MAX_TEXT_TIME) {
                if (text_idx < 8) {
                    ++text_idx;
                    text_timer.start();
                } else {
                    next_stage(); return;
                }
            }

            draw_text();
            break;
        case Starport:
            if (!stage_started) {
                draw_manager.draw(IMG_INTRO_STARPORT);
                break;
            }

            if (text_idx >= 8 && text_time > MAX_TEXT_TIME) {
                if (text_idx < 13) {
                    ++text_idx;
                    text_timer.start();
                } else {
                    next_stage(); return;
                }
            }

            if (time < SP_SHIP_STOP) {
                float fly_progress = time / SP_SHIP_STOP;
                fly_progress = fly_progress > 0 ? fly_progress : 0;
                float ship_scale = (1.f - fly_progress) * SP_SHIP_MAX_SCALE;
                draw_manager.draw(IMG_INTRO_SH2_SHIP, {480, 180, 0.5, 0.5, ship_scale});
            }

            draw_text();
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

void Intro::draw_text() {
    unsigned char brightness = determine_text_brightness(0, text_timer.get_delta());
    draw_manager.draw_text(
            intro_text[text_idx],
            Justify::Centre,
            SCREEN_WIDTH / 2,
            SCREEN_HEIGHT - 26,
            {brightness, brightness, brightness},
            {0, 0, 0});
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
