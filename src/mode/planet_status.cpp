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
    PlayerInfo *player = exostate.get_active_player();

    if (!p) {
        L.fatal("Entered PlanetStatus mode with no active planet!");
    }

    // Orig: PROCstatus
    draw_manager.draw(p->sprites()->landscape);
    draw_manager.fill({0, 0, RES_X, 70}, COL_BORDERS);
    draw_manager.pattern_fill({4, 4, RES_X - 8, 62});

    char heading[12 + PLANET_MAX_NAME];

    int text_x = 30;
    PlayerInfo *owner = p->is_owned() ? exostate.get_player(p->get_owner()) : nullptr;

    char ownership[PLANET_MAX_NAME + MAX_PLAYER_FULLNAME + 31];

    if (owner) {
        snprintf(heading, 12 + PLANET_MAX_NAME, "The planet %s", p->get_name());
        text_x += 100;

        draw_manager.draw(
            id(ID::FLAG),
            flags[owner->flag_idx],
            {16, 100, 0, 0, 1, 1});

        if (owner == player) {
            snprintf(ownership, PLANET_MAX_NAME + MAX_PLAYER_FULLNAME + 30,
                     "%s belongs to you, %s.", p->get_name(), owner->ref);
        } else {
            snprintf(ownership, PLANET_MAX_NAME + MAX_PLAYER_FULLNAME + 30,
                     "%s belongs to %s.", p->get_name(), owner->full_name);
        }
    } else {
        strncpy(heading, "Unexplored planet", 12 + PLANET_MAX_NAME);
        strncpy(ownership, "The planet belongs to no confederation.",
                PLANET_MAX_NAME + MAX_PLAYER_FULLNAME + 30);
    }

    char summary[60];
    const char* size_str = "Small";
    if(p->get_size() == PLANET_Medium) size_str = "Medium";
    if(p->get_size() == PLANET_Large) size_str = "Large";
    char cost[16]; snprintf(cost, 16, ", cost %d MC", p->get_settlement_cost());

    snprintf(summary, 60,
        "O %d km (%s %s world%s))",
        p->get_diameter(),
        size_str,
        p->get_class_str_lower(),
        owner ? "" : cost);

    char pop[31];
    snprintf(pop, 30, "Population: %d million.", p->get_population());

    char day[51];
    snprintf(day, 50, "One day is %d standard hours long.", p->get_day_hours());

    const char *cred0 = "Monthly MCredits: ";
    char cred1[11];
    if (owner && owner == player) {
        snprintf(cred1, 10, "%d/%d", p->get_net_income(), p->get_income());
    } else {
        snprintf(cred1, 10, "?");
    }

    draw_manager.draw_text(
        Font::Large,
        heading,
        Justify::Centre,
        RES_X/2, 16,
        COL_TEXT);

    int text_y = 90;

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

    draw_manager.save_background();
    draw_manager.show_cursor(true);
}

ExodusMode PlanetStatus::update(float delta) {
    if (draw_manager.clicked()) {
        return exodus.get_prev_mode();
    }

    return ExodusMode::MODE_None;
}
