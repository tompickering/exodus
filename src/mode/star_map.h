#ifndef GUARD_STAR_MAP_H
#define GUARD_STAR_MAP_H

#include "mode_base.h"
#include "mixins/panel_drawer.h"
#include "mixins/comm_panel_drawer.h"
#include "mixins/frame_drawer.h"
#include "mixins/bombing.h"

#include "draw/draw.h"

#include "galaxy/star.h"
#include "state/exodus_state.h"

extern ExodusState exostate;

class StarMap : ModeBase, PanelDrawer, CommPanelDrawer, FrameDrawer, Bombing {
    public:
        StarMap();
        virtual void enter() override;
        virtual void exit() override;
        virtual ExodusMode update(float) override;
        virtual bool should_push_to_stack() override;
    private:
        enum Stage {
            SM_Idle,
            SM_Fleet,
            SM_MissionScout,
            SM_MissionBomb,
            SM_PlanSettle,
            SM_PlanSettleFade,
            SM_PlanetComm,
            SM_EnemyComm,
            SM_PlanAttack,
            SM_FestivalDelay,
            SM_Festival,
            SM_SelectPlanet,
            SM_SelectPlanetFadeOut,
            SM_HandlePostPlanet,
            SM_HandlePostPlanetComms,
            SM_Counsellor,
            SM_PlanetRename,
            SM_Back2Gal,
        };

        Stage stage;
        Star *star;
        void draw_planets(float);
        float planet_progress[STAR_MAX_PLANETS];

        bool select_planet(int);
        void set_fleet_button(bool);
        void update_fleet_menu();

        float festival_delay;

        bool lookagain;

        SprID id_planet_names[STAR_MAX_PLANETS];
        SprID id_fleet_markers[STAR_MAX_PLANETS];

        int rename_planet;
};

#endif
