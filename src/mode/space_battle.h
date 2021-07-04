#ifndef GUARD_SPACE_BATTLE_H
#define GUARD_SPACE_BATTLE_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

#include "assetpaths.h"

#define MAX_SHIPS_PER_TYPE 5
#define MAX_SHIPS (MAX_SHIPS_PER_TYPE * 4)

extern ExodusState exostate;
extern EphemeralState ephstate;
extern DRAWMANAGER draw_manager;

enum BattleShipType {
    SHIP_Starship,
    SHIP_Warship,
    SHIP_Bomber,
    SHIP_Scout,
};

enum BattleShipAction {
    BSA_Idle,
    BSA_Attack,
    BSA_Report,
};

struct BattleShip {
    void init(BattleShipType, bool, int, int, int);
    void init(BattleShipType, bool, int, int, int, int);
    void cleanup();

    BattleShipType type;
    bool exists;
    bool enemy;
    int x;
    int y;
    int hp;
    int shield;
    int shield_max;
    BattleShipAction action;
    int target;
    const char *spr;
    const char *spr_sel;

    SprID spr_id;
};

class SpaceBattle : ModeBase {
    public:
        SpaceBattle();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        void prepare();
        void draw();

        enum Stage {
            SB_Setup,
            SB_Battle,
            SB_Exit,
        };

        Stage stage;

        BattleShip ships[MAX_SHIPS];
        int selected;
};

#endif
