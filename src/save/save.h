#ifndef GUARD_SAVE_H
#define GUARD_SAVE_H

#include "state/exodus_state.h"
#include "player/player.h"

#define MAX_SLOTS 11
#define SAVE_SIZE (sizeof(SaveMeta) + sizeof(ExodusState))

typedef struct {
    bool exists;
    char name[MAX_PLAYER_NAME+1];
    int month;
    int planets;
} SaveMeta;

class SaveManager {
    public:
        virtual const SaveMeta* get_all_meta(bool);
        virtual bool save(int);
        virtual bool load(int);
    protected:
        virtual bool save_data(int, const char*) = 0;
        virtual bool load_data(int, char*) = 0;
        virtual const char* get_save_dir() = 0;
};

#ifdef LINUX
#include "save.linux.h"
#endif

#ifdef WINDOWS
#include "save.windows.h"
#endif

#endif
