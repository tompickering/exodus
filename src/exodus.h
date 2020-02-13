#ifndef GUARD_EXODUS_H
#define GUARD_EXODUS_H

#include <map>

using std::map;

enum ExodusMode {
    ST_None,
    ST_Intro,
    ST_MainMenu,
};

class ModeBase;

class Exodus {
    public:
        Exodus();
        virtual ~Exodus();
        int run(int argc, char** argv);
    protected:
        virtual bool init() = 0;
        virtual void cleanup() = 0;
    private:
        ModeBase *mode;
        map<ExodusMode, ModeBase*> mode_map;
        void set_mode(ExodusMode);
};

#ifdef SDL
#include "exodus.SDL.h"
#endif

#endif
