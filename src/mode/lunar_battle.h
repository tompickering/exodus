#ifndef GUARD_LUNAR_BATTLE_H
#define GUARD_LUNAR_BATTLE_H

#include "mode_base.h"
#include "mixins/comm_panel_drawer.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

#define COVER_MAX 10
#define BATTLE_UNITS_MAX 176    // Battlefield is 16x11

extern ExodusState exostate;

enum Direction {
    DIR_Up,
    DIR_Down,
    DIR_Left,
    DIR_Right,
};

class BattleUnit {
    public:
        BattleUnit() {};
        BattleUnit(int, int, int, bool);
        void do_move(Direction);
        int x;
        int y;
        int tgt_x;
        int tgt_y;
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

class Cover {
    public:
        Cover() {};
        Cover(int _x, int _y, bool _alt) : x(_x), y(_y), alt(_alt) {};
        int x;
        int y;
        bool alt;
};

class LunarBattle : ModeBase, CommPanelDrawer {
    public:
        LunarBattle();
        virtual void enter() override;
        virtual void exit() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            LB_Idle,
        };

        Stage stage;

        int n_cover;
        Cover cover[COVER_MAX];

        int n_units;
        BattleUnit units[BATTLE_UNITS_MAX];
        SprID unit_ids[BATTLE_UNITS_MAX];

        bool use_alt_aliens;

        void place_cover();
        void place_units();
        void place_unit(BattleUnit);

        void draw_units();
        bool unit_moving;
        float move_interp;
};

class Inf : public BattleUnit {
    public:
        Inf(int, int, int, bool);
};

class Gli : public BattleUnit {
    public:
        Gli(int, int, int, bool);
};

class Art : public BattleUnit {
    public:
        Art(int, int, int, bool);
};

class LBGun : public BattleUnit {
    public:
        LBGun(int, int);
};

class LBCtl : public BattleUnit {
    public:
        LBCtl(int, int);
};

class Rebel : public BattleUnit {
    public:
        Rebel(int, int, int);
};

class AInf : public BattleUnit {
    public:
        AInf(int, int, int, bool);
};

class AArt : public BattleUnit {
    public:
        AArt(int, int, int, bool);
};

#endif
