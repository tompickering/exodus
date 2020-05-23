#include "galaxy_map.h"

#include "assetpaths.h"

#define BLINK_TIME 0.5

enum ID {
    SELECTED,
    FLEET_MARKER,
    END,
};

GalaxyMap::GalaxyMap() : ModeBase("GalaxyMap"), GalaxyDrawer(), PanelDrawer(PNL_Galaxy), CommPanelDrawer() {
    stage = GM_SwapIn;
    selected_ft = nullptr;
    selected_ft_blink = 0;
}

void GalaxyMap::enter() {
    ModeBase::enter(ID::END);
    draw_galaxy(false);

    DrawTarget tgt = TGT_Primary;
    if (exodus.get_prev_mode() == ExodusMode::MODE_GalaxyGen) {
        tgt = TGT_Secondary;
    }

    draw_panel_bg(tgt);

    if (tgt != TGT_Primary) {
        draw_manager.pixelswap_start(&galaxy_panel_area);
    }

    stage = GM_SwapIn;

    selected_ft = nullptr;
}

const float FADE_SPEED = 10.f;

ExodusMode GalaxyMap::update(float delta) {
    int draw_x, draw_y;
    FlyTarget *ft;
    SpriteClick click;
    CommAction action;

    if (draw_manager.pixelswap_active() || draw_manager.fade_active()) {
        return ExodusMode::MODE_None;
    }

    Galaxy *gal = exostate.get_galaxy();
    Player *player = exostate.get_active_player();

    selected_ft_blink += delta;
    while (selected_ft_blink > 2*BLINK_TIME)
        selected_ft_blink -= 2*BLINK_TIME;

    switch (stage) {
        case GM_SwapIn:
            draw_manager.save_background();
            stage = GM_Idle;
            break;
        case GM_Idle:
            if (player->get_race() == RACE_Human && !player->intro_seen()) {
                exostate.set_active_flytarget(gal->get_guild());
                return ExodusMode::MODE_GalaxyIntro;
            }

            draw_manager.show_cursor(true);

            ft = get_clicked_flytarget();

            if (ft) {
                exostate.set_active_flytarget(ft);
            }

            if (!selected_ft) {
                ft = exostate.get_active_flytarget();
            }

            if (ft && ft != selected_ft) {
                draw_manager.draw(id(ID::SELECTED), nullptr);
                selected_ft = ft;
                selected_ft_blink = BLINK_TIME;
            }

            if (selected_ft && selected_ft_blink >= BLINK_TIME) {
                get_draw_position(selected_ft, draw_x, draw_y);
                draw_manager.draw(
                        id(ID::SELECTED),
                        IMG_TS1_MK1,
                        {draw_x, draw_y, 0.5, 0.5, 1, 1});
            } else {
                draw_manager.draw(id(ID::SELECTED), nullptr);
            }

            update_panel_info_player(TGT_Primary, player);
            update_panel_info_ft(TGT_Primary, player, selected_ft);

            if (!player->get_location().in_flight()) {
                FlyTarget *fleet_pos = exostate.loc2tgt(player->get_location().get_target());
                get_draw_position(fleet_pos, draw_x, draw_y);
                draw_manager.draw(
                        id(ID::FLEET_MARKER),
                        player->get_fleet_marker(),
                        {draw_x - 10, draw_y + 10, 0.5, 0.5, 1, 1});
            } else {
                draw_manager.draw(
                        id(ID::FLEET_MARKER),
                        nullptr);
            }

            click = draw_manager.query_click(id_panel);
            if (click.id) {
                if (click.x < 0.25) {
                    // Fly
                    comm_set_title("Message from counsellor");
                    comm_set_img_caption("COUNSELLOR");
                    comm_set_text(0, "For our flight to the star");
                    comm_set_text(1, "????, we need X months.");
                    comm_set_text(2, "A pirate attack is unlikely.");
                    comm_set_text(4, "Do you wish to start?");
                    comm_open();
                    stage = GM_FlyConfirm;
                    return ExodusMode::MODE_None;
                } else if (click.x < 0.5) {
                    L.debug("Panel 1");
                } else if (click.x < 0.75) {
                    L.debug("Panel 2");
                } else {
                    // Zoom
                    if (player->get_location().has_visited(exostate.tgt2loc(selected_ft))) {
                        exostate.set_active_flytarget(selected_ft);
                        exostate.set_active_planet(-1);
                        draw_manager.show_cursor(false);
                        draw_manager.fade_black(1.2f, 24);
                        if (selected_ft == gal->get_guild()) {
                            stage = GM_Zoom2Guild;
                        } else {
                            stage = GM_Zoom2Star;
                        }
                    } else {
                        L.debug("Can't zoom - not visited");
                    }
                }
            }
            break;
        case GM_Zoom2Guild:
            return ExodusMode::MODE_GuildExterior;
        case GM_Zoom2Star:
            return ExodusMode::MODE_StarMap;
        case GM_FlyConfirm:
            action = comm_check_action();
            if (action == CA_Proceed) {
                // TODO: Vary number of months
                player->get_location().set_target(exostate.tgt2loc(selected_ft), 1);
                player->get_location().advance();
                return ExodusMode::MODE_Fly;
            } else if (action == CA_Abort) {
                comm_close();
                return ExodusMode::MODE_Reload;
            }
            break;
        case GM_MonthPassing:
            return ExodusMode::MODE_None;
        default:
            break;
    }

    return ExodusMode::MODE_None;
}

void GalaxyMap::exit() {
    comm_ensure_closed();
    ModeBase::exit();
}
