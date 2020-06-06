#ifndef GUARD_LUNAR_BATTLE_H
#define GUARD_LUNAR_BATTLE_H

#include "mode_base.h"
#include "mixins/comm_panel_drawer.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

#define BATTLE_UNITS_MAX 176    // Battlefield is 16x11

extern ExodusState exostate;

class BattleUnit {
    public:
        BattleUnit() {};
        BattleUnit(int, int, int, bool);
        int x;
        int y;
        int hp;
        int move;
        int fire_range;
        bool defending;
        bool can_shoot_behind;
        bool can_act;
        const char* idle;
        const char* walk;
        const char* fire;
        const char* dead;
        const char* move_sfx;
        const char* shoot_sfx;
};

class LunarBattle : ModeBase, CommPanelDrawer {
    public:
        LunarBattle();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            LB_Idle,
        };

        Stage stage;

        int n_units;
        BattleUnit units[BATTLE_UNITS_MAX];

        bool use_alt_aliens;
};

class Inf : BattleUnit {
    public:
        Inf(int, int, int, bool);
};

class Gli : BattleUnit {
    public:
        Gli(int, int, int, bool);
};

class Art : BattleUnit {
    public:
        Art(int, int, int, bool);
};

class LBGun : BattleUnit {
    public:
        LBGun(int, int);
};

class LBCtl : BattleUnit {
    public:
        LBCtl(int, int);
};

class Rebel : BattleUnit {
    public:
        Rebel(int, int, int);
};

class AInf : BattleUnit {
    public:
        AInf(int, int, int, bool);
};

class AArt : BattleUnit {
    public:
        AArt(int, int, int, bool);
};

#endif
