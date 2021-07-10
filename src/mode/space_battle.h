#ifndef GUARD_SPACE_BATTLE_H
#define GUARD_SPACE_BATTLE_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

#include "assetpaths.h"

#define MAX_SHIPS_PER_TYPE 5
#define MAX_SHIPS (MAX_SHIPS_PER_TYPE * 4)
#define MAX_ROCKETS 20
#define ROCKET_TTL 20
#define N_EXPLOSIONS 8

extern ExodusState exostate;
extern EphemeralState ephstate;
extern DRAWMANAGER draw_manager;

enum BattleShipType {
    SHIP_Starship,
    SHIP_Warship,
    SHIP_Bomber,
    SHIP_Scout,
    SHIP_Transporter,
};

enum BattleShipAction {
    BSA_Idle,
    BSA_AttackSlow,
    BSA_AttackFast,
    BSA_Report,
};

struct BattleShip {
    void init(BattleShipType, bool, float, float, int, int);
    void cleanup();

    BattleShipType type;
    bool exists;
    bool enemy;
    float x;
    float y;
    int hp;
    int shield;
    int shield_max;
    BattleShipAction action;
    BattleShip* target;
    bool draw_hit;
    const char *spr;
    const char *spr_sel;
    const char *spr_label;

    SprID spr_id;
    SprID spr_id_label;
};

struct Rocket {
    void init(float, float, float, float);
    void update();
    const char* get_sprite() const;
    void cleanup();

    bool exists;
    int ticks_alive;
    float x;
    float y;
    float dx;
    float dy;

    SprID spr_id;
};

enum SpaceBattleResolution {
    SBRES_None,
    SBRES_Won,
    SBRES_StarshipDestroyed,
};

struct Explosion {
    int x;
    int y;
    int frame;
    SprID spr_id;
};

class SpaceBattle : ModeBase {
    public:
        SpaceBattle();
        virtual void enter() override;
        virtual void exit() override;
        virtual ExodusMode update(float) override;
    private:
        Explosion explosions[N_EXPLOSIONS];
        BattleShip* place(BattleShipType, bool, int);
        BattleShip* place(BattleShipType, bool, int, int);
        void distribute(BattleShipType, bool, int, int, int);
        void prepare();
        void update_battle();
        void update_ships();
        void ships_think();
        void ships_act();
        void update_rockets();
        void update_mouse();
        void do_attack(BattleShip*);
        void start_explosions();
        void draw();
        BattleShip* find_ship(BattleShipType, bool);
        int count_ships(BattleShipType, bool);
        Rocket* spawn_rocket(BattleShip*, float, float);

        enum Stage {
            SB_Setup,
            SB_Battle,
            SB_Wait,
            SB_Info,
            SB_Surrender,
            SB_Resolved,
            SB_Destroyed,
            SB_Won,
            SB_Exit,
        };

        Stage stage;
        Stage update_buttons();

        BattleShip ships[MAX_SHIPS];
        BattleShip *next_ship;
        BattleShip *selected;
        BattleShip *starship;
        Player *player;

        Rocket rockets[MAX_ROCKETS];

        bool auto_battle;
        bool full_detail;
        float frame_time_elapsed;
        float resolution_delay;
        SpaceBattleResolution resolution;

        int fail_ship_stats;
        int fail_diagnostics;
        bool fail_battle_readout;
        bool fail_battle_readout_this_frame;

        bool do_explosions;
};

#endif
