#ifndef GUARD_EXODUS_H
#define GUARD_EXODUS_H

#include <map>

using std::map;

enum ExodusMode {
    MODE_None,
    MODE_Intro,
    MODE_Menu,
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
