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
        FlyTarget *get_mouseover_flytarget(bool);
        void get_draw_position(const FlyTarget*, int&, int&);
        void draw_markers(bool, bool);
        void draw_planet_markers(bool);
        bool draw_planet_markers_for_star(bool, Star*, bool);
        void clear_mouseover_star_name();
        void draw_mouseover_star_name();
    protected:
        void draw_galaxy(bool);
        void draw_fleet_marker(SprID);
    private:
        SprID guild_id;
        SprID star_ids[GALAXY_MAX_STARS];
        SprID marker_ids[N_MARKERS];
        SprID marker_text_ids[N_MARKERS];
        SprID star_name_id;
        SprID star_name_sh_id;
        SprID star_name2_id;
        SprID star_name2_sh_id;
};

#endif
