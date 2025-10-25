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

enum PlanetReportEventLevel {
    PREL_None,
    PREL_Good,
    PREL_Warn,
    PREL_Crit,
};

struct PlanetReport : public Saveable {
    PlanetReport();
    PlanetReport& operator=(const PlanetReport& other);
    bool operator>(const PlanetReport& other) const;
    bool operator<(const PlanetReport& other) const;

    void init(int star, int planet, int player);

    int star_idx;
    int planet_idx;
    int player_idx;

    int items;
    char content[REPORT_LINES][REPORT_LINE_MAX];

    uint32_t event_mask;

    PlanetTrafficLight light_food;
    PlanetTrafficLight light_plu;
    PlanetTrafficLight light_unrest;
    PlanetTrafficLight light_army;

    int problems_critical;
    int problems_major;
    int problems_minor;
    int good_news;

    bool finalised;

    void add_event(PlanetReportEvent event);
    bool has_event(PlanetReportEvent event) const;
    void add_line(const char* msg, ...);
    void register_critical_problem();
    void register_major_problem();
    void register_minor_problem();
    void register_good_news();
    PlanetReportEventLevel get_level() const;
    void finalise();
    bool empty() const;
    void reset();

    Planet* get_planet() const;

    virtual void save(cJSON* j) const override;
    virtual void load(cJSON* j) override;
};

#endif
