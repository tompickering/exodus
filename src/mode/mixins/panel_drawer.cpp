#include "panel_drawer.h"

#include "draw/draw.h"

#include "galaxy/star.h"

#include "exodus_features.h"

#include "assetpaths.h"
#include "shared.h"

const int PNL_TOP        = 412;
const int PNL_BORDER     = 4;
const int PNL_Y_SEP      = TEXT_Y_SEP;
const int PNL_PLAYER_PAD = 5;

const DrawArea galaxy_panel_area = {0, PNL_TOP, RES_X, RES_Y - PNL_TOP};

DrawArea area_playerinfo;
DrawArea area_starinfo;

PanelDrawer::PanelDrawer(PanelType _type) : type(_type) {
    id_panel       = draw_manager.new_sprite_id();
    id_name        = draw_manager.new_sprite_id();
    id_month_txt   = draw_manager.new_sprite_id();
    id_month       = draw_manager.new_sprite_id();
    id_mc_txt      = draw_manager.new_sprite_id();
    id_mc          = draw_manager.new_sprite_id();
    id_planets_txt = draw_manager.new_sprite_id();
    id_planets     = draw_manager.new_sprite_id();
    id_qm          = draw_manager.new_sprite_id();
    id_desc        = draw_manager.new_sprite_id();
    id_desc1       = draw_manager.new_sprite_id();
    id_desc2       = draw_manager.new_sprite_id();

    for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
        id_planet_icons[i] = draw_manager.new_sprite_id();
        id_marker_icons[i] = draw_manager.new_sprite_id();
    }

    for (int i = 0; i < PNL_MAX_FLEETS ; ++i) {
        id_fleet_icons[i] = draw_manager.new_sprite_id();
    }

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
    draw_manager.fill(tgt, galaxy_panel_area, COL_BORDERS);
    draw_manager.fill_pattern(tgt, area_playerinfo);
    draw_manager.fill_pattern(tgt, area_starinfo);
    draw_manager.save_background();

    const char* pnl_spr = type == PNL_Galaxy ? IMG_BR9_EXPORT : IMG_BR10_EXPORT;
    const int top = area_playerinfo.y + PNL_PLAYER_PAD;

    draw_manager.draw(
            tgt,
            id_panel,
            pnl_spr,
            {RES_X - PNL_BORDER, RES_Y - PNL_BORDER, 1, 1, 1, 1});

    draw_manager.draw_text(
            tgt,
            id_month_txt,
            "Month: ",
            Justify::Left,
            area_playerinfo.x + 4,
            top + PNL_Y_SEP,
            COL_TEXT);
    draw_manager.draw_text(
            tgt,
            id_mc_txt,
            "MCredits: ",
            Justify::Left,
            area_playerinfo.x + 4,
            top + 2*PNL_Y_SEP,
            COL_TEXT);
    draw_manager.draw_text(
            tgt,
            id_planets_txt,
            "Planets: ",
            Justify::Left,
            area_playerinfo.x + 4,
            top + 3*PNL_Y_SEP,
            COL_TEXT);
}

void PanelDrawer::update_panel_info_player(DrawTarget tgt, Player* player) {
    char month_string[5];
    char mc_string[7];
    char planets_string[4];

    snprintf(month_string, 5, "%u", exostate().get_month());
    if (player) {
        snprintf(mc_string, 7, "%u", player->get_mc());
        snprintf(planets_string, 4, "%d", exostate().get_n_planets(player));
    } else {
        strcpy(mc_string, "");
        strcpy(planets_string, "");
    }

    const int top = area_playerinfo.y + PNL_PLAYER_PAD;

    draw_manager.draw_text(
            tgt,
            id_name,
            player ? player->get_full_name() : "",
            Justify::Left,
            area_playerinfo.x + 4,
            top,
            COL_TEXT);
    draw_manager.draw_text(
            tgt,
            id_month,
            month_string,
            Justify::Left,
            draw_manager.right(id_month_txt),
            top + PNL_Y_SEP,
            COL_TEXT2);
    draw_manager.draw_text(
            tgt,
            id_mc,
            mc_string,
            Justify::Left,
            draw_manager.right(id_mc_txt),
            top + 2*PNL_Y_SEP,
            COL_TEXT2);
    draw_manager.draw_text(
            tgt,
            id_planets,
            planets_string,
            Justify::Left,
            draw_manager.right(id_planets_txt),
            top + 3*PNL_Y_SEP,
            COL_TEXT2);
}

void PanelDrawer::update_panel_info_ft(DrawTarget tgt, Player* player, FlyTarget* ft) {
    Galaxy *gal = exostate().get_galaxy();

    char ft_desc[42];
    ft_desc[0] = '\0';

    if (ft) {
        bool is_guild = ft == gal->get_guild();
        snprintf(ft_desc, sizeof(ft_desc), "This is the %s%s.", is_guild ? "" : "star ", ft->name);
    }

    for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
        draw_manager.draw(id_planet_icons[i], nullptr);
        draw_manager.draw(id_marker_icons[i], nullptr);
    }

    for (int i = 0; i < PNL_MAX_FLEETS; ++i) {
        draw_manager.draw(id_fleet_icons[i], nullptr);
    }

    if (!(player && ft)) {
        draw_manager.draw(
            id_qm,
            nullptr);
        for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
            draw_manager.draw(id_marker_icons[i], nullptr);
        }
        return;
    }

    // Draw '?' or star details
    if (player->get_location().has_visited(exostate().tgt2loc(ft))) {
        draw_manager.draw(
            id_qm,
            nullptr);

        if (ft->is_star) {
            Star *star = (Star*)ft;
            int planets_drawn = 0;
            bool drawing = false;
            for (int i = STAR_MAX_PLANETS - 1; i >= 0; --i) {
                Planet *p = star->get_planet(i);
                if (!drawing) {
                    drawing = p && p->exists();
                }

                if (drawing && !(p && p->exists())) {
                    drawing = false;
                }

                if (drawing) {
                    float planet_scale = 1.0;

                    if (FEATURE(EF_MULTISIZE_PLANETS_PANEL)) {
                        if (p->get_size() == PLANET_Small) planet_scale = 0.6;
                        if (p->get_size() == PLANET_Medium) planet_scale = 0.8;
                    }

                    draw_manager.draw(
                        id_planet_icons[STAR_MAX_PLANETS - 1 - planets_drawn],
                        p->sprites()->panel_icon,
                        {RES_X - 32 - 22*planets_drawn,
                         area_starinfo.y + 18,
                         0.5, 0.5,
                         planet_scale, planet_scale});

                    if (p->is_owned()) {
                        Player *owner = exostate().get_player(p->get_owner());
                        bool enemy = owner != player;
                        draw_manager.draw(
                            id_marker_icons[STAR_MAX_PLANETS - 1 - planets_drawn],
                            enemy ? IMG_TS1_ID2 : IMG_TS1_ID1,
                            {RES_X - 32 - 22*planets_drawn + 1,
                             area_starinfo.y + 36,
                             0.5, 0.5, 1, 1});
                    }

                    ++planets_drawn;
                }
            }

            int ftloc = exostate().tgt2loc(ft);
            PlayerLocation &loc = player->get_location();
            if (!loc.in_flight() && loc.get_target() == ftloc)
            {
                int fleets_drawn = 0;
                for (int i = 0; i < N_PLAYERS && fleets_drawn < PNL_MAX_FLEETS; ++i) {
                    Player *pl = exostate().get_player(i);

                    const char* icon = IMG_TS1_SHICON;

                    if (pl == player) {
                        icon = IMG_SHICON_H;
                    }

                    if (pl->get_location().get_target() == ftloc) {
                        draw_manager.draw(
                            id_fleet_icons[PNL_MAX_FLEETS - 1 - fleets_drawn],
                            icon,
                            {area_starinfo.x + 4 + 44*fleets_drawn,
                             area_starinfo.y + 24,
                             0, 0, 1, 1});

                        fleets_drawn++;
                    }
                }
            }
        }
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

void PanelDrawer::update_panel_info_planet(DrawTarget tgt, Player *player, Planet* planet) {
    if (!planet) {
        // Clear the info
        draw_manager.draw(id_desc,  nullptr);
        draw_manager.draw(id_desc1, nullptr);
        draw_manager.draw(id_desc2, nullptr);
        return;
    }

    char planet_desc[41];
    char planet_own[20 + MAX_PLAYER_FULLNAME];
    if (planet->is_owned()) {
        Player *owner = exostate().get_player(planet->get_owner());
        snprintf(planet_desc, 40, "This is the planet %s.", planet->get_name());
        snprintf(planet_own, 20 + MAX_PLAYER_FULLNAME, "It belongs to %s.", owner->get_full_name());
    } else {
        snprintf(planet_desc, 40, "This is an unexplored planet.");
        snprintf(planet_own, 20 + MAX_PLAYER_FULLNAME, "It belongs to no confederation.");
    }

    char planet_class[20];
    snprintf(planet_class, 20, "Class: %s", planet->get_class_str());

    draw_manager.draw_text(
        id_desc,
        Font::Small,
        (const char*) planet_desc,
        Justify::Left,
        area_starinfo.x + 4,
        area_starinfo.y + 2,
        COL_TEXT);

    draw_manager.draw_text(
        id_desc1,
        Font::Small,
        (const char*) planet_own,
        Justify::Left,
        area_starinfo.x + 4,
        area_starinfo.y + 2 + PNL_Y_SEP,
        COL_TEXT);

    draw_manager.draw_text(
        id_desc2,
        Font::Small,
        (const char*) planet_class,
        Justify::Left,
        area_starinfo.x + 4,
        area_starinfo.y + 2 + 2*PNL_Y_SEP,
        COL_TEXT);
}

void PanelDrawer::panel_clear_starinfo() {
    draw_manager.draw(id_qm, nullptr);
    draw_manager.draw(id_desc, nullptr);
    draw_manager.draw(id_desc1, nullptr);
    draw_manager.draw(id_desc2, nullptr);

    for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
        draw_manager.draw(id_planet_icons[i], nullptr);
        draw_manager.draw(id_marker_icons[i], nullptr);
    }

    for (int i = 0; i < PNL_MAX_FLEETS; ++i) {
        draw_manager.draw(id_fleet_icons[i], nullptr);
    }
}

void PanelDrawer::panel_set_text(const char* text) {
    panel_clear_starinfo();
    draw_manager.draw_text(
        id_desc,
        text,
        Justify::Left,
        area_starinfo.x + 4,
        area_starinfo.y + 2,
        COL_TEXT);
}
