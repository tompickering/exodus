#ifndef GUARD_LUNAR_BATTLE_H
#define GUARD_LUNAR_BATTLE_H

#include "mode_base.h"
#include "mixins/comm_panel_drawer.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

#define COVER_MAX 10
// FIXME: Unify with mines_available values in lunar_battle_prep.cpp
#define MINES_MAX 12
#define TELE_MAX 3
#define BATTLE_UNITS_MAX 176    // Battlefield is 16x11

extern ExodusState exostate;

enum LBPanelMode {
    LBPM_None,
    LBPM_Placement,
    LBPM_Battle,
};

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
        void release_spr_id();
        BattleUnitType type;
        void do_move(Direction);
        const char* name;
        int x;
        int y;
        int tgt_x;
        int tgt_y;
        int hp;
        int move;
        int fire_range;
        float fire_rate;
        int fire_power;
        bool defending;
        bool can_shoot_behind;
        bool can_act;
        bool can_use_cover;
        bool is_alien;
        int moves_remaining;
        int shots_remaining;
        bool turn_taken;
        bool teleported;
        const char* idle;
        const char* walk;
        const char* fire;
        const char* dead;
        const char* move_sfx;
        const char* shoot_sfx;
        SprID spr_id;
        bool spr_id_set;
};

class Cover {
    public:
        Cover() {};
        Cover(int _x, int _y, bool _alt) : x(_x), y(_y), alt(_alt) {};
        int x;
        int y;
        bool alt;
        SprID spr_id;
};

class Mine {
    public:
        Mine() : x(0), y(0), live(true) {}
        int x;
        int y;
        bool live;
        SprID spr_id;
};

class Teleporter {
    public:
        Teleporter() : x(0), y(0) {}
        int x;
        int y;
        SprID spr_id;
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
            LB_Placement,
            LB_PlacementEnd,
            LB_StartBattle,
            LB_CommHuman,
            LB_CommAI,
            LB_SelectUnit,
            LB_Move,
            LB_Mine,
            LB_Tele,
            LB_Fire,
            LB_CalcDamage,
            LB_Damage,
            LB_CheckWon,
            LB_Won,
        };

        Stage stage;

        Player *aggressor;

        int auto_base;
        void auto_run();
        void auto_act(bool);
        void auto_kill(bool);

        bool manual_placement;
        bool placement_def;
        int placement_item;
        SprID placement_ids[8];
        int to_place_inf;
        int to_place_gli;
        int to_place_art;
        int to_place_msc;
        SprID fog_ids[88];

        int n_cover;
        Cover cover[COVER_MAX];

        int n_mines;
        Mine mines[MINES_MAX];

        int n_tele;
        Teleporter tele[TELE_MAX];

        int n_units;
        BattleUnit units[BATTLE_UNITS_MAX];

        bool select_unit();
        void reset_round();
        BattleUnit* active_unit;
        BattleUnit* target_unit;
        BattleUnit* panel_unit;
        bool is_in_cover(BattleUnit*);

        BattleUnit* unit_at(int, int);

        int cursor_x;
        int cursor_y;
        int cursor_prev_x;
        int cursor_prev_y;

        bool use_alt_aliens;

        void place_cover();
        void place_units(bool);
        void place_unit(BattleUnit);

        void draw_ground();
        void draw_units();
        void update_cursor();
        void update_arrows();
        void draw_explosion();

        LBPanelMode panel_mode;
        void update_panel();
        void update_panel_setup();
        void update_panel_battle();

        bool valid_placement(int, int);

        char get_valid_move_directions();
        Direction get_random_move_direction();

        bool ai_can_move_to(BattleUnit*, int, int);
        Direction ai_decide_move_direction();

        bool infinite_range();

        bool check_viable_targets();
        bool check_viable_target(BattleUnit*);
        bool set_target_unit();
        bool in_range(int, int);

        bool defender_turn;
        bool human_turn;

        bool unit_moving;
        float move_interp;
        float shot_interp;
        float exp_interp;
        float fire_time;

        int damage_to_apply;

        float mine_timer;
        int mine_beeps;
        bool mine_damage;
        Mine *active_mine;

        float tele_timer;
        Teleporter *active_tele;
        bool tele_done;
};

#endif
