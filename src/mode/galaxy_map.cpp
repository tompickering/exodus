#include "galaxy_map.h"

#include "assetpaths.h"

#define PNL_BORDER 6

enum ID {
    PANEL,
    END,
};

GalaxyMap::GalaxyMap() : ModeBase("GalaxyMap"), GalaxyDrawer() {
    stage = GM_SwapIn;
}

void GalaxyMap::enter() {
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
    draw_manager.pixelswap_start();
    stage = GM_SwapIn;
}

const float FADE_SPEED = 10.f;

ExodusMode GalaxyMap::update(float delta) {
    FlyTarget *ft;
    SpriteClick click;

    if (draw_manager.pixelswap_active()) {
        return ExodusMode::MODE_None;
    }

    switch (stage) {
        case GM_SwapIn:
            draw_manager.save_background();
            draw_manager.show_cursor(true);
            stage = GM_Idle;
            break;
        case GM_Idle:
            ft = get_clicked_flytarget();
            if (ft) {
                L.debug("Clicked %s", ft->name);
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
