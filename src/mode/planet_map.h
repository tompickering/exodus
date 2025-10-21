#ifndef GUARD_PLANET_MAP_H
#define GUARD_PLANET_MAP_H

#include "mode_base.h"

#include "draw/draw.h"

#include "galaxy/planet.h"
#include "state/exodus_state.h"

#include "anim.h"

#define MAX_CHAINED_EXPLOSIONS 256



enum Tool {
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

enum ExplodeState {
    EXP_None,
    EXP_Drawing,
    EXP_Done,
};

typedef struct {
    int x;
    int y;
    bool radiation;
} ChainedExplosion;

class PlanetMap : ModeBase {
    public:
        PlanetMap();
        virtual void enter() override;
        virtual void exit() override;
        virtual ExodusMode update(float) override;
    private:
        void draw();

        enum Stage {
            PM_Idle,
            PM_Construct,
            PM_Scout,
            PM_Destruction,
            PM_Frame,
            PM_Law,
            PM_LawJustice,
            PM_LawTrade,
            PM_LawTaxes,
            PM_LawFestival,
            PM_LawFestivalConfirm,
        };

        Stage stage;
        Planet *planet;
        Player *player;
        Tool active_tool;
        float anim_cycle;
        int surf_x;
        int surf_y;
        int menu_x;
        int menu_y;
        int blocks;
        void draw_stones();
        void update_gauges();
        void clear_surf(int, int);

        bool draw_menu;
        bool do_animations;

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
        void draw_mc();
        void draw_army_funding();

        void set_build_button(bool);
        void hide_lunar_base_tool();

        void draw_attack_details();

        ExodusMode update_destruction(float);
        bool destruction_done;
        float destruction_time;
        float replay_timer;
        float targeting_interp;
        float explosion_interp;
        ExplodeState exploding;
        Stone exploding_stone;
        int explode_x;
        int explode_y;
        int chained_explosion_idx;
        int chained_explosion_head;
        ChainedExplosion chained_explosions[MAX_CHAINED_EXPLOSIONS];

        void draw_frame_fd();
        void draw_frame_pl();
        void draw_frame_unrest();
        void draw_frame_help(Tool);
        void draw_frame(int, int, bool);
        void close_frame();

        void open_law_panel();
        void close_law_panel();
        void draw_law_panel();
        void clear_law_ids();

        bool law_changed;

        bool bomb_achievements_done;

        SprID id_stones[MAX_STONES];
};

#endif
