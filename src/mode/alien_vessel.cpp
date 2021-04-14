#include "alien_vessel.h"

#include "util/value.h"

#include "assetpaths.h"

enum ID {
    VESSEL,
    END,
};

AlienVessel::AlienVessel() : ModeBase("AlienVessel"), PanelDrawer(PNL_Galaxy), FrameDrawer() {
    approach = 0;
    ship_img = nullptr;
}

void AlienVessel::enter() {
    ModeBase::enter(ID::END);
    Player *player = exostate.get_active_player();
    FlyTarget *tgt = exostate.loc2tgt(player->get_location().get_target());

    stage = AV_Approach;

    approach = 0;

    int r = RND(23);

         if (r <  4) type = VESSEL_NoID;
    else if (r <  8) type = VESSEL_GuildPatrol;
    else if (r < 13) type = VESSEL_Trading;
    else if (r < 15) type = VESSEL_TradingConvoy;
    else if (r < 18) type = VESSEL_Travelling;
    else if (r < 20) type = VESSEL_Science;
    else if (r < 22) type = VESSEL_Mining;
    else if (r < 23) type = VESSEL_Religious;
    else             type = VESSEL_Unknown;

    switch (type) {
        case VESSEL_NoID:
            ship_img = IMG_VS3_VESSEL;
            break;
        case VESSEL_GuildPatrol:
            ship_img = IMG_VS3_VESSEL;
            break;
        case VESSEL_Trading:
            ship_img = IMG_VS1_VESSEL;
            break;
        case VESSEL_TradingConvoy:
            ship_img = IMG_VS1_VESSEL;
            break;
        case VESSEL_Travelling:
            ship_img = IMG_VS3_VESSEL;
            break;
        case VESSEL_Science:
            ship_img = IMG_VS4_VESSEL;
            break;
        case VESSEL_Mining:
            ship_img = IMG_VS1_VESSEL;
            break;
        case VESSEL_Religious:
            ship_img = IMG_VS2_VESSEL;
            break;
        case VESSEL_Unknown:
            ship_img = IMG_VS5_VESSEL;
            break;
    }

    draw_manager.draw(IMG_VS0_VPIC);
    draw_manager.save_background();

    frame_draw();

    draw_panel_bg(TGT_Primary);
    update_panel_info_player(TGT_Primary, player);
    update_panel_info_ft(TGT_Primary, player, tgt);
}

ExodusMode AlienVessel::update(float delta) {
    switch (stage) {
        case AV_Approach:
            {
                approach = fclamp(approach+delta*0.25, 0, 1);

                if (draw_manager.clicked()) {
                    approach = 1.f;
                }

                draw_manager.draw(
                    id(ID::VESSEL),
                    ship_img,
                    {RES_X/2, 200, .5, .5, approach, approach});

                if (approach >= 1) {
                    stage = AV_Idle;
                }
            }
            break;
        case AV_Idle:
            {
                if (draw_manager.clicked()) {
                    stage = AV_Exit;
                }
            }
            break;
        case AV_Comm:
            {
            }
            break;
        case AV_Exit:
            {
                ephstate.set_ephemeral_state(EPH_ResumeFly);
                return ExodusMode::MODE_Pop;
            }
            break;
    }

    frame_draw();

    return ExodusMode::MODE_None;
}
