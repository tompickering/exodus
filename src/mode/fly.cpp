#include "fly.h"

#include "galaxy/flytarget.h"

#include "anim.h"
#include "assetpaths.h"

#define N_THRUSTERS 12
#define FLY_SCALE 1.76f

#define THRUST_DELAY 0.1f
#define THRUST_TIME 3.f //0.4f
#define FLY_START (THRUST_DELAY*N_THRUSTERS + THRUST_TIME + 0.3f)
#define FLY_TIME 1.8f

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

const float thrust_pos[] = {
    224,  99, // OK
     77, 155, // OK
    233, 168, // OK
    143, 236, // OK
    456, 171, // OKish
    266, 358, // OKish
    140, 100,
    160, 100,
    180, 100,
    200, 100,
    220, 100,
    569, 364, // OK
};

Fly::Fly() : ModeBase("Fly"), PanelDrawer(PNL_Galaxy) {
    time = 0;
    arriving = false;
    current_thrust = 0;
}

void Fly::enter() {
    ModeBase::enter(ID::END);
    time = 0;
    PlayerInfo *player = exostate.get_active_player();
    FlyTarget *tgt = exostate.loc2tgt(player->location.get_target());
    //arriving = !(player->location.in_flight());

    draw_manager.draw(IMG_STARTGR_FL6_STARS);

    draw_manager.fill({0, 0, RES_X, 6}, COL_BORDERS);
    draw_manager.fill({0, 404, RES_X, 6}, COL_BORDERS);
    draw_manager.fill({0, 0, 6, 404}, COL_BORDERS);
    draw_manager.fill({RES_X - 6, 0, 6, 404}, COL_BORDERS);

    draw_panel_bg(TGT_Primary);
    update_panel_info_player(TGT_Primary, player);
    update_panel_info_ft(TGT_Primary, player, tgt);

    draw_manager.save_background();
    current_thrust = 0;
}

ExodusMode Fly::update(float delta) {
    if (input_manager.consume(K_Space)) {
        return ExodusMode::MODE_Pop;
    }

    time += delta;

    if (arriving) {
        float progress = time / FLY_TIME;
        if (progress > 1) progress = 1;

        if (time > FLY_TIME + 1) {
            return ExodusMode::MODE_Pop;
        }
    } else {
        float fly_progress = 0.f;

        if (time > FLY_START) {
            fly_progress = (time - FLY_START) / FLY_TIME;
            fly_progress = fly_progress < 1 ? fly_progress : 1;
        }

        // Remove completed thrusters from the screen
        for (int i = 0; i < current_thrust; ++i) {
            draw_manager.draw(id(ID::THRUSTERS0 + i), nullptr);
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

                if (thrust_progress == 1)
                    current_thrust++;

                int x = thrust_pos[i*2];
                int y = thrust_pos[i*2 + 1];

                draw_manager.draw(
                    id(ID::THRUSTERS0 + i),
                    thrust_anims[i].interp(thrust_progress),
                    {x, y, 0.5, 0.5, FLY_SCALE + 0.02, FLY_SCALE + 0.02});
            }
        }

        if (time > FLY_START + FLY_TIME + 2) {
            return ExodusMode::MODE_Pop;
        }

    }

    return ExodusMode::MODE_None;
}
