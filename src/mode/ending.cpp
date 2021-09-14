#include "ending.h"

#include "anim.h"
#include "util/value.h"

#include "shared.h"

#define INITIAL_DELAY 3
#define SPACE_SCROLL_SPEED 3
#define HALL_ZOOM_SPEED 1.4f
#define HALL_DOOR_Y 236
#define HALL_DOOR_DELAY .2f
#define HALL_DOOR_SPEED 10
#define HALL_END 6
#define WELCOME_TIME 6

#define TEXT_ENTRIES_0 6
#define TEXT_ENTRIES_1 2
#define TEXT_FADE_TIME .4f
#define TEXT_SHOW_TIME 4.0f
#define TEXT_DELAY 2.2f
#define TEXT_TIME ((TEXT_FADE_TIME)*2 + (TEXT_SHOW_TIME) + (TEXT_DELAY))

#define OUTRO_TIME 105
#define OUTRO_TEXT_ENTRIES 9
#define OUTRO_TEXT_PROP .68f
#define OUTRO_TEXT_LEN (OUTRO_TEXT_PROP / OUTRO_TEXT_ENTRIES)
#define OUTRO_TEXT_DIV .4f
#define STAR_PROP .585f
#define STAR_SPEED 32
#define STAR_X 400
#define STAR_Y 140

#define THEEND_DELAY 2
#define THEEND_TIME 12

static const char* end_text_0[] = {
    "You are the Guildmaster.",
    "The galaxy is yours.",
    "None of your opponents",
    "has shown the skill and faith",
    "that lead to your success.",
    "Now we will look at your past.",
};

static const char* end_text_1[] = {
    "And now",
    "let us take a look at the future.",
};

static const char* outro_text[] = {
    "Your empire is mighty",
    "but one day it will be forgotten.",
    "Sooner or later",
    "there will be a new time of chaos",
    "and despair.",
    "But there will be people who remember you",
    "and these people will start again.",
    "And long after your time",
    "your name will still be known.",
};

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
    OUTRO_BACKGROUND,
    OUTRO_TEXT,
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
        IMG_EN1_STARS,
        {(int)(time*SPACE_SCROLL_SPEED), 0, 0, 0, 1, 1});
    draw_manager.draw(
        id(ID::INTRO_SPACE1),
        IMG_EN1_STARS,
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

    audio_manager.target_music(MUS_ST2_E);

    set_stage(Intro);
}

ExodusMode Ending::update(float dt) {

    switch (stage) {
        case Intro:
            {
                if (time > INITIAL_DELAY) {
                    set_stage(ZoomAndDoors);
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case ZoomAndDoors:
            {
                draw_manager.draw(
                    id(ID::INTRO_SPACE0),
                    IMG_EN1_STARS,
                    {(int)(time*SPACE_SCROLL_SPEED), 0, 0, 0, 1, 1});
                draw_manager.draw(
                    id(ID::INTRO_SPACE1),
                    IMG_EN1_STARS,
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
                    audio_manager.fade_out(1);
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case WelcomeFade:
            {
                if (!draw_manager.fade_active()) {
                    audio_manager.target_music(MUS_END);
                    draw_manager.draw(TGT_Secondary, IMG_EN1_STARS);
                    draw_manager.fade_start(1.2f, 24);
                    set_stage(StarsFadeIn0);
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case StarsFadeIn0:
            {
                if (!draw_manager.fade_active()) {
                    set_stage(Text0);
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case Text0:
            {
                if (!update_text(end_text_0, TEXT_ENTRIES_0)) {
                    draw_manager.draw(TGT_Secondary, IMG_BG_MENU1);
                    draw_manager.pixelswap_start();
                    set_stage(BadDeeds);
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case BadDeeds:
            {
                if (!draw_manager.pixelswap_active()) {
                    draw_manager.draw(TGT_Secondary, IMG_BG_MENU1);
                    draw_manager.pixelswap_start();
                    set_stage(GoodDeeds);
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case GoodDeeds:
            {
                if (!draw_manager.pixelswap_active()) {
                    draw_manager.fade_black(1.2f, 24);
                    set_stage(DeedsFade);
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case DeedsFade:
            {
                if (!draw_manager.fade_active()) {
                    draw_manager.draw(TGT_Secondary, IMG_EN1_STARS);
                    draw_manager.fade_start(1.2f, 24);
                    set_stage(StarsFadeIn1);
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case StarsFadeIn1:
            {
                if (!draw_manager.fade_active()) {
                    set_stage(Text1);
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case Text1:
            {
                if (!update_text(end_text_1, TEXT_ENTRIES_1)) {
                    draw_manager.fade_black(1.2f, 24);
                    set_stage(FadeText1);
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case FadeText1:
            {
                if (!draw_manager.fade_active()) {
                    set_stage(Outro);
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case Outro:
            {
                float interp = fclamp(time / OUTRO_TIME, 0, 1);
                /*
                x.95 here because the image needs to be lowered slightly
                below its height to obscure the summit of the mountain.
                */
                float back_y = RES_Y - (interp*RES_Y*.95f);
                float anchor_y = (1-interp);
                draw_manager.draw(
                    id(ID::OUTRO_BACKGROUND),
                    IMG_EN2_SCAPE,
                    {0, (int)back_y,
                    0, anchor_y, 1, 1});

                // Outro text
                bool text_wanted = false;
                int text_idx = interp / OUTRO_TEXT_LEN;
                if (interp < OUTRO_TEXT_PROP) {
                    if (text_idx < OUTRO_TEXT_ENTRIES) {
                        float i = interp;
                        while (i - OUTRO_TEXT_LEN > 0) {
                            i -= OUTRO_TEXT_LEN;
                        }
                        text_wanted = i > (OUTRO_TEXT_LEN*OUTRO_TEXT_DIV);
                    }
                }
                if (text_wanted) {
                    draw_manager.draw_text(
                        id(ID::OUTRO_TEXT),
                        outro_text[text_idx],
                        Justify::Centre,
                        RES_X/2, 240,
                        COL_TEXT2);
                } else {
                    draw_manager.draw(id(ID::OUTRO_TEXT), nullptr);
                }

                if (interp > STAR_PROP) {
                    float star_interp = fclamp((interp - STAR_PROP)*STAR_SPEED, 0, 1);
                    draw_manager.draw(
                        id(ID::STAR),
                        anim_star.interp(star_interp),
                        {STAR_X, STAR_Y, 0, 1, 1, 1});
                }

                if (interp >= 1) {
                    set_stage(TheEndDelay);
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case TheEndDelay:
            {
                if (time > THEEND_DELAY) {
                    set_stage(TheEnd);
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case TheEnd:
            {
                if (time == 0) {
                    draw_manager.draw(
                        IMG_EN2_THEEND,
                        {RES_X/2, 288,
                         .5f, .5f, 1, 1});
                }

                if (time > THEEND_TIME) {
                    draw_manager.fade_black(1.2f, 24);
                    audio_manager.fade_out(1);
                    set_stage(FadeEnd);
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case FadeEnd:
            {
                if (!draw_manager.fade_active() && time > 2) {
                    set_stage(ForNow);
                    return ExodusMode::MODE_None;
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

bool Ending::update_text(const char* text[], int entries) {
    int text_idx = (int)(time / TEXT_TIME);
    if (text_idx >= entries) {
        return false;
    }

    if (draw_manager.fade_active()) {
        return true;
    }

    float i = time;
    while (i > TEXT_TIME) {
        i -= TEXT_TIME;
    }
    if (text_faded_in < text_idx && i < TEXT_FADE_TIME) {
        draw_manager.draw(TGT_Secondary, IMG_EN1_STARS);
        draw_manager.draw_text(
            TGT_Secondary,
            Font::Large,
            text[text_idx],
            Justify::Centre,
            RES_X/2, RES_Y/2 - 20,
            COL_TEXT_SPEECH);
        draw_manager.fade_start(TEXT_FADE_TIME, 24);
        text_faded_in = text_idx;
    }
    if (text_faded_out < text_idx && i > TEXT_FADE_TIME + TEXT_SHOW_TIME) {
        draw_manager.draw(TGT_Secondary, IMG_EN1_STARS);
        draw_manager.fade_start(TEXT_FADE_TIME, 24);
        text_faded_out = text_idx;
    }

    return true;
}

void Ending::set_stage(Stage new_stage) {
    stage = new_stage;
    time = 0;
    text_faded_in = -1;
    text_faded_out = -1;

}