#include "panel_drawer.h"

#include "draw/draw.h"

#include "assetpaths.h"
#include "shared.h"

const int PNL_TOP        = 412;
const int PNL_BORDER     = 4;
const int PNL_Y_SEP      = 20;
const int PNL_PLAYER_PAD = 5;

const DrawArea galaxy_panel_area = {0, PNL_TOP, RES_X, RES_Y - PNL_TOP};

DrawArea area_playerinfo;
DrawArea area_starinfo;

PanelDrawer::PanelDrawer(PanelType _type) : type(_type) {
    id_panel   = draw_manager.new_sprite_id();
    id_name    = draw_manager.new_sprite_id();
    id_month   = draw_manager.new_sprite_id();
    id_mc      = draw_manager.new_sprite_id();
    id_planets = draw_manager.new_sprite_id();
    id_qm      = draw_manager.new_sprite_id();
    id_desc    = draw_manager.new_sprite_id();

    area_playerinfo = {
        galaxy_panel_area.x + PNL_BORDER,
        galaxy_panel_area.y + PNL_BORDER,
        194,
        galaxy_panel_area.h - PNL_BORDER * 2};
    area_starinfo = {
        area_playerinfo.x + area_playerinfo.w + PNL_BORDER,
        galaxy_panel_area.y + PNL_BORDER,
        RES_X - (area_playerinfo.x + area_playerinfo.w + PNL_BORDER) - PNL_BORDER,
        474 - PNL_TOP};
}

void PanelDrawer::draw_panel_bg(DrawTarget tgt) {
    draw_manager.fill(tgt, galaxy_panel_area, {0xA0, 0xA0, 0xA0});
    draw_manager.pattern_fill(tgt, area_playerinfo);
    draw_manager.pattern_fill(tgt, area_starinfo);

    const char* pnl_spr = type == PNL_Galaxy ? IMG_BR9_EXPORT : IMG_BR10_EXPORT;
    const int top = area_playerinfo.y + PNL_PLAYER_PAD;

    draw_manager.draw(
            tgt,
            id_panel,
            pnl_spr,
            {RES_X - PNL_BORDER, RES_Y - PNL_BORDER, 1, 1, 1, 1});

    draw_manager.draw_text(
            tgt,
            "Month: ",
            Justify::Left,
            area_playerinfo.x + 4,
            top + PNL_Y_SEP,
            COL_TEXT);
    draw_manager.draw_text(
            tgt,
            "MCredits: ",
            Justify::Left,
            area_playerinfo.x + 4,
            top + 2*PNL_Y_SEP,
            COL_TEXT);
    draw_manager.draw_text(
            tgt,
            "Planets: ",
            Justify::Left,
            area_playerinfo.x + 4,
            top + 3*PNL_Y_SEP,
            COL_TEXT);
}

void PanelDrawer::update_panel_info(DrawTarget tgt, PlayerInfo* player, FlyTarget* ft) {
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

    const int top = area_playerinfo.y + PNL_PLAYER_PAD;

    draw_manager.draw_text(
            tgt,
            id_name,
            player ? player->full_name : "",
            Justify::Left,
            area_playerinfo.x + 4,
            top,
            COL_TEXT);
    draw_manager.draw_text(
            tgt,
            id_month,
            month_string,
            Justify::Left,
            area_playerinfo.x + 72,
            top + PNL_Y_SEP,
            COL_TEXT2);
    draw_manager.draw_text(
            tgt,
            id_mc,
            mc_string,
            Justify::Left,
            area_playerinfo.x + 94,
            top + 2*PNL_Y_SEP,
            COL_TEXT2);
    draw_manager.draw_text(
            tgt,
            id_planets,
            planets_string,
            Justify::Left,
            area_playerinfo.x + 80,
            top + 3*PNL_Y_SEP,
            COL_TEXT2);

    /* FlyTarget info */
    char ft_desc[41];
    bool is_guild = ft == gal->get_guild();
    snprintf(ft_desc, 40, "This is the %s%s.", is_guild ? "" : "star ", ft->name);

    // Draw '?' or star details
    if (player->location.has_visited(exostate.tgt2loc(ft))) {
        draw_manager.draw(
            id_qm,
            nullptr);
    } else {
        draw_manager.draw(
            id_qm,
            IMG_TS1_QMARK,
            {RES_X - 12, area_starinfo.y + 6, 1, 0, 1, 1});
    }

    draw_manager.draw_text(
        id_desc,
        (const char*) ft_desc,
        Justify::Left,
        area_starinfo.x + 4,
        area_starinfo.y + 2,
        COL_TEXT);
}
