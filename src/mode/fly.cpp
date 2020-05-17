#include "fly.h"

#include <cmath>

#include "galaxy/flytarget.h"

#include "anim.h"
#include "assetpaths.h"

#define N_THRUSTERS 12
#define FLY_SCALE 1.76f

#define THRUST_DELAY 0.1f
#define THRUST_TIME 0.4f
#define FLY_START (THRUST_DELAY*N_THRUSTERS + THRUST_TIME + 0.3f)
#define FLY_TIME 1.8f
#define WARP_X RES_X / 2
#define WARP_Y 200
#define WARP_START (FLY_START + FLY_TIME + 0.7)
#define WARP_TIME 1.8
#define WARP_STAGES 10

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

Anim anim_warp(
    5,
    IMG_HP1_HYP1,
    IMG_HP1_HYP2,
    IMG_HP1_HYP3,
    IMG_HP1_HYP4,
    IMG_HP1_HYP5

);

const float thrust_pos[] = {
    222,  59, // Small, top-left
     75, 115, // Medium, top-left
    233, 129, // Small, up-left of centre
    143, 197, // Small, middle-left
    456, 132, // Small, up-right of centre
    266, 320, // Small, lower-left
    566, 177, // Medium, right
    519, 5,   // Large, top-right
    393, 92,  // Medium, up-up-right of centre
    507, 137, // Medium, right-up-right of centre
    155, 268, // Medium, lower-left
    569, 325, // Large, lower-right
};

Fly::Fly() : ModeBase("Fly"), PanelDrawer(PNL_Galaxy) {
    time = 0;
    arriving = false;
    current_thrust = 0;
    warp_stage = 0;
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
    warp_stage = 0;
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
            for (int i = 0; i < N_THRUSTERS ; ++i) {
                draw_manager.draw(id(ID::THRUSTERS0 + i), nullptr);
            }
            fly_progress = (time - FLY_START) / FLY_TIME;
            fly_progress = fly_progress < 1 ? fly_progress : 1;
        }

        float fleet_scale = FLY_SCALE * (1.f - fly_progress);

        draw_manager.draw(
            id(ID::FLEET),
            IMG_STARTGR_FL5_FLEET,
            {RES_X/2, 200,
            0.5, 0.5,
            fleet_scale, fleet_scale});

        if (time < FLY_START) {
            for (int i = current_thrust; i < N_THRUSTERS; ++i) {
                float thrust_progress = (time - (i+1)*THRUST_DELAY) / THRUST_TIME;
                thrust_progress = thrust_progress > 0 ? thrust_progress : 0;
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

        if (time >= WARP_START) {
            float interp = (time - WARP_START) / WARP_TIME;
            interp = interp > 1 ? 1 : interp;
            float stage_interp = warp_stage / (float)WARP_STAGES;
            if (interp > stage_interp) {
                ++warp_stage;
                stage_interp = warp_stage / (float)WARP_STAGES;
                float scale_interp = stage_interp < 0.5 ? stage_interp : 1 - stage_interp;
                float scale = 8 * pow(scale_interp, 2);
                draw_manager.draw(
                    id(ID::WARP),
                    anim_warp.frame(warp_stage),
                    {WARP_X, WARP_Y,
                     0.5, 0.5, scale, scale});
            }
        }


        if (time > WARP_START + WARP_TIME + 1.0) {
            return ExodusMode::MODE_Pop;
        }

    }

    draw_manager.fill({0, 0, RES_X, 6}, COL_BORDERS);
    draw_manager.fill({0, 404, RES_X, 6}, COL_BORDERS);
    draw_manager.fill({0, 0, 6, 404}, COL_BORDERS);
    draw_manager.fill({RES_X - 6, 0, 6, 404}, COL_BORDERS);

    return ExodusMode::MODE_None;
}
