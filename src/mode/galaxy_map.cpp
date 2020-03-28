#include "galaxy_map.h"

#include "assetpaths.h"

#define PNL_BORDER 6

#define BLINK_TIME 0.5

enum ID {
    PANEL,
    SELECTED,
    END,
};

GalaxyMap::GalaxyMap() : ModeBase("GalaxyMap"), GalaxyDrawer() {
    stage = GM_SwapIn;
    selected_ft = nullptr;
    selected_ft_blink = 0;
}

void GalaxyMap::enter() {
    Galaxy *gal = exostate.get_galaxy();

    DrawArea area_playerinfo = {
        galaxy_panel_area.x + PNL_BORDER,
        galaxy_panel_area.y + PNL_BORDER,
        190,
        galaxy_panel_area.h - PNL_BORDER * 2};
    DrawArea area_starinfo = {
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

    selected_ft = gal->get_guild();
}

const float FADE_SPEED = 10.f;

ExodusMode GalaxyMap::update(float delta) {
    int draw_x, draw_y;
    FlyTarget *ft;
    SpriteClick click;

    PlayerInfo *player = exostate.get_active_player();

    if (draw_manager.pixelswap_active()) {
        return ExodusMode::MODE_None;
    }

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
            if (ft) {
                L.debug("Clicked %s", ft->name);
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
