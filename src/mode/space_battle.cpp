#include "space_battle.h"

#include "assetpaths.h"

enum ID {
    BACKGROUND,
    END,
};

SpaceBattle::SpaceBattle() : ModeBase("SpaceBattle") {
}

void SpaceBattle::enter() {
    ModeBase::enter(ID::END);

    draw_manager.draw(
        id(ID::BACKGROUND),
        IMG_RADAR,
        {0, 0, 0, 0, 1, 1});

    stage = SB_Setup;

    draw_manager.show_cursor(true);

    SpaceBattleParams &b = ephstate.space_battle;
    L.debug("STARTING BATTLE - ENEMY SHIPS: %d", b.enemy_ships);
}

ExodusMode SpaceBattle::update(float delta) {

    switch (stage) {
        case SB_Setup:
            {
                if (draw_manager.clicked()) {
                    stage = SB_Exit;
                }
            }
            break;
        case SB_Exit:
            {
                ephstate.set_ephemeral_state(EPH_ResumeFly);
                return ExodusMode::MODE_Pop;
            }
            break;
    }

    return ExodusMode::MODE_None;
}
