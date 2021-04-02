#ifndef GUARD_EXODUS_H
#define GUARD_EXODUS_H

#include <map>

#define MODE_STACK_SIZE 256

using std::map;

enum ExodusMode {
    MODE_None,
    MODE_Pop,
    MODE_Reload,

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
    MODE_PlanetStatus,
    MODE_PlanetColonise,
    MODE_PlanetTransfer,
    MODE_LunarBattlePrep,
    MODE_LunarBattle,
    MODE_Trade,
    MODE_FleetProduction,
    MODE_ShipEquip,
    MODE_Arrive,
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
        ExodusMode current_mode;
        map<ExodusMode, ModeBase*> mode_map;
        void set_mode(ExodusMode);
        unsigned int mode_stack_head;
        ExodusMode mode_stack[MODE_STACK_SIZE];
        void push_mode(ExodusMode);
        void pop_mode();
        void reset_mode_stack();
        bool mode_updated_since_enter;
};

#ifdef SDL
#include "exodus.SDL.h"
#endif

#endif
