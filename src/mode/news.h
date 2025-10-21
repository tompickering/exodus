#ifndef GUARD_NEWS_H
#define GUARD_NEWS_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"


extern EphemeralState ephstate;

class News : ModeBase {
    public:
        News();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        void draw_news();
};

#endif
