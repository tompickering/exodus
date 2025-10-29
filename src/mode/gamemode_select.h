#ifndef GUARD_GAMEMODE_SELECT_H
#define GUARD_GAMEMODE_SELECT_H

#include "mode_base.h"

class GameModeSelect: ModeBase {
    public:
        GameModeSelect();
        virtual void enter() override;
        virtual void exit() override;
        virtual ExodusMode update(float) override;
};

#endif
