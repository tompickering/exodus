#ifndef GUARD_PLANET_REPORT_H
#define GUARD_PLANET_REPORT_H

#include <cstdio>
#include <cstdarg>

#include "galaxy/galaxy.h"

#include "save/saveable.h"

#define REPORT_LINES 20
#define REPORT_LINE_MAX 256

#define MAX_REPORTS (GALAXY_MAX_PLANETS)

enum PlanetReportEvent {
    PRE_Meteor,
    PRE_Defects,
    PRE_Climate,
    PRE_Plague,
    PRE_Aliens,
    PRE_NewSpecies,
    PRE_Epidemic,
    PRE_Rebels,
    PRE_Meltdown,
    PRE_SurfChangeCultivation,
    PRE_SurfChangeClearing,
    PRE_HQDestroyed,
    PRE_ArmyProductionSaved,
    PRE_ArmyProductionStopped,
    PRE_MAX,
};

struct PlanetReport : public Saveable {
    PlanetReport() {
        reset();
    }

    void init(int star, int planet, int player) {
        reset();
        star_idx = star;
        planet_idx = planet;
        player_idx = player;
    }

    int star_idx;
    int planet_idx;
    int player_idx;

    int items;
    char content[REPORT_LINES][REPORT_LINE_MAX];

    uint32_t event_mask;

    void add_event(PlanetReportEvent event) {
        event_mask = (event_mask | (1 << (int)event));
    }

    bool has_event(PlanetReportEvent event) const {
        return (event_mask & (1 << (int)event));
    }

    void add_line(const char* msg, ...) {
        if (items >= REPORT_LINES)
            return;

        content[items][REPORT_LINE_MAX - 1] = '\0';
        va_list args;
        va_start(args, msg);
        vsnprintf(content[items], REPORT_LINE_MAX - 2, msg, args);
        va_end(args);
        items++;
    }

    bool empty() const {
        return (items == 0) && (event_mask == 0);
    }

    void reset() {
        star_idx = 0;
        planet_idx = 0;
        player_idx = 0;

        for (int i = 0; i < REPORT_LINES; ++i) {
            content[i][0] = '\0';
        }

        items = 0;

        event_mask = 0;
    }

    PlanetReport& operator=(const PlanetReport& other) {
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

    virtual void save(cJSON* j) const override {
        SAVE_NUM(j, star_idx);
        SAVE_NUM(j, planet_idx);
        SAVE_NUM(j, player_idx);
        SAVE_NUM(j, items);
        SAVE_ARRAY_OF_STR(j, content);
        SAVE_NUM(j, event_mask);
    }

    virtual void load(cJSON* j) override {
        LOAD_NUM(j, star_idx);
        LOAD_NUM(j, planet_idx);
        LOAD_NUM(j, player_idx);
        LOAD_NUM(j, items);
        LOAD_ARRAY_OF_STR(j, content);
        LOAD_NUM(j, event_mask);
    }
};

#endif
