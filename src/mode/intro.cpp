#include "intro.h"

#include <cmath>

#include "anim.h"

#include "assetpaths.h"

#include "util/value.h"

// Facility to execute a single statement or block once only.
// Requires a unique name within the context - but this is checked at compile time.
#define ONCE(once_id) static bool once_id = true; if (once_id && !(once_id = false))

static bool ARTEX_LOGO = true;

static const char* intro_text[] = {
    "It is the year 3011.",
    "Civilization has failed.",
    "The last living creatures on Earth",
    "starve in rotten cellars",
    "or fight against each other",
    "for the last food rations.",
    "But a few bosses of once great concern",
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
    "Enter the secret code 594",
    "Access denied.",
    "Access admitted.",
    "The people have succeeded.",
    "So they have a chance to escape from this planet",
    "and find a new home",
    "out in space.",
    "This is the beginning of a long",
    "and dangerous journey.",
};

float START_FADEIN         = 1.f;
float START_FADEOUT        = START_FADEIN + 3.f;

float CITY_SHIP_START      = 5.f;
float CITY_SHIP_STOP       = 17.f;
float CITY_SHIP_MAX_SCALE  = 1.6f;
float CITY_SHIP_SHOT_1     = 22.0f;
float CITY_SHIP_SHOT_2     = 22.2f;
float CITY_SHIP_SHOT_3     = 24.5f;
float CITY_SHIP_SHOT_4     = 24.7f;
float CITY_SHIP_SHOT_TIME  = 0.1f;
float CITY_SHIP_START_Y    = 260.f;
float CITY_SHIP_END_Y      = 210.f;

float SP_SHIP_STOP         = 14.f;
float SP_SHIP_MAX_SCALE    = 2.0f;
float SP_SHIP_X            = 422.f;
float SP_SHIP_START_Y      = 215.f;
float SP_SHIP_END_Y        = 260.f;

float SHOOT_START          = 1.f;
float SHOOT_FRAME          = 0.08f;
float SHOT_START           = 1.55f;
float SHOT_FRAME           = 0.08f;

float GUARD_FRAME          = 0.25f;
float GUARD_SHOT_PAUSE     = 0.8f;

float DOOR_PX_PER_SEC      = 14.f;

float SHUTTLE_P1_START_Y   = 240.f;
float SHUTTLE_P1_END_X     = 320.f;
float SHUTTLE_P1_END_Y     = 140.f;
float SHUTTLE_P1_END       = 4.f;
float SHUTTLE_P2_START     = 4.4f;
float SHUTTLE_P2_END       = 4.5f;
float SHUTTLE_P2_END_X     = 90.f;
float SHUTTLE_P2_END_Y     = 30.f;

float SPC_SHIP_START_X     = 173.f;
float SPC_SHIP_START_Y     = 111.f;
float SPC_SHUTTLE_START_X  = 400.f;
float SPC_SHUTTLE_START_Y  = 240.f;
float SPC_SHUTTLE_END_X    = SPC_SHIP_START_X + 30;
float SPC_SHUTTLE_END_Y    = SPC_SHIP_START_Y + 22;
float SPC_SHUTTLE_END      = 9.f;
float SPC_SHIP_DOOR_START  = SPC_SHUTTLE_END + 4.f;
float SPC_SHIP_DOOR_STEP   = 0.3f;
float SPC_SHIP_START       = SPC_SHIP_DOOR_START + 4.f;
float SPC_SHIP_END         = SPC_SHIP_START + 8;
float SPC_SHIP_END_X       = 366.f;
float SPC_SHIP_END_Y       = 256.f;
float SPC_WARP_START       = SPC_SHIP_END + 0.4;
float SPC_WARP_END         = SPC_WARP_START + 1.8;
int   SPC_WARP_STAGES      = 10;
float SPC_FADE_START       = SPC_WARP_END + 2;

float TTL_FADE_START       = 1.f;
float TTL_FADE_TIME        = 1.2f;
float TTL_JK_START         = TTL_FADE_START + TTL_FADE_TIME + 1.2f;
float TTL_JK_END           = TTL_JK_START + 3.f;
float TTL_PRESENTS_START   = TTL_JK_END + .8f;
float TTL_PRESENTS_END     = TTL_PRESENTS_START + 3.f;
float TTL_EXODUS_START     = TTL_PRESENTS_END + 1.f;
float TTL_EXODUS_END       = TTL_EXODUS_START + 2.f;
float TTL_SUBTTL_START     = TTL_EXODUS_END + 0.2;
float TTL_SUBTTL_CHAR      = 0.1;
float TTL_FADEOUT_START    = TTL_SUBTTL_START + TTL_SUBTTL_CHAR * 25 + 2.f;
float TTL_FADEOUT_TIME     = 1.2f;
float TTL_END              = TTL_FADEOUT_START + TTL_FADEOUT_TIME + .6f;

const float MAX_TEXT_TIME  = 3.8;

enum ID {
    CITY_SHIP,
    SP_SHIP,
    SHOOT,
    SHOT,
    GUARDSHOT,
    KEYPAD,
    DOOR_L,
    DOOR_R,
    SHUTTLE_LAUNCH,
    SPC_SHUTTLE,
    SPC_SHIP,
    SPC_WARP,
    TTL_JK,
    TTL_PRESENTS,
    TTL_EXODUS,
    TTL_COLOFSPACE,
    TEXT,

    END,
};

unsigned int nums_held = 0;
unsigned int prev_nums_held = 0;
unsigned int input_code = 0;
unsigned char keys_to_input = 3;
bool interactive_sequence_completed = false;
bool launchpad_light_toggle = false;
unsigned int warp_stage = 0;

Anim anim_spc_warp(
    5,
    IMG_HP1_HYP1,
    IMG_HP1_HYP2,
    IMG_HP1_HYP3,
    IMG_HP1_HYP4,
    IMG_HP1_HYP5
);

const char* KP_KEYS[] = {
    IMG_INTRO_PH1_PUSH_1,
    IMG_INTRO_PH1_PUSH_2,
    IMG_INTRO_PH1_PUSH_3,
    IMG_INTRO_PH1_PUSH_4,
    IMG_INTRO_PH1_PUSH_5,
    IMG_INTRO_PH1_PUSH_6,
    IMG_INTRO_PH1_PUSH_7,
    IMG_INTRO_PH1_PUSH_8,
    IMG_INTRO_PH1_PUSH_9,
};

const DrawTransform KP_TRANSFORMS[] = {
    {238, 150, 0.5, 0.5, 2, 2},
    {328, 156, 0.5, 0.5, 2, 2},
    {420, 165, 0.5, 0.5, 2, 2},
    {224, 232, 0.5, 0.5, 2, 2},
    {316, 242, 0.5, 0.5, 2, 2},
    {413, 254, 0.5, 0.5, 2, 2},
    {203, 324, 0.5, 0.5, 2, 2},
    {301, 339, 0.5, 0.5, 2, 2},
    {401, 354, 0.5, 0.5, 2, 2},
};

Intro::Intro() : ModeBase("Intro"), text_idx(0) {
}

void Intro::enter() {
    ModeBase::enter(ID::END);
    stage = Stage::None;
    text_idx = 0;
    stage_started = false;
    time = 0;
    text_time = 0;
    for (int i = 0; i < 9; ++i)
        kp_num_held_seconds[i] = 0;
}

ExodusMode Intro::update(float delta) {
    static bool text_delay_complete = false;
    unsigned int released_keys;
    unsigned char brightness;
    float door_time;

    time += delta;
    text_time += delta;

    if (input_manager.consume(K_Space) || input_manager.consume(K_Escape)) {
        if (ENHANCED() || stage >= Title) {
            stage = End;
        } else {
            draw_manager.cancel_transitions();
            draw_manager.clear();
            while (stage < Title) {
                next_stage();
            }
        }
        return ExodusMode::MODE_None;
    }

    switch(stage) {
        case None:
            next_stage(); return ExodusMode::MODE_None;
            break;
        case Artex:
            if (time < START_FADEIN) {
                ONCE(oid_startclear) draw_manager.clear();
                return ExodusMode::MODE_None;
            }

            if (ARTEX_LOGO) {
                ONCE(oid_start_fadein) {
                    audio_manager.target_music(MUS_INTRO);
                    draw_manager.draw(TGT_Secondary, IMG_ARTEX);
                    draw_manager.fade_start(1.2f, 24);
                }
            } else {
                ONCE(oid_start_fadein) {
                    audio_manager.target_music(MUS_INTRO);
                    draw_manager.fade_white(1.2f, 24);
                }
            }

            if (time > START_FADEOUT) {
                ONCE(oid_start_fadeout) {
                    draw_manager.fade_black(1.2f, 24);
                }
            }

            if (time > START_FADEOUT + 1.6f) {
                next_stage(); return ExodusMode::MODE_None;
            }

            break;
        case Earth:
            if (!stage_started) {
                draw_manager.clear();
                draw_manager.clear(TGT_Secondary);
                draw_manager.draw(TGT_Secondary, IMG_INTRO_EARTH);
                draw_manager.pixelswap_start(nullptr);
                break;
            }

            if (draw_manager.pixelswap_active()) {
                return ExodusMode::MODE_None;
            }

            if (time < 1.8) {
                return ExodusMode::MODE_None;
            } else if (!text_delay_complete) {
                text_delay_complete = true;
                text_time = 0;
            }


            if (text_time > MAX_TEXT_TIME) {
                if (text_idx == 0) {
                    ++text_idx;
                    text_time = 0;
                } else {
                    next_stage(); return ExodusMode::MODE_None;
                }
            }

            draw_text();
            break;
        case City:
            if (!stage_started) {
                draw_manager.clear(TGT_Secondary);
                draw_manager.draw(TGT_Secondary, IMG_INTRO_CITY);
                draw_manager.pixelswap_start(nullptr);
                break;
            }

            if (draw_manager.pixelswap_active()) {
                return ExodusMode::MODE_None;
            }

            if (text_idx == 1) {
                ++text_idx;
                text_time = 0;
                draw_manager.save_background();
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

                draw_manager.draw(id(ID::CITY_SHIP), ship, {10 + RES_X / 2, ship_y, 0.5, 0.5, ship_scale, ship_scale});
            }

            if (text_idx >= 1 && text_time > MAX_TEXT_TIME) {
                if (text_idx < 8) {
                    ++text_idx;
                    text_time = 0;
                } else {
                    next_stage(); return ExodusMode::MODE_None;
                }
            }

            draw_text();
            break;
        case Starport:
            if (!stage_started) {
                draw_manager.draw(IMG_INTRO_STARPORT);
                draw_manager.save_background();
                text_idx++;
                break;
            }

            if (text_idx >= 8 && text_time > MAX_TEXT_TIME) {
                if (text_idx < 13) {
                    ++text_idx;
                    text_time = 0;
                } else {
                    next_stage(); return ExodusMode::MODE_None;
                }
            }

            if (time < SP_SHIP_STOP) {
                float fly_progress = time / SP_SHIP_STOP;
                fly_progress = fly_progress > 0 ? fly_progress : 0;
                float ship_scale = (1.f - fly_progress) * SP_SHIP_MAX_SCALE;
                int ship_y = SP_SHIP_START_Y + (int)((SP_SHIP_END_Y - SP_SHIP_START_Y) * fly_progress);
                draw_manager.draw(id(ID::SP_SHIP), IMG_INTRO_SH2_SHIP,
                        {(int)SP_SHIP_X, (int)ship_y,
                        0.5, 0.5, ship_scale, ship_scale});
            }

            draw_text();
            break;
        case Corridor:
            if (!stage_started) {
                draw_manager.draw(IMG_INTRO_CORRIDOR);
                draw_manager.draw(IMG_INTRO_FI1_FIRE1, {468, 104, 0, 0, 1.0, 1.0});
                text_idx++;
                break;
            }

            if (text_idx >= 13 && text_time > MAX_TEXT_TIME) {
                if (text_idx < 17) {
                    ++text_idx;
                    text_time = 0;
                } else {
                    next_stage(); return ExodusMode::MODE_None;
                }
            }

            draw_text();
            break;
        case Guard:
            if (!stage_started) {
                draw_manager.draw(IMG_INTRO_DOOR);
                draw_manager.draw(IMG_INTRO_GU1_GUARD, {330, 257, 0.5, 0.5, 1.0, 1.0});
                break;
            }

            if (time < 1.8) {
                return ExodusMode::MODE_None;
            }

            next_stage(); return ExodusMode::MODE_None;

            break;
        case Shoot:
            if (!stage_started) {
                draw_manager.draw(IMG_INTRO_CORRIDOR);
                draw_manager.save_background();
                break;
            }

            if (time < SHOOT_START) {
                ONCE(oid_shoot_1) draw_manager.draw(id(ID::SHOOT), IMG_INTRO_FI1_FIRE1, {468, 104, 0, 0, 1.0, 1.0});
            } else if (time < SHOOT_START + SHOOT_FRAME * 1) {
                ONCE(oid_shoot_2) draw_manager.draw(id(ID::SHOOT), IMG_INTRO_FI1_FIRE2, {442, 82, 0, 0, 1.0, 1.0});
            } else if (time < SHOOT_START + SHOOT_FRAME * 2) {
                ONCE(oid_shoot_3) draw_manager.draw(id(ID::SHOOT), IMG_INTRO_FI1_FIRE3, {422, 62, 0, 0, 1.0, 1.0});
            } else if (time < SHOOT_START + SHOOT_FRAME * 3) {
                ONCE(oid_shoot_4) draw_manager.draw(id(ID::SHOOT), IMG_INTRO_FI1_FIRE4, {320, 68, 0, 0, 1.0, 1.0});
            } else if (time < SHOOT_START + SHOOT_FRAME * 4) {
                ONCE(oid_shoot_5) draw_manager.draw(id(ID::SHOOT), IMG_INTRO_FI1_FIRE5, {294, 78, 0, 0, 1.0, 1.0});
            } else if (time < SHOOT_START + SHOOT_FRAME * 5) {
                ONCE(oid_shoot_6) draw_manager.draw(id(ID::SHOOT), IMG_INTRO_FI1_FIRE6, {332, 84, 0, 0, 1.0, 1.0});
                ONCE(oid_shoot_bg) draw_manager.save_background();
            }

            if (time > SHOT_START) {
                if (time < SHOT_START + SHOT_FRAME) {
                    ONCE(oid_shot_1) draw_manager.draw(id(ID::SHOT), IMG_INTRO_FI1_SHOT1, {272, 146, 0, 0, 2.0, 2.0});
                    ONCE(oid_shot_fire) draw_manager.draw(id(ID::SHOOT), IMG_INTRO_FI1_FIRE6, {332, 84, 0, 0, 1.0, 1.0});
                } else if (time < SHOT_START + SHOT_FRAME * 2) {
                    ONCE(oid_shot_2) draw_manager.draw(id(ID::SHOT), IMG_INTRO_FI1_SHOT2, {204, 156, 0, 0, 2.0, 2.0});
                } else if (time < SHOT_START + SHOT_FRAME * 3) {
                    ONCE(oid_shot_3) draw_manager.draw(id(ID::SHOT), IMG_INTRO_FI1_SHOT3, {136, 164, 0, 0, 2.0, 2.0});
                } else if (time < SHOT_START + SHOT_FRAME * 4) {
                    ONCE(oid_shot_4) draw_manager.draw(id(ID::SHOT), IMG_INTRO_FI1_SHOT4, {68, 172, 0, 0, 2.0, 2.0});
                } else if (time < SHOT_START + SHOT_FRAME * 5) {
                    ONCE(oid_shot_5) draw_manager.draw(id(ID::SHOT), IMG_INTRO_FI1_SHOT5, {0, 180, 0, 0, 2.0, 2.0});
                } else {
                    next_stage(); return ExodusMode::MODE_None;
                }
            }

            break;
        case GuardShot:
            if (!stage_started) {
                draw_manager.draw(IMG_INTRO_DOOR);
                draw_manager.save_background();
            }

            if (time < GUARD_FRAME) {
                ONCE(oid_guard_1) draw_manager.draw(id(ID::GUARDSHOT), IMG_INTRO_BT1_FALL1, {314, 271, 0.5, 0.5, 2.0, 2.0});
            } else if (time < GUARD_FRAME * 2) {
                ONCE(oid_guard_2) draw_manager.draw(id(ID::GUARDSHOT), IMG_INTRO_BT1_FALL2, {291, 300, 0.5, 0.5, 2.0, 2.0});
            } else if (time < GUARD_FRAME * 3) {
                ONCE(oid_guard_3) draw_manager.draw(id(ID::GUARDSHOT), IMG_INTRO_BT1_FALL3, {293, 307, 0.5, 0.5, 2.0, 2.0});
            } else if (time < GUARD_FRAME * 4) {
                ONCE(oid_guard_4) draw_manager.draw(id(ID::GUARDSHOT), IMG_INTRO_BT1_FALL4, {285, 391, 0.5, 0.5, 2.0, 2.0});
            } else if (time < GUARD_FRAME * 5) {
                ONCE(oid_guard_5) draw_manager.draw(id(ID::GUARDSHOT), IMG_INTRO_BT1_FALL5, {270, 484, 0.5, 1.0, 2.0, 2.0});
            } else if (time < GUARD_FRAME * 6) {
                ONCE(oid_guard_6) draw_manager.draw(id(ID::GUARDSHOT), IMG_INTRO_BT1_FALL6, {270, 484, 0.5, 1.0, 2.0, 2.0});
            } else if (time < GUARD_FRAME * 7) {
                ONCE(oid_guard_7) draw_manager.draw(id(ID::GUARDSHOT), IMG_INTRO_BT1_FALL7, {270, 484, 0.5, 1.0, 2.0, 2.0});
            } else {
                next_stage(); return ExodusMode::MODE_None;
            }

            break;
        case GuardShotPause:
            if (time > GUARD_SHOT_PAUSE) {
                next_stage(); return ExodusMode::MODE_None;
            }
            break;
        case Keypad:
            if (!stage_started) {
                draw_manager.draw(id(ID::KEYPAD), IMG_INTRO_KEYPAD);
                text_idx = 18;
                draw_manager.save_background();
                draw_manager.show_cursor(true);
                break;
            }

            nums_held = kp_held_nums(delta);

            if (keys_to_input) {
                // State changed - just redraw everything
                if (prev_nums_held != nums_held) {
                    draw_manager.draw(id(ID::KEYPAD), IMG_INTRO_KEYPAD);

                    for (int i = 1; i <= 9; ++i) {
                        if ((nums_held >> i) & 1)
                            draw_manager.draw(KP_KEYS[i - 1], KP_TRANSFORMS[i - 1]);
                    }

                    draw_manager.save_background();
                }

                if (text_time < MAX_TEXT_TIME / 2) {
                    brightness = determine_text_brightness(0, text_time);
                } else {
                    brightness = 0xFF;
                }

                draw_manager.draw_text(
                        id(ID::TEXT),
                        intro_text[text_idx],
                        Justify::Centre,
                        RES_X / 2,
                        RES_Y - 26,
                        {brightness, brightness, brightness});


                released_keys = prev_nums_held & ~nums_held;
                if (released_keys) {
                    // There is no 0 key, so start from 1
                    for (int i = 1; i < 10; ++i) {
                        if ((released_keys >> i) & 1) {
                            unsigned int digit = 1;
                            if (keys_to_input == 2) digit = 10;
                            if (keys_to_input == 3) digit = 100;
                            input_code += i * digit;
                            --keys_to_input;
                        }
                    }
                }
                prev_nums_held = nums_held;
                return ExodusMode::MODE_None;
            }

            ONCE(oid_code) L.debug("Entered code: %d", input_code);
            ONCE(oid_padddone) draw_manager.draw(IMG_INTRO_KEYPAD);

            ONCE(oid_codedone_fadetext) text_time = 0;


            // We only want to do the fade-out half, so pretend
            // we started half-max-time ago.
            if (!interactive_sequence_completed
                && text_time < MAX_TEXT_TIME / 2) {
                brightness = determine_text_brightness(
                    -MAX_TEXT_TIME / 2 , text_time);

                draw_manager.draw_text(
                        id(ID::TEXT),
                        intro_text[text_idx],
                        Justify::Centre,
                        RES_X / 2,
                        RES_Y - 26,
                        {brightness, brightness, brightness});
                return ExodusMode::MODE_None;
            } else {
                interactive_sequence_completed = true;
            }

            ONCE(oid_codedone) {
                if (input_code == 594) {
                    text_idx = 20;
                } else {
                    text_idx = 19;
                }
                text_time = 0;
                return ExodusMode::MODE_None;
            }

            draw_text();

            if (text_time < MAX_TEXT_TIME) {
                return ExodusMode::MODE_None;
            }

            if (input_code == 594) {
                ONCE(coode_startface) draw_manager.fade_black(1.2f, 12);
                draw_manager.show_cursor(false);
                if (draw_manager.fade_active() || text_time < MAX_TEXT_TIME + 2.4) {
                    return ExodusMode::MODE_None;
                }
                next_stage(); return ExodusMode::MODE_None;
            } else {
                stage = Fail;
                stage_started = false;
                return ExodusMode::MODE_None;
            }

            break;
        case Success:
            if (!stage_started) {
                draw_manager.draw(IMG_INTRO_DOOR);
                draw_manager.draw(IMG_INTRO_OD1_BEHIND, {319, 218, 0.5, 0.5, 1.0, 1.0});
                draw_manager.save_background();
                text_idx = 21;
            }

            door_time = time - 2.6f;
            door_time = door_time < 0 ? 0 : door_time;
            draw_manager.draw(id(ID::DOOR_L), IMG_INTRO_OD1_DOOR_L, {(int)(320 - door_time * DOOR_PX_PER_SEC), 219, 1.0, 0.5, 1.0, 1.0});
            draw_manager.draw(id(ID::DOOR_R), IMG_INTRO_OD1_DOOR_R, {(int)(320 + door_time * DOOR_PX_PER_SEC), 219, 0.0, 0.5, 1.0, 1.0});
            draw_manager.draw(IMG_INTRO_OD1_FRAME , {320, 212, 0.5, 0.5, 1.0, 1.0});
            draw_manager.draw(IMG_INTRO_OD1_SEPP  , {319, 482, 0.5, 1.0, 1.0, 1.0});

            if (time > 0.9) {
                draw_manager.draw(IMG_INTRO_OD1_OFF, {152,  97, 0.5, 0.5, 2.0, 2.0});
            }

            if (time > 2.15) {
                draw_manager.draw(IMG_INTRO_OD1_ON , {152, 97, 0.5, 0.5, 2.0, 2.0});
                ONCE(oid_dooropen) text_time = 0;
                draw_text();
                if (text_time < MAX_TEXT_TIME) {
                    return ExodusMode::MODE_None;
                } else {
                    next_stage(); return ExodusMode::MODE_None;
                }
            }

            break;
        case DepartShuttle:
            if (!stage_started) {
                draw_manager.draw(IMG_INTRO_LAUNCH);
                draw_manager.save_background();
                text_idx = 22;
                break;
            }

            if (text_time > MAX_TEXT_TIME) {
                if (text_idx < 23) {
                    ++text_idx;
                    text_time = 0;
                } else {
                    next_stage(); return ExodusMode::MODE_None;
                }
            }

            if (!launchpad_light_toggle && fmod(time, 1.4) < 0.1) {
                draw_manager.draw(IMG_INTRO_SH4_BLINK1 , { 30, 211, 0.5, 0.5, 1.0, 1.0});
                draw_manager.draw(IMG_INTRO_SH4_BLINK2 , {497, 158, 0.5, 0.5, 1.0, 1.0});
                draw_manager.draw(IMG_INTRO_SH4_BLINK3 , {540, 244, 0.5, 0.5, 1.0, 1.0});
                launchpad_light_toggle = true;
            }

            if (launchpad_light_toggle && (fmod(time, 1.4) >= 0.1)) {
                draw_manager.draw(IMG_INTRO_LAUNCH);
                launchpad_light_toggle = false;
            }

            if (time < SHUTTLE_P1_END) {
                const char *ship = IMG_INTRO_SH4_SHUTTLE1;
                float phase1_linear_interp = time / SHUTTLE_P1_END;
                float phase1_anim_intetrp = sqrt(phase1_linear_interp);
                float shuttle_y = SHUTTLE_P1_START_Y - ((SHUTTLE_P1_START_Y - SHUTTLE_P1_END_Y) * phase1_anim_intetrp);
                if (time < 0.8 * SHUTTLE_P1_END) {
                    ship = IMG_INTRO_SH4_SHUTTLE2;
                } else if (time < 0.9 * SHUTTLE_P1_END) {
                    ship = IMG_INTRO_SH4_SHUTTLE3;
                } else {
                    ship = IMG_INTRO_SH4_SHUTTLE4;
                }
                draw_manager.draw(id(ID::SHUTTLE_LAUNCH), ship, {(int)SHUTTLE_P1_END_X, (int)shuttle_y, 0.5, 0.5, 2.0, 2.0});
                draw_manager.draw(IMG_INTRO_SH4_MOVEOUT , {318, 249, 0.5, 0.5, 1.0, 1.0});
            } else if (time > SHUTTLE_P2_START){
                float phase2_linear_interp = (time - SHUTTLE_P2_START) / SHUTTLE_P2_END;
                float phase2_anim_interp = pow(phase2_linear_interp, 1.3);
                float shuttle_x = SHUTTLE_P1_END_X - (SHUTTLE_P1_END_X - SHUTTLE_P2_END_X) * phase2_anim_interp;
                float shuttle_y = SHUTTLE_P1_END_Y - (SHUTTLE_P1_END_Y - SHUTTLE_P2_END_Y) * phase2_anim_interp;
                float scale = 2 * (1.f - (0.2 * phase2_anim_interp));
                draw_manager.draw(id(ID::SHUTTLE_LAUNCH), IMG_INTRO_SH4_SHUTTLE4, {(int)shuttle_x, (int)shuttle_y, 0.5, 0.5, scale, scale});
            } else {
                draw_manager.draw(id(ID::SHUTTLE_LAUNCH), IMG_INTRO_SH4_SHUTTLE4, {(int)SHUTTLE_P1_END_X, (int)SHUTTLE_P1_END_Y, 0.5, 0.5, 2.0, 2.0});
            }

            draw_text();

            break;
        case DepartShip:
            if (!stage_started) {
                draw_manager.draw(IMG_INTRO_SPACE);
                draw_manager.draw(id(ID::SPC_SHIP), IMG_INTRO_SH3_CRUISER, {(int)SPC_SHIP_START_X, (int)SPC_SHIP_START_Y, 0.5, 0.5, 1.0, 1.0});
                draw_manager.save_background();
                text_idx = 24;
                break;
            }

            if (time < SPC_SHUTTLE_END) {
                float interp = time / SPC_SHUTTLE_END;
                float scale = 0.02 + 2 * (1 - interp);
                int x = SPC_SHUTTLE_START_X + (SPC_SHUTTLE_END_X - SPC_SHUTTLE_START_X) * interp;
                int y = SPC_SHUTTLE_START_Y + (SPC_SHUTTLE_END_Y - SPC_SHUTTLE_START_Y) * interp;
                draw_manager.draw(id(ID::SPC_SHUTTLE), IMG_INTRO_SH3_SHUTTLE, {x, y, 0.5, 0.5, scale, scale});
            } else {
                ONCE(oid_hideshuttle) {
                    draw_manager.draw(IMG_INTRO_SPACE);
                    draw_manager.save_background();
                    draw_manager.draw(id(ID::SPC_SHIP), IMG_INTRO_SH3_CRUISER, {(int)SPC_SHIP_START_X, (int)SPC_SHIP_START_Y, 0.5, 0.5, 1.0, 1.0});
                }
            }

            if (time >= SPC_SHIP_DOOR_START) {
                ONCE(oid_door0) draw_manager.draw(id(ID::SPC_SHIP), IMG_INTRO_SH3_CRUISER, {(int)SPC_SHIP_START_X, (int)SPC_SHIP_START_Y, 0.5, 0.5, 1.0, 1.0});
            }

            if (time >= SPC_SHIP_DOOR_START + SPC_SHIP_DOOR_STEP) {
                ONCE(oid_door1) draw_manager.draw(id(ID::SPC_SHIP), IMG_INTRO_SH3_CRUISER2, {(int)SPC_SHIP_START_X, (int)SPC_SHIP_START_Y, 0.5, 0.5, 1.0, 1.0});
            }

            if (time >= SPC_SHIP_DOOR_START + SPC_SHIP_DOOR_STEP * 2) {
                ONCE(oid_door2) draw_manager.draw(id(ID::SPC_SHIP), IMG_INTRO_SH3_CRUISER1, {(int)SPC_SHIP_START_X, (int)SPC_SHIP_START_Y, 0.5, 0.5, 1.0, 1.0});
            }

            if (time >= SPC_SHIP_START) {
                float interp = (time - SPC_SHIP_START) / (SPC_SHIP_END - SPC_SHIP_START);
                interp = interp > 1 ? 1 : interp;
                float anim_interp = pow(interp, 2);
                float scale = 1 - anim_interp;
                int x = SPC_SHIP_START_X + (SPC_SHIP_END_X - SPC_SHIP_START_X) * anim_interp;
                int y = SPC_SHIP_START_Y + (SPC_SHIP_END_Y - SPC_SHIP_START_Y) * anim_interp;
                draw_manager.draw(id(ID::SPC_SHIP), IMG_INTRO_SH3_CRUISER1, {x, y, 0.5, 0.5, scale, scale});
            }

            if (time >= SPC_WARP_START) {
                float interp = (time - SPC_WARP_START) / (SPC_WARP_END - SPC_WARP_START);
                interp = interp > 1 ? 1 : interp;
                float stage_interp = warp_stage / (float)SPC_WARP_STAGES;
                if (interp > stage_interp) {
                    ++warp_stage;
                    stage_interp = warp_stage / (float)SPC_WARP_STAGES;
                    float scale_interp = stage_interp < 0.5 ? stage_interp : 1 - stage_interp;
                    float scale = 16 * pow(scale_interp, 2);
                    draw_manager.draw(
                            id(ID::SPC_WARP),
                            anim_spc_warp.frame(warp_stage),
                            {(int)SPC_SHIP_END_X,
                             (int)SPC_SHIP_END_Y,
                             0.5, 0.5, scale, scale});
                }
            }

            if (time >= SPC_WARP_END) {
                ONCE(oid_warpend) draw_manager.draw(id(ID::SPC_WARP), nullptr);
            }

            if (text_time > MAX_TEXT_TIME) {
                if (text_idx < 26) {
                    ++text_idx;
                    text_time = 0;
                }
            }

            if (time > SPC_FADE_START) {
                ONCE(oid_spcfade) draw_manager.fade_black(1.2f, 12);
                if (!draw_manager.fade_active()) {
                    next_stage(); return ExodusMode::MODE_None;
                }
            }

            draw_text();

            break;
        case Title:
            if (time < TTL_FADE_START) {
                return ExodusMode::MODE_None;
            }

            if (!stage_started) {
                draw_manager.draw(TGT_Secondary, IMG_INTRO_SPACE, {RES_X/2, 0, 1, 0, 1, 1});
                draw_manager.draw(TGT_Secondary, IMG_INTRO_SPACE, {RES_X/2, 0, 0, 0, 1, 1});
                draw_manager.fade_start(TTL_FADE_TIME, 12);
                break;
            }

            ONCE(oid_titlebg) draw_manager.save_background();

            if (time < TTL_JK_START) {
                return ExodusMode::MODE_None;
            } else if (time < TTL_JK_END) {
                float interp = (time - TTL_JK_START) / (TTL_JK_END - TTL_JK_START);
                float anim_interp = 1.f;
                if (interp < 0.2)
                    anim_interp = interp * (1.f / .2f);
                if (interp > 0.8)
                    anim_interp = (1 - interp) * (1.f / .2f);
                draw_manager.draw(id(ID::TTL_JK), IMG_FILM_1_SPR_2_3, {RES_X/2, RES_Y/2, 0.5, 0.5, 1, anim_interp});
            } else {
                ONCE(oid_removejk) draw_manager.draw(id(ID::TTL_JK), nullptr);
            }

            if (time < TTL_PRESENTS_START) {
                return ExodusMode::MODE_None;
            } else if (time < TTL_PRESENTS_END) {
                float interp = (time - TTL_PRESENTS_START) / (TTL_PRESENTS_END - TTL_PRESENTS_START);
                float anim_interp = 1.f;
                if (interp < 0.2)
                    anim_interp = interp * (1.f / .2f);
                if (interp > 0.8)
                    anim_interp = (1 - interp) * (1.f / .2f);
                draw_manager.draw(id(ID::TTL_PRESENTS), IMG_FILM_1_SPR_3_3, {RES_X/2, RES_Y/2, 0.5, 0.5, 1, anim_interp});
            } else {
                ONCE(oid_removepresents) draw_manager.draw(id(ID::TTL_PRESENTS), nullptr);
            }

            if (time < TTL_EXODUS_START) {
                return ExodusMode::MODE_None;
            } else if (time < TTL_EXODUS_END) {
                float interp = (time - TTL_EXODUS_START) / (TTL_EXODUS_END - TTL_EXODUS_START);
                float sc = interp;
                draw_manager.draw(id(ID::TTL_EXODUS), IMG_FILM_1_SPR_1, {RES_X/2, RES_Y/2, 0.5, 0.5, sc, sc});
            }

            if (time < TTL_SUBTTL_START) {
                return ExodusMode::MODE_None;
            } else if (time < TTL_FADEOUT_START) {
                int nchars = (int)((time - TTL_SUBTTL_START) / TTL_SUBTTL_CHAR);
                if (nchars > 25)
                    nchars = 25;
                char subttl[] = "THE COLONIZATION OF SPACE";
                subttl[nchars] = '\0';
                draw_manager.draw_text(
                    id(ID::TTL_COLOFSPACE),
                    subttl,
                    Justify::Left,
                    178, 320,
                    COL_TEXT);
            }

            if (time > TTL_FADEOUT_START) {
                ONCE(oid_ttlfadeout) draw_manager.fade_black(TTL_FADEOUT_TIME, 12);
            }

            if (time > TTL_END) {
                next_stage();
                return ExodusMode::MODE_None;
            }

            return ExodusMode::MODE_None;
            break;
        case End:
            return ExodusMode::MODE_Menu;
        case Fail:
            if (!stage_started) {
                draw_manager.draw(IMG_INTRO_FAIL);
            }
            break;
        default:
            break;
    }

    stage_started = true;
    return ExodusMode::MODE_None;
}

void Intro::next_stage() {
    draw_manager.clear_sprite_ids();
    if (stage != Stage::End) {
        stage = (Stage)((int)stage + 1);
        stage_started = false;
    }
    time = 0;
    text_time = 0;

    // Skip keypad in Enhanced mode
    if (ENHANCED() && (stage == Stage::Keypad)) {
        next_stage();
    }
}

void Intro::draw_text() {
    unsigned char brightness = determine_text_brightness(0, text_time);
    draw_manager.draw_text(
            id(ID::TEXT),
            intro_text[text_idx],
            Justify::Centre,
            RES_X / 2,
            RES_Y - 26,
            {brightness, brightness, brightness});
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

unsigned int Intro::kp_held_nums(float delta) {
    unsigned int nums_held = input_manager.read_numbers();
    SpriteClick click = draw_manager.query_click(id(ID::KEYPAD));
    if (click.id) {
        for (int i = 0; i < 9; ++i) {
            if (in_radius(click.x, click.y,
                          (float)KP_TRANSFORMS[i].x/RES_X,
                          (float)KP_TRANSFORMS[i].y/RES_Y, 0.08)) {
                kp_num_held_seconds[i] = 0.2;
            }
        }
    }
    for (int i = 0; i < 9; ++i) {
        if (kp_num_held_seconds[i] > 0) {
            float newval = kp_num_held_seconds[i] - delta;
            nums_held |= 1 << (i+1);
            newval = newval < 0 ? 0 : newval;
            kp_num_held_seconds[i] = newval;
        }
    }
    return nums_held;
}


