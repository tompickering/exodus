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

    comm_started = false;
    comm_done = false;
    comm_timer = 0.f;
    comm_line = 0;
    surrender_done = false;
}

void AlienVessel::enter() {
    ModeBase::enter(ID::END);
    Player *player = exostate.get_active_player();
    FlyTarget *tgt = exostate.get_active_flytarget();

    for (int i = 0; i < MAX_COMM_LINES; ++i) {
        comm_ids[i] = draw_manager.new_sprite_id();
    }

    stage = AV_Approach;

    approach = 0;
    comm_started = false;
    comm_done = false;
    comm_timer = 0.f;
    comm_line = 0;
    surrender_done = false;

    will_respond = true;
    will_hail = false;
    will_attack = false;
    will_surrender = false;

    enemy_ships = 1;
    enemy_cargo = 0;
    enemy_scouts = 0;

    int musidx = rand() % 3;
    if (musidx == 0) {
        audio_manager.target_music(mpart2mus(4));
    } else if (musidx == 1) {
        audio_manager.target_music(mpart2mus(14));
    } else {
        audio_manager.target_music(mpart2mus(16));
    }

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

    set_comm_text();

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

void AlienVessel::exit() {
    for (int i = 0; i < MAX_COMM_LINES; ++i) {
        draw_manager.release_sprite_id(comm_ids[i]);
    }
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
                if (!comm_started) {
                    clear_overlay();

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

                    comm_started = true;
                }

                if (comm_done) {
                    stage = AV_Idle;
                    break;
                }

                // TODO - Gradual speech
                for (int i = 0; i < MAX_COMM_LINES; ++i) {
                    draw_manager.draw_text(
                        comm_ids[i],
                        comm_text[i],
                        Justify::Left,
                        20, 60 + 20*i,
                        COL_TEXT2);
                }

                comm_done = true;
            }
            break;
        case AV_Attack:
            {
                if (type != VESSEL_NoID) {
                    player->add_trace(TRACE_PeacefulShipsAttacked);
                }

                if (type == VESSEL_GuildPatrol) {
                    // punish%(1,1)=1
                    player->commit_infraction(INF_AttackGuildShip);
                }

                if (will_surrender) {
                    stage = AV_Surrender;
                    break;
                }

                // Launch battle
                SpaceBattleParams &b = ephstate.space_battle;
                b.enemy_ships = enemy_ships;
                b.enemy_cargo = enemy_cargo;
                b.enemy_scouts = enemy_scouts;
                return ExodusMode::MODE_SpaceBattle;
            }
            break;
        case AV_Surrender:
            {
                if (!surrender_done) {
                    clear_overlay();
                    draw_manager.draw_text(
                        "Your enemy is not armed.",
                        Justify::Left,
                        20, 20,
                        COL_TEXT);

                    Fleet &f = player->get_fleet_nonconst();

                    const char *cap_str = "";

                    // FIXME: Do we care about maxima here?
                    if (type == VESSEL_Trading) {
                        f.transporters++;
                        cap_str = "You have capptured a transporter ship.";
                        switch (rand() % 3) {
                            case 0:
                                player->transfer_min(1);
                                break;
                            case 1:
                                player->transfer_fd(1);
                                break;
                            case 2:
                                player->transfer_plu(1);
                                break;
                        }
                    } else {
                        cap_str = "You have capptured a scout ship.";
                        f.scouts++;
                    }

                    draw_manager.draw_text(
                        cap_str,
                        Justify::Left,
                        20, 40,
                        COL_TEXT);

                    surrender_done = true;
                }

                if (draw_manager.clicked()) {
                    stage = AV_Exit;
                }
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

void AlienVessel::clear_overlay() {
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
    draw_manager.draw(id(ID::COMM_RESULT), nullptr);
    for (int i = 0; i < MAX_COMM_LINES; ++i) {
        draw_manager.draw(comm_ids[i], nullptr);
    }
}

void AlienVessel::set_comm_text() {
    for (int i = 0; i < MAX_COMM_LINES; ++i) {
        comm_text[i] = "";
    }

    switch (type) {
        case VESSEL_NoID:
            // This isn't actually used, but left in from original for fun
            comm_text[0] = "Mylord!";
            comm_text[1] = "We have come to rob your Mega Credits";
            comm_text[2] = "and borrow some of your pretty transport ships!";
            comm_text[3] = "So prepare for a little fight!";
            break;
        case VESSEL_GuildPatrol:
            comm_text[0] = "This is a Space Guild Patrol Flight.";
            comm_text[1] = "Have a nice day, space fleet.";
            break;
        case VESSEL_Trading:
            // TODO: Use player referece
            comm_text[0] = "Mylord!";
            comm_text[1] = "We are very sorry to disturb your flight.";
            comm_text[2] = "You shall move on with our best wishes!";
            break;
        case VESSEL_TradingConvoy:
            comm_text[0] = "Our trading convoy would be glad if you let us pass.";
            break;
        case VESSEL_Travelling:
            comm_text[0] = "Hello space fleet!";
            comm_text[1] = "This is a private travelling shuttle of InterSpace Inc.";
            comm_text[2] = "It is fully licensed and carries no prohibited cargo.";
            comm_text[3] = "We would be glad if you allowed us to continue our flight.";
            break;
        case VESSEL_Science:
            comm_text[0] = "This is an independent scientific ship.";
            comm_text[1] = "Please continue your flight; we will not interfere.";
            break;
        case VESSEL_Mining:
            // TODO: Use player reference
            comm_text[0] = "What do you want, Mylord?";
            comm_text[2] = "This mining vessel is on a registered flight.";
            comm_text[3] = "I hope you do not complain if we move on.";
            break;
        case VESSEL_Religious:
            comm_text[0] = "PRAISE THE ALMIGHTY!";
            comm_text[2] = "There is a FORCE that has created this WONDERFUL";
            comm_text[3] = "universe! It is our MISSION to serve this mighty";
            comm_text[4] = "being! So we are missionaries with the task to convince";
            comm_text[5] = "ALL LIVING CREATURES to BELIEVE in the Almighty!";
            comm_text[7] = "SO HELP US! Be our fellow on the PATH OF GLORY,";
            comm_text[8] = "be our friend and our ally on the WAY OF HAPPINESS!";
            comm_text[9] = "Praise the Almighty!";
            break;
        case VESSEL_Unknown:
            break;
    }
}
