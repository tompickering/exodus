#ifndef GUARD_EXODUS_H
#define GUARD_EXODUS_H

#include <map>

using std::map;

enum ExodusMode {
    MODE_None,
    MODE_Intro,
    MODE_Menu,
    MODE_GalaxyGen,
    MODE_GalaxyMap,
    MODE_GalaxyIntro,
    MODE_GuildExterior,
    MODE_GuildBar,
    MODE_GuildHQ,
    MODE_Fly,
    MODE_StarMap,
    MODE_PlanetMap,
};

class ModeBase;

class Exodus {
    public:
        Exodus();
        virtual ~Exodus();
        int run(int argc, char** argv);
        ExodusMode get_prev_mode();
    protected:
        virtual bool init() = 0;
        virtual void cleanup() = 0;
    private:
        ModeBase *mode;
        map<ExodusMode, ModeBase*> mode_map;
        void set_mode(ExodusMode);
        ExodusMode current_mode;
        ExodusMode prev_mode;
};

#ifdef SDL
#include "exodus.SDL.h"
#endif

#endif
