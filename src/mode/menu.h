#ifndef GUARD_MENU_H
#define GUARD_MENU_H

#include "mode_base.h"

#include "../platform.h"
#include "../draw/draw.h"

class Menu : ModeBase {
    public:
        Menu();
        virtual void enter() override;
        virtual void exit() override;
        virtual ExodusMode update(float) override;
    private:
        bool pixelswap_done;
};

#endif
