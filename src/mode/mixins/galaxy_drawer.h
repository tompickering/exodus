#ifndef GUARD_GALAXY_DRAWER_H
#define GUARD_GALAXY_DRAWER_H

#include "draw/draw.h"

#include "galaxy/galaxy.h"
#include "state/exodus_state.h"

extern DRAWMANAGER draw_manager;

extern ExodusState exostate;

extern const DrawArea galaxy_panel_area;

class GalaxyDrawer {
    public:
        GalaxyDrawer();
        FlyTarget *get_clicked_flytarget();
        void get_draw_position(const FlyTarget*, int&, int&);
    protected:
        void draw_galaxy(bool);
    private:
        SprID guild_id;
        SprID star_ids[GALAXY_MAX_STARS];
};

#endif
