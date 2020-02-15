#ifndef GUARD_EXODUS_H
#define GUARD_EXODUS_H

enum ExodusState {
    ST_None,
    ST_Intro,
    ST_MainMenu,
};

class Exodus {
    public:
        Exodus();
        virtual ~Exodus();
        int run(int argc, char** argv);
    protected:
        virtual bool init() = 0;
        virtual void cleanup() = 0;
};

#ifdef SDL
#include "exodus.SDL.h"
#endif

#endif
