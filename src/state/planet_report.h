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
    PlanetReport();
    PlanetReport& operator=(const PlanetReport& other);

    void init(int star, int planet, int player);

    int star_idx;
    int planet_idx;
    int player_idx;

    int items;
    char content[REPORT_LINES][REPORT_LINE_MAX];

    uint32_t event_mask;

    void add_event(PlanetReportEvent event);
    bool has_event(PlanetReportEvent event) const;
    void add_line(const char* msg, ...);
    bool empty() const;
    void reset();

    Planet* get_planet() const;

    virtual void save(cJSON* j) const override;
    virtual void load(cJSON* j) override;
};

#endif
