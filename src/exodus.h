#ifndef GUARD_EXODUS_H
#define GUARD_EXODUS_H

#include <map>

using std::map;

enum ExodusState {
    ST_None,
    ST_Intro,
    ST_MainMenu,
};

class StateBase;

class Exodus {
    public:
        Exodus();
        virtual ~Exodus();
        int run(int argc, char** argv);
    protected:
        virtual bool init() = 0;
        virtual void cleanup() = 0;
    private:
        StateBase *state;
        map<ExodusState, StateBase*> state_map;
        void set_state(ExodusState);
};

#ifdef SDL
#include "exodus.SDL.h"
#endif

#endif
