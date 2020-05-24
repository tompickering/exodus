#ifndef GUARD_PLANET_H
#define GUARD_PLANET_H

#define PLANET_MAX_NAME 12
#define PLANET_BLOCKS_SM 12
#define PLANET_BLOCKS_MD 14
#define PLANET_BLOCKS_LG 16
#define CITY_INCOME 2
#define CITY_FOOD_REQ 3

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
    Terra,       // 6   |
    Artificial,  // 8  -
};

enum Stone {
    STONE_Clear,         // 0
    STONE_NaturalLarge,  // 8
    STONE_NaturalSmall,  // 9
    STONE_NaturalAnim,   // 29
    STONE_Village,       // 22
    STONE_Agri,          // 2
    STONE_Base,
    STONE_AgriDead,
    STONE_Mine,
    STONE_Plu,
    STONE_City,
    STONE_Inf,
    STONE_Gli,
    STONE_Art,
    STONE_Port0,
    STONE_Port1,
    STONE_Port2,
    STONE_Trade,
    STONE_Park,
    STONE_Rubble,
    STONE_Radiation,
    STONE_END,
};

typedef struct {
    const char *panel_icon;
    const char *landscape;
    const char *map_bg;
    const char *surf;
} PlanetSpriteSet;

class Planet {
    public:
        Planet();
        Planet(PlanetClass);
        bool exists();
        PlanetClass get_class();
        const char* get_class_str();
        const char* get_class_str_lower();
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
        const char* get_name();      // Orig: SIn
        void set_name(const char*);
        bool is_named();
        const char* get_name_suggestion();
        const PlanetSpriteSet* sprites();
        bool is_owned();
        int get_owner();
        void set_owner(int);
        void unset_owner();
        Stone get_stone(int, int);
        void set_stone(int, int, Stone);
        bool has_stone(Stone);
        int count_stones(Stone);
        int get_army_funding();
        void adjust_army_funding(int);
        int get_army_required_mc();
        bool army_funding_sufficient();
        void prepare_for_cpu_lord(); // Orig: PROCgivestation
    private:
        void init();

        bool _exists;

        PlanetClass cls;
        char name[PLANET_MAX_NAME + 1];
        Stone surf[PLANET_BLOCKS_LG * PLANET_BLOCKS_LG];

        int pspeed;            // Orig: pspeed
        int sit;               // Orig: SIt. Trade-related?
        int diameter;          // Orig: SId.
        int minerals;          // Orig: SIrm.
        int sim;               // Orig: SIm. Battle-related?

        int owner;             // Orig: SIi

        bool lunar_base;       // Orig: SIb
        int unrest;            // Orig: SIr
        int army_funding;      // Orig: SIam
        int airdef_guns;       // Orig: SIv
        int robots;            // Orig: SIgr

        int reserves_min;      // Orig: SIg(1)
        int reserves_food;     // Orig: SIg(2)
        int reserves_plu;      // Orig: SIg(3)

        int army_inf;          // Orig: SIk(1)
        int army_gli;          // Orig: SIk(2)
        int army_art;          // Orig: SIk(3)

        // TODO: SIu, SIl, SItb

        void cull_stones_to_size();
        Stone _get_stone(int, int);
        void _set_stone(int, int, Stone);
        void _to_real(int, int, int&, int&);
        bool _real_in_bounds(int, int);
};

#endif
