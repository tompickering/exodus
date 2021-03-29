#ifndef GUARD_PLANET_H
#define GUARD_PLANET_H

#include <cstdint>

#define PLANET_MAX_NAME 12
#define PLANET_BLOCKS_SM 12
#define PLANET_BLOCKS_MD 14
#define PLANET_BLOCKS_LG 16
#define CITY_INCOME 2
#define CITY_FOOD_REQ 3
#define N_UNREST 8
#define MAX_STONES (PLANET_BLOCKS_LG * PLANET_BLOCKS_LG)

#define COST_LUNAR_BASE 120
#define COST_AIRDEF 5
#define COST_ROBOT  6

enum PlanetSize {
    PLANET_Small,
    PLANET_Medium,
    PLANET_Large,
};

enum PlanetClass {
    Forest,      // 1  -
    Desert,      // 2   |
    Volcano,     // 3   |
    Rock,        // 4    > Original codes for planet types
    Ice,         // 5   |
    Terra,       // 6   |     (Trivia: It looks like '7' was originally intended
    Artificial,  // 8  -       to represent uninhabitable gas giants).
};

// These are assigned the values that they had in the original.
// This is inconsequential, but can help with debugging!
enum Stone : uint8_t {
    STONE_Clear         =  0,
    STONE_Base          =  1,
    STONE_Agri          =  2,
    STONE_Mine          =  3,
    STONE_Plu           =  4,
    STONE_City          =  5,
    STONE_Radiation     =  6,
    STONE_NaturalLarge  =  8,
    STONE_NaturalSmall  =  9,
    STONE_AgriDead      = 10,
    STONE_Inf           = 11,
    STONE_Gli           = 12,
    STONE_Art           = 13,
    STONE_Port0         = 14,
    STONE_Port1         = 15,
    STONE_Port2         = 16,
    STONE_Trade         = 17,
    STONE_Rubble        = 20,
    STONE_Village       = 22,
    STONE_Park          = 23,
    STONE_NaturalAnim   = 29,
    STONE_END           = 30,
};

enum TradeQuality {
    TRADE_None,
    TRADE_Bad,
    TRADE_Fair,
    TRADE_Good,
};

enum Law {
    LAW_TradeMinerals,
    LAW_TradeFood,
    LAW_TradePlutonium,
    LAW_FreeSpeech,
    LAW_PrivateIndustry,
    LAW_DrugLegalisation,
    LAW_AllowSystemEnemies,
    LAW_DifferentReligions,
    LAW_CivilianWeapons,
};

enum MoonClass {
    MOON_Dirt,
    MOON_Ice,
    MOON_Grass,
};

typedef struct {
    const char *panel_icon;
    const char *landscape;
    const char *map_bg;
    const char *surf;
    const char *bulletin_bg;
} PlanetSpriteSet;

typedef struct {
    const char *bg;
    const char *cover0;
    const char *cover1;
} MoonSpriteSet;

typedef struct {
    bool no_money;
    bool no_space;
    bool no_plu;
    int inf;
    int gli;
    int art;
    int not_produced;
} ProductionReport;

typedef struct {
    int mi;
    int fd;
    int pl;
    int mc;
} TradeReport;

class Planet {
    public:
        static int stone_cost(Stone);

        Planet();
        Planet(PlanetClass);
        bool exists();
        int get_quality();
        PlanetClass get_class();
        const char* get_class_str();
        const char* get_class_str_lower();
        MoonClass get_moon_class();
        int get_diameter();
        PlanetSize get_size();
        int get_size_blocks();
        int get_settlement_cost();
        int get_base_income();       // Orig: SIs
        int get_income();            // Orig: SIs + SIc*2
        int get_net_income();        // Orig: SIs + SIc*2 - SIam
        int get_population();        // Orig: SIpo
        int get_day_hours();         // Orig: pspeed - except we multiply by 4 here.
        int get_n_cities();          // Orig: SIc
        int get_n_agri();            // Orig: SIna
        bool agri_sufficient();
        bool has_lunar_base();       // Orig: SIb
        void build_lunar_base();
        void destroy_lunar_base();
        bool has_spaceport();
        const char* get_name();      // Orig: SIn
        void set_name(const char*);
        bool is_named();
        const char* get_name_suggestion();
        const PlanetSpriteSet* sprites();
        const MoonSpriteSet* moon_sprites();
        bool is_owned();
        int get_owner();
        void set_owner(int);
        void unset_owner();
        Stone get_stone(int, int);
        void set_stone(int, int, Stone);
        Stone get_stone_wrap(int, int);
        void set_stone_wrap(int, int, Stone);
        bool has_stone(Stone);
        Stone get_random_point(int&, int&);
        bool find_random_stone(Stone, int&, int&);
        bool find_random_buildable_stone(int&, int&);
        bool find_random_buildable_stone_next_to_8(Stone, int&, int&);
        bool next_to_4(int, int, Stone);
        bool next_to_8(int, int, Stone);
        int count_stones(Stone);
        int get_food_production();
        int get_food_consumption();
        bool food_prod_sufficient();
        int get_plu_production();
        int get_plu_consumption();
        bool plu_prod_sufficient();
        int get_army_funding();
        void adjust_army_funding(int);
        void validate_army_funding();
        void discard_excess_resources();
        int get_army_required_mc();
        bool army_funding_sufficient();
        int get_minerals();
        int get_unrest();
        int get_unrest(int);
        int get_airdef_guns();
        int get_robots();
        void prepare_for_cpu_lord(); // Orig: PROCgivestation
        void adjust_unrest(int);
        void surfchange();
        void change_class(PlanetClass);
        bool surfchange_happened_this_month();
        bool trade_possible(int player_idx);
        TradeQuality initiate_trade(int player_idx);
        void month_reset();
        void randomise_trade_quality();
        bool can_collect_taxes();
        void collect_taxes();
        int get_tax_amount();
        bool may_be_attacked_by(int);
        void this_month_prevent_attacks_by(int);
        void set_law(Law, bool);
        bool has_law(Law);
        bool laws_cause_unrest();
        void clear_radiation();
        void produce_food();
        void produce_plutonium();
        ProductionReport produce_military();
        int consume_food();
        TradeReport monthly_trade();
        void mine();
        void perish_food();
        int get_resource_cap();
        void update_unrest_history();
        const char* get_moon_bg();
        bool has_army();
        void get_army(int&, int&, int&);
        int get_army_size();
        int get_army_size_weighted();
        void adjust_army(int, int, int);
        void adjust_reserves(int, int, int);
        int get_reserves_min();
        int get_reserves_food();
        int get_reserves_plu();
        int get_total_reserves();
        void disown();
        bool expand_city();
        bool expand_village();
        bool agri_collapse();
        void monthly_processing_start();
        bool monthly_processing_in_progress();
        void plunder();
        void ai_update();
    private:
        void init();

        bool _exists;

        bool expand(Stone);

        PlanetClass cls;
        MoonClass moon_cls;    // Determines battle background. Orig was random each time.
        char name[PLANET_MAX_NAME + 1];
        Stone surf[MAX_STONES];

        int pspeed;            // Orig: pspeed
        int diameter;          // Orig: SId.
        int minerals;          // Orig: SIrm.
        int sim;               // Orig: SIm. Battle-related?
        TradeQuality trade;    // Orig: SIt. Original adds 10 to disable trade for the month.
        uint16_t traded;       // Our alternative for SIt's +10 trade mask.

        // Orig: SIu - This tracks 2 bits of unrelated data!
        // SIu & 1: Tracks whether a player has been paid to cease an attack.
        // SIu & 2: Tracks whether a planet has been charged additional taxes this month.
        // I believe the former should be per-player, but the second should be per-month.
        uint16_t paid_to_leave; // Orig: SIu & 1 - but made per-player, like SIt/traded.
        bool taxes_collected;   // Orig: SIu & 2

        uint16_t laws;         // Orig: SIl

        int owner;             // Orig: SIi

        bool lunar_base;       // Orig: SIb
        int unrest[N_UNREST];  // Orig: SIr (current) / SItb (historical)
        int army_funding;      // Orig: SIam
        int airdef_guns;       // Orig: SIv
        int robots;            // Orig: SIgr

        int reserves_min;      // Orig: SIg(1)
        int reserves_food;     // Orig: SIg(2)
        int reserves_plu;      // Orig: SIg(3)

        int army_inf;          // Orig: SIk(1)
        int army_gli;          // Orig: SIk(2)
        int army_art;          // Orig: SIk(3)

        // TODO: SItb

        bool surfchange_this_month;
        bool processing_in_progress;

        void cull_stones_to_size();
        Stone _get_stone(int, int);
        void _set_stone(int, int, Stone);
        void _to_real(int, int, int&, int&);
        bool _real_in_bounds(int, int);

        void _ai_make_space();

        bool _ai_place_random(Stone);
        bool _ai_place_tactical(Stone, Stone);
        int ai_place_stone(int, Stone, Stone);  // Orig: PROCpl_stone
};

#endif
