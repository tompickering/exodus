#include "ending.h"

#include "anim.h"
#include "util/value.h"

#include "shared.h"

#define INITIAL_DELAY 3
#define ZOOM_SPEED .2f

Anim anim_star(
    11,
    nullptr,
    IMG_EN2_STS1,
    IMG_EN2_STS2,
    IMG_EN2_STS3,
    IMG_EN2_STS4,
    IMG_EN2_STS5,
    IMG_EN2_STS6,
    IMG_EN2_STS7,
    IMG_EN2_STS8,
    IMG_EN2_STS9,
    nullptr
);


enum ID {
    HALL,
    END,
};

Ending::Ending() : ModeBase("Ending") {
    stackable = false;
}

void Ending::enter() {
    ModeBase::enter(ID::END);

    draw_manager.draw(IMG_INTRO_SPACE);
    draw_manager.save_background();

    draw_manager.draw(
        IMG_EN1_HALL,
        {RES_X/2, RES_Y/2,
         .5, .5, 1, 1});

    stage = Intro;
}

ExodusMode Ending::update(float dt) {

    switch (stage) {
        case Intro:
            {
                if (time > INITIAL_DELAY) {
                    set_stage(ZoomAndDoors);
                }
            }
            break;
        case ZoomAndDoors:
            {
                float sz = 1 + (time * ZOOM_SPEED);
                draw_manager.draw(
                    IMG_EN1_HALL,
                    {RES_X/2, RES_Y/2,
                     .5, .5, sz, sz});
            }
            break;
        default:
            // TODO: Remove this when everything else done
            break;
    }

    time += dt;

    return ExodusMode::MODE_None;
}

void Ending::set_stage(Stage new_stage) {
    stage = new_stage;
    time = 0;
}
