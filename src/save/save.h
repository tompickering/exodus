#ifndef GUARD_SAVE_H
#define GUARD_SAVE_H

#include "exodus.h"

#include "saveable.h"

#include "state/exodus_state.h"
#include "player/player.h"

#define MAX_SLOTS 11
#define MAX_PATH 255

class SaveMeta : public Saveable {
    public:
        virtual void save(cJSON* j) const override {
            SAVE_BOOL(j, exists);
            SAVE_STR(j, name);
            SAVE_NUM(j, month);
            SAVE_NUM(j, planets);
        }

        virtual void load(cJSON* j) override {
            LOAD_BOOL(j, exists);
            LOAD_STR(j, name);
            LOAD_NUM(j, month);
            LOAD_NUM(j, planets);
        }

        bool exists;
        char name[MAX_PLAYER_NAME+1];
        int month;
        int planets;
};

class SaveManager {
    public:
        virtual const SaveMeta* get_all_meta(bool);
        virtual bool save(int);
        virtual bool load(int);
    protected:
        virtual const char* get_save_dir() = 0;
        virtual bool save_data(uint32_t, int, const char*, const char*);
        virtual bool load_data(uint32_t, int, char**, char**);
};

#ifdef LINUX
#include "save.linux.h"
#endif

#ifdef MAC
#include "save.linux.h"
#endif

#ifdef WINDOWS
#include "save.windows.h"
#endif

#endif
