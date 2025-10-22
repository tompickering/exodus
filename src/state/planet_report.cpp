#include "planet_report.h"

#include "exodus_state.h"

PlanetReport::PlanetReport() {
    reset();
}

PlanetReport& PlanetReport::operator=(const PlanetReport& other) {
    reset();

    star_idx = other.star_idx;
    planet_idx = other.planet_idx;
    player_idx = other.player_idx;

    items = other.items;
    for (int i = 0; i < other.items; ++i) {
        snprintf(content[i], REPORT_LINE_MAX, other.content[i]);
    }

    event_mask = other.event_mask;
    problems_critical = other.problems_critical;
    problems_major = other.problems_major;
    problems_minor = other.problems_minor;
    good_news = other.good_news;
    finalised = other.finalised;

    light_food = other.light_food;
    light_plu = other.light_plu;
    light_unrest = other.light_unrest;

    return *this;
}

void PlanetReport::init(int star, int planet, int player) {
    reset();
    star_idx = star;
    planet_idx = planet;
    player_idx = player;
}

void PlanetReport::add_event(PlanetReportEvent event) {
    switch (event) {
        case PRE_HQDestroyed:
            register_critical_problem();
            break;
        case PRE_Plague:
        case PRE_Aliens:
        case PRE_Epidemic:
        case PRE_Rebels:
        case PRE_Meltdown:
            register_major_problem();
            break;
        case PRE_Meteor:
        case PRE_Defects:
        case PRE_SurfChangeClearing:
        case PRE_ArmyProductionStopped:
            register_minor_problem();
            break;
        default:
            break;
    }

    event_mask = (event_mask | (1 << (int)event));
}

bool PlanetReport::has_event(PlanetReportEvent event) const {
    return (event_mask & (1 << (int)event));
}

void PlanetReport::add_line(const char* msg, ...) {
    if (items >= REPORT_LINES)
        return;

    content[items][REPORT_LINE_MAX - 1] = '\0';
    va_list args;
    va_start(args, msg);
    vsnprintf(content[items], REPORT_LINE_MAX - 2, msg, args);
    va_end(args);
    items++;
}

void PlanetReport::register_critical_problem() {
    ++problems_critical;
}

void PlanetReport::register_major_problem() {
    ++problems_major;
}

void PlanetReport::register_minor_problem() {
    ++problems_minor;
}

void PlanetReport::register_good_news() {
    ++good_news;
}

void PlanetReport::finalise() {
    if (finalised) {
        return;
    }

    Planet *p = get_planet();
    light_food = p->get_traffic_light(PTLP_Food);
    light_plu = p->get_traffic_light(PTLP_Plu);
    light_unrest = p->get_traffic_light(PTLP_Unrest);

    if (light_food == PTL_Amber) register_minor_problem();
    if (light_plu == PTL_Amber) register_minor_problem();
    if (light_unrest == PTL_Amber) register_minor_problem();
    if (light_food == PTL_Red) register_major_problem();
    if (light_plu == PTL_Red) register_major_problem();
    if (light_unrest == PTL_Red) register_major_problem();

    finalised = true;
}

bool PlanetReport::empty() const {
    return (items == 0)
        && (event_mask == 0)
        && (light_food == PTL_Green)
        && (light_plu == PTL_Green)
        && (light_unrest == PTL_Green);
}

void PlanetReport::reset() {
    star_idx = 0;
    planet_idx = 0;
    player_idx = 0;

    for (int i = 0; i < REPORT_LINES; ++i) {
        content[i][0] = '\0';
    }

    items = 0;
    event_mask = 0;

    light_food = PTL_Green;
    light_plu = PTL_Green;
    light_unrest = PTL_Green;

    problems_critical = 0;
    problems_major = 0;
    problems_minor = 0;
    good_news = 0;

    finalised = false;
}

Planet* PlanetReport::get_planet() const {
    return exostate().get_planet(star_idx, planet_idx);
}

void PlanetReport::save(cJSON* j) const {
    SAVE_NUM(j, star_idx);
    SAVE_NUM(j, planet_idx);
    SAVE_NUM(j, player_idx);
    SAVE_NUM(j, items);
    SAVE_ARRAY_OF_STR(j, content);
    SAVE_NUM(j, event_mask);
    SAVE_ENUM(j, light_food);
    SAVE_ENUM(j, light_plu);
    SAVE_ENUM(j, light_unrest);
    SAVE_NUM(j, problems_critical);
    SAVE_NUM(j, problems_major);
    SAVE_NUM(j, problems_minor);
    SAVE_NUM(j, good_news);
    SAVE_BOOL(j, finalised);
}

void PlanetReport::load(cJSON* j) {
    LOAD_NUM(j, star_idx);
    LOAD_NUM(j, planet_idx);
    LOAD_NUM(j, player_idx);
    LOAD_NUM(j, items);
    LOAD_ARRAY_OF_STR(j, content);
    LOAD_NUM(j, event_mask);
    LOAD_ENUM(j, light_food);
    LOAD_ENUM(j, light_plu);
    LOAD_ENUM(j, light_unrest);
    LOAD_NUM(j, problems_critical);
    LOAD_NUM(j, problems_major);
    LOAD_NUM(j, problems_minor);
    LOAD_NUM(j, good_news);
    LOAD_BOOL(j, finalised);
}
