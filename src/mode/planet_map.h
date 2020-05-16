#ifndef GUARD_PLANET_MAP_H
#define GUARD_PLANET_MAP_H

#include "mode_base.h"

#include "draw/draw.h"

#include "galaxy/planet.h"
#include "state/exodus_state.h"

#include "anim.h"

extern ExodusState exostate;

enum Tool {
    TOOL_None,
    TOOL_HQ,
    TOOL_Cultivate,
    TOOL_Mine,
    TOOL_Plu,
    TOOL_City,
    TOOL_Clear,
    TOOL_Inf,
    TOOL_Gli,
    TOOL_Art,
    TOOL_Port0,
    TOOL_Port1,
    TOOL_Port2,
    TOOL_Trade,
    TOOL_LunarBase,
    TOOL_Park,
    TOOL_END,
};

class PlanetMap : ModeBase {
    public:
        PlanetMap();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            PM_Idle,
            PM_Construct,
        };

        Stage stage;
        Planet *planet;
        Tool active_tool;
        float anim_cycle;
        int surf_x;
        int surf_y;
        int menu_x;
        int menu_y;
        int blocks;
        void draw_stones(bool);
        void clear_surf(int, int);

        float construct_progress;
        Anim* construct_anim;

        int construct_x;
        int construct_y;
        Stone construct_stone;
        Anim* get_stone_anim(Stone);
        Anim* get_construct_anim(Stone);

        Tool get_tool_for_click(float, float);
        void set_tool(Tool);
        void draw_tool_rect(Tool, RGB);
        Stone tool2stone(Tool);
        int tool2cost(Tool);

        bool can_build_on(Stone);
};

#endif
