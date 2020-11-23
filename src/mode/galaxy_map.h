#ifndef GUARD_GALAXY_MAP_H
#define GUARD_GALAXY_MAP_H

#include "mode_base.h"
#include "mixins/galaxy_drawer.h"
#include "mixins/panel_drawer.h"
#include "mixins/comm_panel_drawer.h"
#include "mixins/bulletin_drawer.h"
#include "mixins/frame_drawer.h"

#include "platform.h"
#include "draw/draw.h"

#include "state/exodus_state.h"

extern ExodusState exostate;

#define REPORT_LINES 20
#define REPORT_LINE_MAX 256

typedef struct {
    int items;
    char content[REPORT_LINES][REPORT_LINE_MAX];
    void add_line(const char* msg, ...) {
        if (items >= REPORT_LINES)
            return;

        content[items][REPORT_LINE_MAX - 1] = '\0';
        va_list args;
        va_start(args, msg);
        vsnprintf(content[items], REPORT_LINE_MAX - 2, msg, args);
        va_end(args);
        items++;
    }
} PlanetReport;

// Enum order determines order of processing
enum MonthPassStage {
    MP_None,
    MP_TimeDelay,
    MP_CheckMissionFail,
    MP_StarshipRepairs,
    MP_UpdateReputation,
    MP_SunExpansion,
    MP_MoveArtificialPlanets,
    MP_EnemyReturns,
    MP_EnemyDies,
    MP_EnemyTactics,
    MP_ArtificialWorldNews,
    MP_EnemyActions,
    MP_UpdatePirateProbabilities,
    MP_PlanetBackgroundUpdate,
    MP_PlanetMainUpdate, // PROCcal_plan - comprised of MonthPassPlanetStages
    MP_UpdateAlienFly,
    MP_AlienMissions,
    MP_PayOfficers,
    MP_GuildCommendations,
    MP_AlienExile,
    MP_UpdateHumanFly,
    MP_End,
};

enum MonthPassPlanetStage {
    MPP_ShuffleTrade,
    MPP_FirstCity,
    MPP_FirstSpaceport,
    MPP_Research1,
    MPP_Meteors,
    MPP_KillArmies,
    MPP_ClimateChange,
    MPP_Epidemic,
    MPP_AlienAttack,
    MPP_AlienAttackResult,
    MPP_Research2,
    MPP_DiscoverSpecies,
    MPP_CityEpidemic,
    MPP_RebelAttack,
    MPP_LawsIncreaseUnrest,
    MPP_ParksReduceUnrest,
    MPP_ClearRadiation,
    MPP_ReactorMeltdown,
    MPP_ClimateChangeDueToCultivation,
    MPP_ClimateChangeDueToDeforestation,
    MPP_LosePlanetControl,
    MPP_Income,
    MPP_ProducePlutonium,
    MPP_ProduceMilitary,
    MPP_Mining,
    // Can use excess plu in production / mining, but not trade
    MPP_ResourceCap1,
    MPP_MilitaryFacilityShutdown,
    MPP_Trade,
    MPP_VillageGifts,
    MPP_FoodPerishes,
    MPP_FoodProduction,
    MPP_AgriCollapse,
    MPP_CityExpansion,
    MPP_VillageExpansion,
    MPP_ConsumeFood,
    // Can consume excess food before we apply cap
    MPP_ResourceCap2,
    MPP_AdvanceUnrest,
    MPP_AmendMilitaryFunding,
    MPP_DisplayPlanetReport,
    MPP_EnsureComplete,
    MPP_End,
};

typedef struct {
    MonthPassStage mp_stage;
    MonthPassPlanetStage mpp_stage;
    float month_pass_time;
    int mp_player_idx;
    unsigned int mp_star_idx;
    int mp_planet_idx;
} MPState;

class GalaxyMap : ModeBase, GalaxyDrawer, PanelDrawer, CommPanelDrawer, BulletinDrawer, FrameDrawer {
    public:
        GalaxyMap();
        virtual void enter() override;
        virtual void exit() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            GM_SwapIn,
            GM_Idle,
            GM_Zoom2Guild,
            GM_Zoom2Star,
            GM_FlyConfirm,
            GM_MonthPassing,
            GM_MP_FirstCity,
        };

        Stage stage;
        FlyTarget *selected_ft;
        float selected_ft_blink;

        MPState mp_state;
        ExodusMode month_pass_update();
        ExodusMode month_pass_planet_update();
        void month_pass_start();
        void month_pass_end();
        void next_mp_stage();
        void next_mpp_stage();

        PlanetReport report;
        void reset_planet_report();

        bool do_first_city;
        bool do_meteor;
        bool do_meltdown;
        bool do_lunar_battle;
};

#endif
