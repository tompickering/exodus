#include "alien_vessel.h"

#include "util/value.h"

#include "assetpaths.h"

enum ID {
    BACKGROUND,
    INTRO_0,
    INTRO_1,
    VESSEL,
    CORNER_TL,
    CORNER_TR,
    CORNER_BL,
    CORNER_BR,
    SHIP_ID0,
    SHIP_ID1,
    SHIP_ID2,
    END,
};

AlienVessel::AlienVessel() : ModeBase("AlienVessel"), PanelDrawer(PNL_Galaxy), FrameDrawer() {
    approach = 0;
    ship_img = nullptr;
    ship_name = nullptr;
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
            ship_name = "NO ID";
            break;
        case VESSEL_GuildPatrol:
            ship_img = IMG_VS3_VESSEL;
            ship_name = "GUILD PATROL SHIP";
            break;
        case VESSEL_Trading:
            ship_img = IMG_VS1_VESSEL;
            ship_name = "TRADING SHIP";
            break;
        case VESSEL_TradingConvoy:
            ship_img = IMG_VS1_VESSEL;
            ship_name = "TRADING CONVOY";
            break;
        case VESSEL_Travelling:
            ship_img = IMG_VS6_VESSEL;
            ship_name = "TRAVELLING SHUTTLE";
            break;
        case VESSEL_Science:
            ship_img = IMG_VS4_VESSEL;
            ship_name = "SCIENCE SHIP";
            break;
        case VESSEL_Mining:
            ship_img = IMG_VS1_VESSEL;
            ship_name = "MINING SHIP";
            break;
        case VESSEL_Religious:
            ship_img = IMG_VS2_VESSEL;
            ship_name = "RELIGIOUS VESSEL";
            break;
        case VESSEL_Unknown:
            ship_img = IMG_VS5_VESSEL;
            ship_name = "UNKNOWN TYPE";
            break;
    }

    draw_manager.draw(
        id(ID::BACKGROUND),
        IMG_VS0_VPIC,
        {6, 8, 0, 0, 1, 1});

    draw_manager.draw_text(
        id(ID::INTRO_0),
        "While leaving the solar system,",
        Justify::Left,
        20, 20,
        COL_TEXT2);
    draw_manager.draw_text(
        id(ID::INTRO_1),
        "we have discovered an alien spacecraft.",
        Justify::Left,
        20, 40,
        COL_TEXT2);

    frame_draw();

    draw_panel_bg(TGT_Primary);
    update_panel_info_player(TGT_Primary, player);
    update_panel_info_ft(TGT_Primary, player, tgt);

    draw_manager.show_cursor(true);
}

ExodusMode AlienVessel::update(float delta) {
    switch (stage) {
        case AV_Approach:
            {
                approach = fclamp(approach+delta*0.25, 0, 1);

                if (draw_manager.clicked()) {
                    approach = 1.f;
                }

                int ship_y = 200;
                draw_manager.draw(
                    id(ID::VESSEL),
                    ship_img,
                    {RES_X/2, ship_y,
                     .5, .5, approach, approach});

                if (approach >= 1) {
                    draw_manager.draw(
                        id(ID::CORNER_TL),
                        IMG_TS1_M25,
                        {RES_X/2 - 100, ship_y - 80,
                         .5, .5, 1, 1});
                    draw_manager.draw(
                        id(ID::CORNER_TR),
                        IMG_TS1_M26,
                        {RES_X/2 + 100, ship_y - 80,
                         .5, .5, 1, 1});
                    draw_manager.draw(
                        id(ID::CORNER_BL),
                        IMG_TS1_M27,
                        {RES_X/2 - 100, ship_y + 80,
                         .5, .5, 1, 1});
                    draw_manager.draw(
                        id(ID::CORNER_BR),
                        IMG_TS1_M28,
                        {RES_X/2 + 100, ship_y + 80,
                         .5, .5, 1, 1});
                    draw_manager.draw_text(
                        Font::Tiny,
                        id(ID::SHIP_ID0),
                        "SPACECRAFT",
                        Justify::Left,
                        460, 140,
                        COL_TEXT_SPEECH);
                    draw_manager.draw_text(
                        Font::Tiny,
                        id(ID::SHIP_ID1),
                        "IDENTIFICATION:",
                        Justify::Left,
                        460, 155,
                        COL_TEXT_SPEECH);
                    draw_manager.draw_text(
                        Font::Tiny,
                        id(ID::SHIP_ID2),
                        ship_name,
                        Justify::Left,
                        460, 185,
                        COL_TEXT_SPEECH);
                    stage = AV_Idle;
                }
            }
            break;
        case AV_Idle:
            {
                SpriteClick clk = draw_manager.query_click(id(ID::BACKGROUND));
                if (clk.id) {
                    if (clk.y > 0.92f) {
                        stage = AV_Exit;
                    }
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
