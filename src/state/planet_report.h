#ifndef GUARD_PLANET_REPORT_H
#define GUARD_PLANET_REPORT_H

#include <cstdio>
#include <cstdarg>

#include "galaxy/galaxy.h"

#include "save/saveable.h"

#define REPORT_LINES 20
#define REPORT_LINE_MAX 256

#define MAX_REPORTS (GALAXY_MAX_PLANETS)

struct PlanetReport : public Saveable {
    PlanetReport() : items(0) {
        reset();
    }

    int items;
    char content[REPORT_LINES][REPORT_LINE_MAX];
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

    void reset() {
        for (int i = 0; i < REPORT_LINES; ++i) {
            content[i][0] = '\0';
        }

        items = 0;
    }

    PlanetReport& operator=(const PlanetReport& other) {
        reset();

        items = other.items;
        for (int i = 0; i < other.items; ++i) {
            snprintf(content[i], REPORT_LINE_MAX, other.content[i]);
        }

        return *this;
    }

    virtual void save(cJSON* j) const override {
        SAVE_NUM(j, items);
        SAVE_ARRAY_OF_STR(j, content);
    }

    virtual void load(cJSON* j) override {
        LOAD_NUM(j, items);
        LOAD_ARRAY_OF_STR(j, content);
    }
};

#endif
