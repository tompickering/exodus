#ifndef GUARD_GALAXY_DRAWER_H
#define GUARD_GALAXY_DRAWER_H

#include "draw/draw.h"

#include "galaxy/galaxy.h"
#include "state/exodus_state.h"

extern DRAWMANAGER draw_manager;



class GalaxyDrawer {
    public:
        GalaxyDrawer();
        FlyTarget *get_clicked_flytarget();
        FlyTarget *get_mouseover_flytarget();
        void get_draw_position(const FlyTarget*, int&, int&);
        void draw_markers(bool, bool);
    protected:
        void draw_galaxy(bool);
    private:
        SprID guild_id;
        SprID star_ids[GALAXY_MAX_STARS];
        SprID marker_ids[N_MARKERS];
        SprID marker_text_ids[N_MARKERS];
};

#endif
