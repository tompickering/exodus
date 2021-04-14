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
    NORADAR,
    COMM_RESULT,
    END,
};

AlienVessel::AlienVessel() : ModeBase("AlienVessel"), PanelDrawer(PNL_Galaxy), FrameDrawer() {
    stackable = false;

    approach = 0;
    ship_img = nullptr;
    ship_name = nullptr;
    will_respond = true;
    will_hail = false;
    will_attack = false;
    will_surrender = false;

    enemy_ships = 1;
    enemy_cargo = 0;
    enemy_scouts = 0;

    comm_done = false;
    comm_timer = 0.f;
    comm_line = 0;
}

void AlienVessel::enter() {
    ModeBase::enter(ID::END);
    Player *player = exostate.get_active_player();
    FlyTarget *tgt = exostate.get_active_flytarget();

    stage = AV_Approach;

    approach = 0;
    comm_done = false;
    comm_timer = 0.f;
    comm_line = 0;

    will_respond = true;
    will_hail = false;
    will_attack = false;
    will_surrender = false;

    enemy_ships = 1;
    enemy_cargo = 0;
    enemy_scouts = 0;

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

    int m = exostate.get_orig_month();

    switch (type) {
        case VESSEL_NoID:
            ship_img = IMG_VS3_VESSEL;
            ship_name = "NO ID";
            will_respond = false;
            will_attack = true;
            enemy_ships = RND((int)(fclamp((float)m * 1.5, 1, 250)));
            enemy_cargo = rand() % 6;
            enemy_scouts = rand() % 7;
            break;
        case VESSEL_GuildPatrol:
            ship_img = IMG_VS3_VESSEL;
            ship_name = "GUILD PATROL SHIP";
            will_hail = true;
            enemy_ships = 5;
            break;
        case VESSEL_Trading:
            ship_img = IMG_VS1_VESSEL;
            ship_name = "TRADING SHIP";
            will_surrender = true;
            break;
        case VESSEL_TradingConvoy:
            ship_img = IMG_VS1_VESSEL;
            ship_name = "TRADING CONVOY";
            enemy_ships = 25;
            enemy_cargo = 2 + RND(3);
            enemy_scouts = rand() % 3;
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
            will_hail = true;
            will_surrender = true;
            break;
        case VESSEL_Unknown:
            ship_img = IMG_VS5_VESSEL;
            ship_name = "UNKNOWN TYPE";
            will_respond = false;
            enemy_ships = RND(m+5);
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
    Player *player = exostate.get_active_player();

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
                    float pad = .15f;
                    float bw = (1.f - pad*2)/4.f;
                    if (clk.y > 0.92f) {
                        if (clk.x > pad && clk.x < (1.f - pad)) {
                            if (clk.x < pad + bw) {
                                // Comm
                                if (!comm_done) {
                                    stage = AV_Comm;
                                }
                            } else if (clk.x < pad + 2*bw) {
                                // Attack
                                stage = AV_Attack;
                            } else if (clk.x < pad + 3*bw) {
                                // Hide
                                if (player->has_invention(INV_RadarExtension)) {
                                    stage = AV_Exit;
                                } else {
                                    draw_manager.draw_text(
                                        id(ID::NORADAR),
                                        "No Radar Manipulation System fitted.",
                                        Justify::Centre,
                                        RES_X/2, 340,
                                        COL_TEXT_BAD);
                                }
                            } else {
                                // Wait
                                if (will_hail && !comm_done) {
                                    stage = AV_Comm;
                                } else {
                                    stage = will_attack ? AV_Attack : AV_Exit;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case AV_Comm:
            {
                if (!will_respond) {
                    comm_done = true;
                    draw_manager.draw_text(
                        id(ID::COMM_RESULT),
                        "The ship does not respond.",
                        Justify::Left,
                        20, 20,
                        COL_TEXT);
                } else {
                    draw_manager.draw_text(
                        id(ID::COMM_RESULT),
                        "The ship answers:",
                        Justify::Left,
                        20, 20,
                        COL_TEXT);
                }

                if (comm_done) {
                    stage = AV_Idle;
                    break;
                }

                draw_manager.draw(id(ID::INTRO_0), nullptr);
                draw_manager.draw(id(ID::INTRO_1), nullptr);
                draw_manager.draw(id(ID::CORNER_TL), nullptr);
                draw_manager.draw(id(ID::CORNER_TR), nullptr);
                draw_manager.draw(id(ID::CORNER_BL), nullptr);
                draw_manager.draw(id(ID::CORNER_BR), nullptr);
                draw_manager.draw(id(ID::SHIP_ID0), nullptr);
                draw_manager.draw(id(ID::SHIP_ID1), nullptr);
                draw_manager.draw(id(ID::SHIP_ID2), nullptr);
                draw_manager.draw(id(ID::NORADAR), nullptr);

                // TODO
                comm_done = true;
            }
            break;
        case AV_Attack:
            {
                if (type != VESSEL_NoID) {
                    // TODO: trace%(4) += 1
                }

                if (type == VESSEL_GuildPatrol) {
                    // TODO: punish%(1,1)=1
                }

                if (will_surrender) {
                    stage = AV_Surrender;
                    break;
                }

                // TODO - Launch battle with enemy_ships, enemy_cargo, enemy_scouts
                stage = AV_Exit;
            }
        case AV_Surrender:
            {
                // TODO
                stage = AV_Exit;
            }
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
