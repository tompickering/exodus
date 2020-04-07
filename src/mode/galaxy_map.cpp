#include "galaxy_map.h"

#include "assetpaths.h"

#define PNL_BORDER 4
#define PNL_Y_SEP 16

#define BLINK_TIME 0.5

enum ID {
    PANEL,
    PANEL_NAME,
    PANEL_MONTH,
    PANEL_MC,
    PANEL_PLANETS,
    SELECTED,
    FLEET_MARKER,
    FLYTARGET_DESC,
    FLYTARGET_QM,
    END,
};

DrawArea area_playerinfo;
DrawArea area_starinfo;

GalaxyMap::GalaxyMap() : ModeBase("GalaxyMap"), GalaxyDrawer() {
    stage = GM_SwapIn;
    selected_ft = nullptr;
    selected_ft_blink = 0;
}

void GalaxyMap::enter() {
    area_playerinfo = {
        galaxy_panel_area.x + PNL_BORDER,
        galaxy_panel_area.y + PNL_BORDER,
        194,
        galaxy_panel_area.h - PNL_BORDER * 2};
    area_starinfo = {
        area_playerinfo.x + area_playerinfo.w + PNL_BORDER,
        galaxy_panel_area.y + PNL_BORDER,
        RES_X - (area_playerinfo.x + area_playerinfo.w + PNL_BORDER) - PNL_BORDER,
        44};
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

void GalaxyMap::draw_panel_bg(DrawTarget tgt) {
    draw_manager.fill(tgt, galaxy_panel_area, {0xA0, 0xA0, 0xA0});
    draw_manager.pattern_fill(tgt, area_playerinfo);
    draw_manager.pattern_fill(tgt, area_starinfo);
    draw_manager.draw(
            tgt,
            id(ID::PANEL),
            IMG_BR9_EXPORT,
            {RES_X - PNL_BORDER, RES_Y - PNL_BORDER, 1, 1, 1, 1});

    draw_manager.draw_text(
            tgt,
            Font::Small,
            "Month: ",
            Justify::Left,
            area_playerinfo.x + 4,
            area_playerinfo.y + 2 + PNL_Y_SEP,
            COL_TEXT);
    draw_manager.draw_text(
            tgt,
            Font::Small,
            "MCredits: ",
            Justify::Left,
            area_playerinfo.x + 4,
            area_playerinfo.y + 2 + 2*PNL_Y_SEP,
            COL_TEXT);
    draw_manager.draw_text(
            tgt,
            Font::Small,
            "Planets: ",
            Justify::Left,
            area_playerinfo.x + 4,
            area_playerinfo.y + 2 + 3*PNL_Y_SEP,
            COL_TEXT);
}

void GalaxyMap::update_panel_info(DrawTarget tgt, PlayerInfo* player, FlyTarget* ft) {
    char month_string[5];
    char mc_string[7];
    char planets_string[3];
    Galaxy *gal = exostate.get_galaxy();

    /* Player info */
    snprintf(month_string, 5, "%u", exostate.get_month());
    if (player) {
        snprintf(mc_string, 7, "%u", player->mc);
        snprintf(planets_string, 3, "??");
    } else {
        strcpy(mc_string, "");
        strcpy(planets_string, "");
    }

    draw_manager.draw_text(
            tgt,
            id(ID::PANEL_NAME),
            Font::Small,
            player ? player->full_name : "",
            Justify::Left,
            area_playerinfo.x + 4,
            area_playerinfo.y + 2,
            COL_TEXT);
    draw_manager.draw_text(
            tgt,
            id(ID::PANEL_MONTH),
            Font::Small,
            month_string,
            Justify::Left,
            area_playerinfo.x + 60,
            area_playerinfo.y + 2 + PNL_Y_SEP,
            COL_TEXT2);
    draw_manager.draw_text(
            tgt,
            id(ID::PANEL_MC),
            Font::Small,
            mc_string,
            Justify::Left,
            area_playerinfo.x + 76,
            area_playerinfo.y + 2 + 2*PNL_Y_SEP,
            COL_TEXT2);
    draw_manager.draw_text(
            tgt,
            id(ID::PANEL_PLANETS),
            Font::Small,
            planets_string,
            Justify::Left,
            area_playerinfo.x + 68,
            area_playerinfo.y + 2 + 3*PNL_Y_SEP,
            COL_TEXT2);

    /* FlyTarget info */
    char ft_desc[41];
    bool is_guild = ft == gal->get_guild();
    snprintf(ft_desc, 40, "This is the %s%s.", is_guild ? "" : "star ", ft->name);

    // Draw '?' or star details
    if (player->location.has_visited(exostate.tgt2loc(ft))) {
        draw_manager.draw(
            id(ID::FLYTARGET_QM),
            nullptr);
    } else {
        draw_manager.draw(
            id(ID::FLYTARGET_QM),
            IMG_TS1_QMARK,
            {RES_X - 12, area_starinfo.y + 6, 1, 0, 1, 1});
    }

    draw_manager.draw_text(
        id(ID::FLYTARGET_DESC),
        (const char*) ft_desc,
        Justify::Left,
        area_starinfo.x + 4,
        area_starinfo.y + 2,
        COL_TEXT);
}

ExodusMode GalaxyMap::update(float delta) {
    int draw_x, draw_y;
    FlyTarget *ft;
    SpriteClick click;

    if (draw_manager.pixelswap_active() || draw_manager.fade_active()) {
        return ExodusMode::MODE_None;
    }

    Galaxy *gal = exostate.get_galaxy();
    PlayerInfo *player = exostate.get_active_player();

    selected_ft_blink += delta;
    while (selected_ft_blink > 2*BLINK_TIME)
        selected_ft_blink -= 2*BLINK_TIME;

    switch (stage) {
        case GM_SwapIn:
            draw_manager.save_background();
            draw_manager.show_cursor(true);
            stage = GM_Idle;
            break;
        case GM_Idle:
            if (player->human && !player->intro_seen) {
                return ExodusMode::MODE_GalaxyIntro;
            }

            ft = get_clicked_flytarget();
            if (!selected_ft) ft = gal->get_guild();
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

            update_panel_info(TGT_Primary, player, selected_ft);

            if (!player->location.in_flight()) {
                FlyTarget *fleet_pos = exostate.loc2tgt(player->location.get_target());
                get_draw_position(fleet_pos, draw_x, draw_y);
                draw_manager.draw(
                        id(ID::FLEET_MARKER),
                        player->fleet_marker,
                        {draw_x - 10, draw_y + 10, 0.5, 0.5, 1, 1});
            } else {
                draw_manager.draw(
                        id(ID::FLEET_MARKER),
                        nullptr);
            }

            click = draw_manager.query_click(id(ID::PANEL));
            if (click.id) {
                if (click.x < 0.25) {
                    // Fly
                    // TODO: Vary number of months
                    player->location.set_target(exostate.tgt2loc(selected_ft), 1);
                    return ExodusMode::MODE_Fly;
                } else if (click.x < 0.5) {
                    L.debug("Panel 1");
                } else if (click.x < 0.75) {
                    L.debug("Panel 2");
                } else {
                    // Zoom
                    if (player->location.has_visited(exostate.tgt2loc(selected_ft))) {
                        exostate.set_active_flytarget(selected_ft);
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
            break;
        case GM_Zoom2Star:
            break;
        case GM_MonthPassing:
            return ExodusMode::MODE_None;
        default:
            break;
    }

    return ExodusMode::MODE_None;
}
