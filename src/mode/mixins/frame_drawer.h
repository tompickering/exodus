#ifndef GUARD_FRAME_DRAWER_H
#define GUARD_FRAME_DRAWER_H

#include "draw/draw.h"

extern DRAWMANAGER draw_manager;

class FrameDrawer {
    public:
        FrameDrawer();
        void frame_draw();
        void frame_remove();
    protected:
        bool _frame_open;
        SprID id_frame_t;
        SprID id_frame_b;
        SprID id_frame_l;
        SprID id_frame_r;
};

#endif
