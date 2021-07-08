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

class SpaceBattle : ModeBase {
    public:
        SpaceBattle();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
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
        void draw();
        BattleShip* find_ship(BattleShipType, bool);
        Rocket* spawn_rocket(BattleShip*, float, float);

        enum Stage {
            SB_Setup,
            SB_Battle,
            SB_Wait,
            SB_Exit,
        };

        Stage stage;

        BattleShip ships[MAX_SHIPS];
        BattleShip *next_ship;
        BattleShip *selected;
        BattleShip *starship;
        Player *player;

        Rocket rockets[MAX_ROCKETS];

        bool auto_battle;
        float frame_time_elapsed;
        SpaceBattleResolution resolution;

        int fail_ship_stats;
        int fail_diagnostics;
        bool fail_battle_readout;
        bool fail_battle_readout_this_frame;
};

#endif
