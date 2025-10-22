#ifndef GUARD_GALAXY_MAP_H
#define GUARD_GALAXY_MAP_H

#include "mode_base.h"
#include "mixins/galaxy_drawer.h"
#include "mixins/panel_drawer.h"
#include "mixins/comm_panel_drawer.h"
#include "mixins/bulletin_drawer.h"
#include "mixins/frame_drawer.h"
#include "mixins/menu_drawer.h"
#include "mixins/month_report.h"
#include "mixins/trade_buy.h"
#include "mixins/bombing.h"

#include "platform/platform.h"
#include "draw/draw.h"

#include "state/exodus_state.h"



// Enum order determines order of processing
enum MonthPassStage {
    MP_None,
    MP_TimeDelay,
    MP_CheckMissionFail,
    MP_StarshipRepairs,
    MP_UpdateReputation,
    MP_SunExpansion,
    MP_MoveArtificialPlanets,
    MP_EnemyTactics,
    MP_ArtificialWorldNews,
    MP_EnemyActions,
    MP_NoPlanetsCounsellor,
    MP_EndOfMonthReport,
    MP_UpdatePirateProbabilities,
    MP_GenerateLostPlanetReports,
    MP_PlanetBackgroundUpdate,
    MP_PlanetMainUpdate, // PROCcal_plan - comprised of MonthPassPlanetStages
    MP_UpdateAlienFly,
    MP_Missions,
    MP_PayOfficers,
    MP_GuildCommendations,
    MP_AlienExile,
    MP_UpdateHumanFly,
    MP_End,
};

enum MonthPassAIStage {
    // PROClordreturn
    MPAI_Return,
    // PROClorddies
    MPAI_Die,
    // PROCet_main
    MPAI_Hostilities,
    MPAI_Alliances,
    MPAI_UpdateTaxes,
    MPAI_DecideTerrorAttacks,
    MPAI_FleetPurchase,
    MPAI_NewOfficers,
    MPAI_DevelopArtificialPlanet,
    MPAI_MoveArtificialPlanet,
    MPAI_CheckArmy,
    MPAI_SwitchTactics,
    MPAI_ExecTactics,
    MPAI_End,
};

enum MonthPassPlanetStage {
    MPP_InitialiseReport,
    MPP_ShuffleTrade,
    MPP_FirstCity,
    MPP_FirstSpaceport,
    MPP_ResearchCheck,
    MPP_Research,
    MPP_Meteors,
    MPP_KillArmies,
    MPP_ClimateChange,
    MPP_Epidemic,
    MPP_AlienAttack,
    MPP_AlienAttackResult,
    MPP_AlienResearchCheck,
    MPP_AlienResearch,
    MPP_DiscoverSpecies,
    MPP_CityEpidemic,
    MPP_RebelAttack,
    MPP_RebelAttackResult,
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
    MPP_MilitaryFacilityShutdown2,
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
    MonthPassAIStage mpai_stage;
    MonthPassPlanetStage mpp_stage;
    float month_pass_time;
    int mp_player_idx;
    int mp_star_idx;
    int mp_planet_idx;
    Officer mp_officer;
    int mp_production_shutdown;

    int mpai_player_idx;
    int mpai_star_idx;
    int mpai_planet_idx;
    int mpai_substage;
    int mpai_substage_player_idx;

    int mpai_bombings_max;
    int mpai_bombings_remain;
    int mpai_original_bombers;
} MPState;

enum MissionState : uint8_t {
    MS_None,
    MS_Active,
    MS_AssassinCaptured,
    MS_AssassinCapturedDead,
    MS_AssassinCapturedKillChoice,
};

class GalaxyMap : ModeBase, GalaxyDrawer, PanelDrawer, CommPanelDrawer, BulletinDrawer, FrameDrawer, MenuDrawer, MonthReport, TradeBuy, Bombing {
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
            GM_Fly,
            GM_Counsellor,
            GM_SelectStar,
            GM_MonthPassing,
            GM_MonthPassMain,
            GM_MP_FirstCity,
            GM_MP_FirstSpaceport,
            GM_Menu,
            GM_ArtificialWorldStarSelect,
            GM_ArtificialWorldStarSelectInvalid,
            GM_OpenPlanetReports,
            GM_PlanetReports,
            GM_QuitConfirm,
        };

        Stage stage;
        FlyTarget *selected_ft;
        float selected_ft_blink;
        float first_spaceport_time;

        void set_stage(Stage);

        bool first_spaceport_update(float);

        MPState mp_state;
        ExodusMode month_pass_update();
        ExodusMode month_pass_ai_update();
        ExodusMode month_pass_planet_update();
        void month_pass_start();
        void month_pass_end();
        void next_mp_stage();
        void next_mpai_stage();
        void next_mpp_stage();
        void ai_planet_update(Planet*);

        PlanetReport report;
        void reset_planet_report();
        void planet_report_bulletin(bool, int);
        int planet_report_current;

        bool do_first_city;
        bool do_first_spaceport;
        bool do_meteor;
        bool do_meltdown;
        bool do_lunar_battle;
        GuildTitle do_guild_title;

        MissionState mission_state;
        int mission_n_strikes;

        bool update_researchcheck(Star*, Planet *p);
        bool update_research(Planet *p);

        void discover_species_bulletin(Planet*);

        Planet* artificial_planet_to_move;
};

#endif
