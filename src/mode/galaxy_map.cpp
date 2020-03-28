#include "galaxy_map.h"

#include "assetpaths.h"

#define PNL_BORDER 6

#define BLINK_TIME 0.5

enum ID {
    PANEL,
    SELECTED,
    FLEET_MARKER,
    FLYTARGET_DESC,
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
        190,
        galaxy_panel_area.h - PNL_BORDER * 2};
    area_starinfo = {
        area_playerinfo.x + area_playerinfo.w + PNL_BORDER,
        galaxy_panel_area.y + PNL_BORDER,
        RES_X - (area_playerinfo.x + area_playerinfo.w + PNL_BORDER) - PNL_BORDER,
        44};
    ModeBase::enter(ID::END);
    draw_galaxy(false);
    draw_manager.fill(TGT_Secondary, galaxy_panel_area, {0xA0, 0xA0, 0xA0});
    draw_manager.pattern_fill(TGT_Secondary, area_playerinfo);
    draw_manager.pattern_fill(TGT_Secondary, area_starinfo);
    draw_manager.draw(
            TGT_Secondary,
            id(ID::PANEL),
            IMG_BR9_EXPORT,
            {RES_X, RES_Y, 1, 1, 1, 1});
    int y_sep = 16;
    draw_manager.draw_text(
            TGT_Secondary,
            Font::Small,
            "Test Name",
            Justify::Left,
            area_playerinfo.x + 4,
            area_playerinfo.y + 2,
            {0xFF, 0xFF, 0xFF});
    draw_manager.draw_text(
            TGT_Secondary,
            Font::Small,
            "Month: ",
            Justify::Left,
            area_playerinfo.x + 4,
            area_playerinfo.y + 2 + y_sep,
            {0xFF, 0xFF, 0xFF});
    draw_manager.draw_text(
            TGT_Secondary,
            Font::Small,
            "MCredits: ",
            Justify::Left,
            area_playerinfo.x + 4,
            area_playerinfo.y + 2 + 2*y_sep,
            {0xFF, 0xFF, 0xFF});
    draw_manager.draw_text(
            TGT_Secondary,
            Font::Small,
            "Planets: ",
            Justify::Left,
            area_playerinfo.x + 4,
            area_playerinfo.y + 2 + 3*y_sep,
            {0xFF, 0xFF, 0xFF});
    draw_manager.pixelswap_start(&galaxy_panel_area);
    stage = GM_SwapIn;

    selected_ft = nullptr;
}

const float FADE_SPEED = 10.f;

ExodusMode GalaxyMap::update(float delta) {
    int draw_x, draw_y;
    FlyTarget *ft;
    SpriteClick click;

    if (draw_manager.pixelswap_active()) {
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
                char ft_desc[41];
                bool is_guild = ft == gal->get_guild();
                snprintf(ft_desc, 40, "This is the %s%s.", is_guild ? "" : "star ", ft->name);
                selected_ft = ft;
                selected_ft_blink = BLINK_TIME;
                L.debug("%s", ft_desc);
                draw_manager.draw_text(
                    id(ID::FLYTARGET_DESC),
                    (const char*) ft_desc,
                    Justify::Left,
                    area_starinfo.x + 4,
                    area_starinfo.y + 2,
                    {0xFF, 0xFF, 0xFF});
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
                        nullptr,
                        {draw_x - 10, draw_y - 10, 0.5, 0.5, 1, 1});
            }

            click = draw_manager.query_click(id(ID::PANEL));
            if (click.id) {
                if (click.x < 0.25) {
                    L.debug("Panel 0");
                } else if (click.x < 0.5) {
                    L.debug("Panel 1");
                } else if (click.x < 0.75) {
                    L.debug("Panel 2");
                } else {
                    L.debug("Panel 3");
                }
            }
            break;
        default:
            break;
    }

    return ExodusMode::MODE_None;
}
