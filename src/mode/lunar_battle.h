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
    DIR_None,
    DIR_Up,
    DIR_Down,
    DIR_Left,
    DIR_Right,
};

enum BattleUnitType {
    UNIT_Inf,
    UNIT_Gli,
    UNIT_Art,
    UNIT_LBGun,
    UNIT_LBCtl,
    UNIT_Rebel,
    UNIT_AInf,
    UNIT_AArt,
};

class BattleUnit {
    public:
        BattleUnit() {};
        BattleUnit(BattleUnitType);
        BattleUnit& init(int, int);
        BattleUnit& init(int, int, int);
        BattleUnit& init(int, int, int, bool);
        BattleUnitType type;
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
        int moves_remaining;
        bool turn_taken;
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
            LB_Auto,
            LB_SelectUnit,
            LB_Move,
            LB_Fire,
            LB_CheckWon,
            LB_Won,
        };

        Stage stage;

        int n_cover;
        Cover cover[COVER_MAX];

        int n_units;
        BattleUnit units[BATTLE_UNITS_MAX];
        SprID unit_ids[BATTLE_UNITS_MAX];

        bool select_unit();
        void reset_round();
        BattleUnit* active_unit;

        BattleUnit* unit_at(int, int);

        int cursor_x;
        int cursor_y;

        bool use_alt_aliens;

        void place_cover();
        void place_units();
        void place_side_units(bool);
        void place_unit(BattleUnit);

        void draw_units();
        void update_cursor();
        void update_arrows();

        bool defender_turn;
        bool human_turn;

        bool unit_moving;
        float move_interp;
};

#endif
