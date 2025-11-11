#ifndef GUARD_FINANCE_H
#define GUARD_FINANCE_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

class Finance : ModeBase {
    public:
        Finance();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        void draw_finance();
};

#endif
