#include "ending.h"

#include "anim.h"
#include "util/value.h"

#include "shared.h"

#define INITIAL_DELAY 3
#define SPACE_SCROLL_SPEED 3
#define HALL_ZOOM_SPEED 1.4f
#define HALL_DOOR_Y 234
#define HALL_DOOR_DELAY .2f
#define HALL_DOOR_SPEED 10
#define HALL_END 6
#define WELCOME_TIME 6

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
    INTRO_SPACE0,
    INTRO_SPACE1,
    HALL,
    HALL_DOOR_L,
    HALL_DOOR_R,
    STAR,
    END,
};

Ending::Ending() : ModeBase("Ending") {
    stackable = false;
    time = 0;
}

void Ending::enter() {
    ModeBase::enter(ID::END);

    set_stage(Intro);

    draw_manager.draw(
        id(ID::INTRO_SPACE0),
        IMG_INTRO_SPACE,
        {(int)(time*SPACE_SCROLL_SPEED), 0, 0, 0, 1, 1});
    draw_manager.draw(
        id(ID::INTRO_SPACE1),
        IMG_INTRO_SPACE,
        {(int)(time*SPACE_SCROLL_SPEED), 0, 1, 0, 1, 1});

    draw_manager.draw(
        id(ID::HALL_DOOR_L),
        IMG_EN1_DOOR1,
        {RES_X/2, HALL_DOOR_Y,
         1, .5, 1, 1});
    draw_manager.draw(
        id(ID::HALL_DOOR_R),
        IMG_EN1_DOOR2,
        {RES_X/2, HALL_DOOR_Y,
         0, .5, 1, 1});
    draw_manager.draw(
        id(ID::HALL),
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
                draw_manager.draw(
                    id(ID::INTRO_SPACE0),
                    IMG_INTRO_SPACE,
                    {(int)(time*SPACE_SCROLL_SPEED), 0, 0, 0, 1, 1});
                draw_manager.draw(
                    id(ID::INTRO_SPACE1),
                    IMG_INTRO_SPACE,
                    {(int)(time*SPACE_SCROLL_SPEED), 0, 1, 0, 1, 1});

                float sz = 1 + (time * HALL_ZOOM_SPEED);

                int y_off = time * 24;

                int door_off = 0;

                if (time > HALL_DOOR_DELAY) {
                    door_off = (time - HALL_DOOR_DELAY) * HALL_DOOR_SPEED * sz;
                }

                draw_manager.draw(
                    id(ID::HALL_DOOR_L),
                    IMG_EN1_DOOR1,
                    {RES_X/2 - door_off, HALL_DOOR_Y,
                     1, .5, sz, sz});
                draw_manager.draw(
                    id(ID::HALL_DOOR_R),
                    IMG_EN1_DOOR2,
                    {RES_X/2 + door_off, HALL_DOOR_Y,
                     0, .5, sz, sz});
                draw_manager.draw(
                    id(ID::HALL),
                    IMG_EN1_HALL,
                    {RES_X/2, RES_Y/2 + y_off,
                     .5, .5, sz, sz});

                if (time > HALL_END) {
                    draw_manager.draw(IMG_EN1_WELCOME, {RES_X/2, RES_Y/2, .5f, .5f, 1, 1});
                }

                if (time > HALL_END + WELCOME_TIME) {
                    set_stage(WelcomeFade);
                    draw_manager.fade_black(1.2f, 24);
                }
            }
            break;
        case WelcomeFade:
            {
                if (!draw_manager.fade_active()) {
                    set_stage(InitialText);
                }
            }
            break;
        case InitialText:
            {
                // TODO:
                set_stage(Outro);
            }
            break;
        case Outro:
            {
                float interp = fclamp(time / 80, 0, 1);
                float back_y = RES_Y - (interp*RES_Y);
                draw_manager.draw(IMG_EN2_SCAPE, {0, back_y, 0, (1-interp), 1, 1});

                if (interp > 0.53) {
                    float star_interp = fclamp((interp - 0.53)*80, 0, 1);
                    draw_manager.draw(
                        id(ID::STAR),
                        anim_star.interp(star_interp),
                        {400, 70, 0, 1, 1, 1});
                }
            }
            break;
        case ForNow:
            {
                return ExodusMode::MODE_Menu;
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
