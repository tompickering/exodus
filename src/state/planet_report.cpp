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

    return *this;
}

void PlanetReport::init(int star, int planet, int player) {
    reset();
    star_idx = star;
    planet_idx = planet;
    player_idx = player;
}

void PlanetReport::add_event(PlanetReportEvent event) {
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

bool PlanetReport::empty() const {
    return (items == 0) && (event_mask == 0);
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
}

void PlanetReport::load(cJSON* j) {
    LOAD_NUM(j, star_idx);
    LOAD_NUM(j, planet_idx);
    LOAD_NUM(j, player_idx);
    LOAD_NUM(j, items);
    LOAD_ARRAY_OF_STR(j, content);
    LOAD_NUM(j, event_mask);
}
