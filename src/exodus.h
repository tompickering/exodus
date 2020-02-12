#ifndef GUARD_EXODUS_H
#define GUARD_EXODUS_H

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
