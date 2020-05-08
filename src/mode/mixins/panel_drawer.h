#ifndef GUARD_PANEL_DRAWER_H
#define GUARD_PANEL_DRAWER_H

#include "draw/draw.h"

#include "state/exodus_state.h"
#include "galaxy/flytarget.h"

extern DRAWMANAGER draw_manager;

extern ExodusState exostate;

extern const DrawArea galaxy_panel_area;

class PanelDrawer {
    public:
        PanelDrawer();
        void draw_panel_bg(DrawTarget);
        void update_panel_info(DrawTarget, PlayerInfo*, FlyTarget*);
    protected:
        SprID id_panel;
        SprID id_name;
        SprID id_month;
        SprID id_mc;
        SprID id_planets;
        SprID id_qm;
        SprID id_desc;
};

#endif
