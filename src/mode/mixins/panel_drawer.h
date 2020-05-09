#ifndef GUARD_PANEL_DRAWER_H
#define GUARD_PANEL_DRAWER_H

#include "draw/draw.h"

#include "state/exodus_state.h"
#include "galaxy/flytarget.h"
#include "galaxy/star.h"

extern const int PNL_TOP;
extern const int PNL_BORDER;
extern const int PNL_Y_SEP;

extern DRAWMANAGER draw_manager;

extern ExodusState exostate;

extern const DrawArea galaxy_panel_area;

enum PanelType {
    PNL_Galaxy,
    PNL_Star,
};

class PanelDrawer {
    public:
        PanelDrawer(PanelType);
        void draw_panel_bg(DrawTarget);
        void update_panel_info_player(DrawTarget, PlayerInfo*);
        void update_panel_info_ft(DrawTarget, PlayerInfo*, FlyTarget*);
        void update_panel_info_planet(DrawTarget, PlayerInfo*, Planet*);
    protected:
        PanelType type;
        SprID id_panel;
        SprID id_name;
        SprID id_month;
        SprID id_mc;
        SprID id_planets;
        SprID id_qm;
        SprID id_desc;
        SprID id_desc1;
        SprID id_desc2;
        SprID id_planet_icons[STAR_MAX_PLANETS];
        SprID id_marker_icons[STAR_MAX_PLANETS];
};

#endif
