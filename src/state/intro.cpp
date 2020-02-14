#include "intro.h"

#include "../assetpaths.h"
#include "../shared.h"

Intro::Intro() : StateBase("Intro", false) {
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
            L.debug("EA %d", stage_started);
            if (!stage_started) {
                draw_manager.draw(IMG_INTRO_EARTH);
            }
            if (time > 2) {
                next_stage(); return;
            }
            break;
        case City:
            L.debug("NEA %d", stage_started);
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
