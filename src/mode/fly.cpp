#include "fly.h"

#include "anim.h"
#include "assetpaths.h"

#define N_THRUSTERS 12
#define FLY_SCALE 1.75f

#define THRUST_DELAY 0.2f
#define THRUST_TIME 2.f
#define FLY_START (THRUST_DELAY*N_THRUSTERS + THRUST_TIME + 0.3f)
#define FLY_TIME 2.6f

enum ID {
    FLEET,
    THRUSTERS0,
    THRUSTERS1,
    THRUSTERS2,
    THRUSTERS3,
    THRUSTERS4,
    THRUSTERS5,
    THRUSTERS6,
    THRUSTERS7,
    THRUSTERS8,
    THRUSTERS9,
    THRUSTERS10,
    THRUSTERS11,
    WARP,
    END,
};

Anim ta1(5,
    IMG_STARTGR_FL5_1_5, IMG_STARTGR_FL5_1_4, IMG_STARTGR_FL5_1_3,
    IMG_STARTGR_FL5_1_2, IMG_STARTGR_FL5_1_1);

Anim ta2(5,
    IMG_STARTGR_FL5_2_5, IMG_STARTGR_FL5_2_4, IMG_STARTGR_FL5_2_3,
    IMG_STARTGR_FL5_2_2, IMG_STARTGR_FL5_2_1);

Anim ta3(5,
    IMG_STARTGR_FL5_3_5, IMG_STARTGR_FL5_3_4, IMG_STARTGR_FL5_3_3,
    IMG_STARTGR_FL5_3_2, IMG_STARTGR_FL5_3_1);

Anim ta4(5,
    IMG_STARTGR_FL5_4_5, IMG_STARTGR_FL5_4_4, IMG_STARTGR_FL5_4_3,
    IMG_STARTGR_FL5_4_2, IMG_STARTGR_FL5_4_1);

Anim ta5(5,
    IMG_STARTGR_FL5_5_5, IMG_STARTGR_FL5_5_4, IMG_STARTGR_FL5_5_3,
    IMG_STARTGR_FL5_5_2, IMG_STARTGR_FL5_5_1);

Anim ta6(5,
    IMG_STARTGR_FL5_6_5, IMG_STARTGR_FL5_6_4, IMG_STARTGR_FL5_6_3,
    IMG_STARTGR_FL5_6_2, IMG_STARTGR_FL5_6_1);

Anim ta7(5,
    IMG_STARTGR_FL5_7_5, IMG_STARTGR_FL5_7_4, IMG_STARTGR_FL5_7_3,
    IMG_STARTGR_FL5_7_2, IMG_STARTGR_FL5_7_1);

Anim ta8(5,
    IMG_STARTGR_FL5_8_5, IMG_STARTGR_FL5_8_4, IMG_STARTGR_FL5_8_3,
    IMG_STARTGR_FL5_8_2, IMG_STARTGR_FL5_8_1);

Anim ta9(5,
    IMG_STARTGR_FL5_9_5, IMG_STARTGR_FL5_9_4, IMG_STARTGR_FL5_9_3,
    IMG_STARTGR_FL5_9_2, IMG_STARTGR_FL5_9_1);

Anim ta10(5,
    IMG_STARTGR_FL5_10_5, IMG_STARTGR_FL5_10_4, IMG_STARTGR_FL5_10_3,
    IMG_STARTGR_FL5_10_2, IMG_STARTGR_FL5_10_1);

Anim ta11(5,
    IMG_STARTGR_FL5_11_5, IMG_STARTGR_FL5_11_4, IMG_STARTGR_FL5_11_3,
    IMG_STARTGR_FL5_11_2, IMG_STARTGR_FL5_11_1);

Anim ta12(5,
    IMG_STARTGR_FL5_12_5, IMG_STARTGR_FL5_12_4, IMG_STARTGR_FL5_12_3,
    IMG_STARTGR_FL5_12_2, IMG_STARTGR_FL5_12_1);

Anim thrust_anims[] = {ta1, ta2, ta3, ta4,
                       ta5, ta6, ta7, ta8,
                       ta9, ta10, ta11, ta12};

const int thrust_pos[] = {
    225,  98,
     40, 100,
     60, 100,
     80, 100,
    100, 100,
    120, 100,
    140, 100,
    160, 100,
    180, 100,
    200, 100,
    220, 100,
    240, 100,
};

Fly::Fly() : ModeBase("Fly") {
    time = 0;
    arriving = false;
    current_thrust = 0;
}

void Fly::enter() {
    ModeBase::enter(ID::END);
    time = 0;
    //PlayerInfo *player = exostate.get_active_player();
    //arriving = !(player->location.in_flight());

    draw_manager.draw(IMG_STARTGR_FL6_STARS);
    draw_manager.save_background();
    current_thrust = 0;
}

ExodusMode Fly::update(float delta) {
    if (input_manager.consume(K_Space)) {
        return ExodusMode::MODE_GalaxyMap;
    }

    time += delta;

    if (arriving) {
        float progress = time / FLY_TIME;
        if (progress > 1) progress = 1;

        if (time > FLY_TIME + 1) {
            return ExodusMode::MODE_GalaxyMap;
        }
    } else {
        float fly_progress = 0.f;

        if (time > FLY_START) {
            fly_progress = (time - FLY_START) / FLY_TIME;
            fly_progress = fly_progress < 1 ? fly_progress : 1;
        }

        float fleet_scale = FLY_SCALE * (1.f - fly_progress);

        draw_manager.draw(
            id(ID::FLEET),
            IMG_STARTGR_FL5_FLEET,
            {RES_X/2, 240,
            0.5, 0.5,
            fleet_scale, fleet_scale});

        if (time < FLY_START) {
            for (int i = current_thrust; i < N_THRUSTERS; ++i) {
                if (time < THRUST_DELAY * (i+1)) {
                    break;
                }
                float thrust_progress = (time - (i+1)*THRUST_DELAY) / THRUST_TIME;
                thrust_progress = thrust_progress < 1 ? thrust_progress : 1;
                if (thrust_progress == 1) {
                    draw_manager.draw(id(ID::THRUSTERS0 + i), nullptr);
                    ++current_thrust;
                    continue;
                }

                int x = thrust_pos[i*2];
                int y = thrust_pos[i*2 + 1];

                draw_manager.draw(
                    id(ID::THRUSTERS0 + 1),
                    thrust_anims[i].interp(thrust_progress),
                    {x, y, 0.5, 0.5, FLY_SCALE, FLY_SCALE});
            }
        }

    }

    return ExodusMode::MODE_None;
}
