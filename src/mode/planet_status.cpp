#include "planet_status.h"

#include "galaxy/planet.h"
#include "state/exodus_state.h"

#include "assetpaths.h"

enum ID {
    FLAG,
    CREDITS_TXT,
    END,
};

PlanetStatus::PlanetStatus() : ModeBase("PlanetStatus") {
}

void PlanetStatus::enter() {
    ModeBase::enter(ID::END);
    Planet *p = exostate.get_active_planet();
    Player *player = exostate.get_active_player();

    if (!p) {
        L.fatal("Entered PlanetStatus mode with no active planet!");
    }

    // Orig: PROCstatus
    draw_manager.draw(p->sprites()->landscape);
    draw_manager.fill({0, 0, RES_X, 70}, COL_BORDERS);
    draw_manager.pattern_fill({4, 4, RES_X - 8, 62});

    char heading[12 + PLANET_MAX_NAME];

    int text_x = 30;
    Player *owner = p->is_owned() ? exostate.get_player(p->get_owner()) : nullptr;

    char ownership[PLANET_MAX_NAME + MAX_PLAYER_FULLNAME + 31];

    if (strnlen(p->get_name(), 1) > 0) {
        snprintf(heading, 12 + PLANET_MAX_NAME, "The planet %s", p->get_name());
    } else {
        strncpy(heading, "Unexplored planet", 12 + PLANET_MAX_NAME);
    }

    if (owner) {
        text_x += 100;

        draw_manager.draw(
            id(ID::FLAG),
            flags[owner->get_flag_idx()],
            {16, 100, 0, 0, 1, 1});

        if (owner == player) {
            snprintf(ownership, PLANET_MAX_NAME + MAX_PLAYER_FULLNAME + 30,
                     "%s belongs to you, %s.", p->get_name(), owner->get_ref());
        } else {
            snprintf(ownership, PLANET_MAX_NAME + MAX_PLAYER_FULLNAME + 30,
                     "%s belongs to %s.", p->get_name(), owner->get_full_name());
        }
    } else {
        strncpy(ownership, "The planet belongs to no confederation.",
                PLANET_MAX_NAME + MAX_PLAYER_FULLNAME + 30);
    }

    char summary[60];
    const char* size_str = "Small";
    if(p->get_size() == PLANET_Medium) size_str = "Medium";
    if(p->get_size() == PLANET_Large) size_str = "Large";
    char cost[16]; snprintf(cost, 16, ", cost %d MC", p->get_settlement_cost());

    snprintf(summary, 60,
        "O %d km (%s %s world%s)",
        p->get_diameter(),
        size_str,
        p->get_class_str_lower(),
        owner ? "" : cost);

    char pop[31];
    snprintf(pop, 30, "Population: %d million.", p->get_population());

    char day[51];
    snprintf(day, 50, "One day is %d standard hours long.", p->get_day_hours());

    char army_heading[46];
    army_heading[45] = '\0';
    snprintf(army_heading, 45, "Other defence systems: (Max each: %d)", p->get_army_cap());

    int inf, gli, art;
    p->get_army(inf, gli, art);
    char inf_str[8]; inf_str[7] = '\0';
    char gli_str[8]; gli_str[7] = '\0';
    char art_str[8]; art_str[7] = '\0';
    snprintf(inf_str, 8, "%d", inf);
    snprintf(gli_str, 8, "%d", gli);
    snprintf(art_str, 8, "%d", art);

    const char *cred0 = "Monthly MCredits: ";
    char cred1[11];
    if (owner && owner == player) {
        snprintf(cred1, 10, "%d/%d", p->get_net_income(), p->get_income());
    } else {
        snprintf(cred1, 10, "?");
    }

    char cities[5];
    int n_cities = p->get_n_cities();
    if (n_cities == 0) {
        strncpy(cities, "None", 5);
    } else {
        snprintf(cities, 5, "%d", n_cities);
    }

    char agri[20];
    int n_agri = p->get_n_agri();
    if (n_agri == 0) {
        strncpy(agri, "None", 20);
    } else {
        if (p->agri_sufficient()) {
            snprintf(agri, 20, "%s", "Well");
        } else {
            snprintf(agri, 20, "%s", "Not sufficient");
        }
    }

    draw_manager.draw_text(
        Font::Large,
        heading,
        Justify::Centre,
        RES_X/2, 16,
        COL_TEXT);

    int text_y = 90;

    char mins[10];
    bool see_minerals = false;

    if (p->is_owned() && player == exostate.get_player(p->get_owner())) {
        see_minerals = true;
    }

    if (exostate.active_player_local() && player->get_fleet().scouts >= 20) {
        see_minerals = true;
    }

    if (see_minerals) {
        int m = p->get_minerals();
        if (m > 0) {
            snprintf(mins, 10, "%d", p->get_minerals());
        } else {
            snprintf(mins, 10, "None");
        }
    } else {
        snprintf(mins, 10, "?");
    }

    draw_manager.draw_text(
        ownership,
        Justify::Left,
        text_x, text_y,
        COL_TEXT2);

    text_y += TEXT_Y_SEP;

    draw_manager.draw_text(
        summary,
        Justify::Left,
        text_x, text_y,
        COL_TEXT2);

    text_y += TEXT_Y_SEP;
    text_y += TEXT_Y_SEP;

    draw_manager.draw_text(
        pop,
        Justify::Left,
        text_x, text_y,
        COL_TEXT);

    text_y += TEXT_Y_SEP;

    draw_manager.draw_text(
        day,
        Justify::Left,
        text_x, text_y,
        COL_TEXT);

    text_y += TEXT_Y_SEP;

    draw_manager.draw_text(
        id(ID::CREDITS_TXT),
        cred0,
        Justify::Left,
        text_x, text_y,
        COL_TEXT);
    draw_manager.draw_text(
        cred1,
        Justify::Left,
        draw_manager.right(id(ID::CREDITS_TXT)), text_y,
        COL_TEXT2);

    text_y += TEXT_Y_SEP;
    text_y += TEXT_Y_SEP;

    draw_manager.draw_text(
        "Cities:",
        Justify::Left,
        text_x, text_y,
        COL_TEXT);
    draw_manager.draw_text(
        cities,
        Justify::Left,
        text_x + 140, text_y,
        COL_TEXT2);

    text_y += TEXT_Y_SEP;

    draw_manager.draw_text(
        "Agriculture:",
        Justify::Left,
        text_x, text_y,
        COL_TEXT);
    draw_manager.draw_text(
        agri,
        Justify::Left,
        text_x + 140, text_y,
        COL_TEXT2);

    text_y += TEXT_Y_SEP;

    draw_manager.draw_text(
        "Minerals:",
        Justify::Left,
        text_x, text_y,
        COL_TEXT);
    draw_manager.draw_text(
        mins,
        Justify::Left,
        text_x + 140, text_y,
        COL_TEXT2);

    text_y += TEXT_Y_SEP;

    draw_manager.draw_text(
        "Lunar base:",
        Justify::Left,
        text_x, text_y,
        COL_TEXT);
    draw_manager.draw_text(
        p->has_lunar_base() ? "Yes" : "No",
        Justify::Left,
        text_x + 140, text_y,
        COL_TEXT2);

    text_y += TEXT_Y_SEP;

    draw_manager.draw_text(
        "AirDef guns:",
        Justify::Left,
        text_x, text_y,
        COL_TEXT);
    draw_manager.draw_text(
        "<TODO>",
        Justify::Left,
        text_x + 140, text_y,
        COL_TEXT2);

    text_y += TEXT_Y_SEP;

    draw_manager.draw_text(
        "Robots:",
        Justify::Left,
        text_x, text_y,
        COL_TEXT);
    draw_manager.draw_text(
        "<TODO>",
        Justify::Left,
        text_x + 140, text_y,
        COL_TEXT2);

    text_y += TEXT_Y_SEP;
    text_y += TEXT_Y_SEP;

    draw_manager.draw_text(
        army_heading,
        Justify::Left,
        text_x, text_y,
        COL_TEXT);

    text_y += TEXT_Y_SEP;
    text_y += TEXT_Y_SEP;

    draw_manager.draw_text(
        "Infantry:",
        Justify::Left,
        text_x, text_y,
        COL_TEXT);
    draw_manager.draw_text(
        inf_str,
        Justify::Left,
        text_x + 140, text_y,
        COL_TEXT2);

    text_y += TEXT_Y_SEP;

    draw_manager.draw_text(
        "Gliders:",
        Justify::Left,
        text_x, text_y,
        COL_TEXT);
    draw_manager.draw_text(
        gli_str,
        Justify::Left,
        text_x + 140, text_y,
        COL_TEXT2);

    text_y += TEXT_Y_SEP;

    draw_manager.draw_text(
        "Artillery:",
        Justify::Left,
        text_x, text_y,
        COL_TEXT);
    draw_manager.draw_text(
        art_str,
        Justify::Left,
        text_x + 140, text_y,
        COL_TEXT2);

    // TODO: Min / Food / Plu gauges

    draw_manager.save_background();
    draw_manager.show_cursor(true);
}

ExodusMode PlanetStatus::update(float delta) {
    if (draw_manager.clicked()) {
        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}
