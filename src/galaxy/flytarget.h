#ifndef GUARD_FLYTARGET_H
#define GUARD_FLYTARGET_H

#define FT_MAX_NAME 20

#include "save/saveable.h"

class FlyTarget : public Saveable {
    public:
        virtual void save(cJSON* j) const override {
            SAVE_NUM(j, x);
            SAVE_NUM(j, y);
            SAVE_STR(j, name);
            SAVE_BOOL(j, is_star);
            SAVE_NUM(j, pirates);
        }

        virtual void load(cJSON* j) override {
            LOAD_NUM(j, x);
            LOAD_NUM(j, y);
            LOAD_STR(j, name);
            LOAD_BOOL(j, is_star);
            LOAD_NUM(j, pirates);
        }

        FlyTarget() : FlyTarget(-1, -1, false, (char*)"NONAME") {};
        FlyTarget(int, int, bool, const char*);
        int x;
        int y;
        char name[FT_MAX_NAME + 1];
        bool is_star;
        int pirates; // Orig: SunP
};

#endif
