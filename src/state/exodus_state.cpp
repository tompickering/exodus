#include "exodus_state.h"

#include "exodus_features.h"

#include <cstdio>
#include <cstring>

#include "util/iter.h"
#include "util/value.h"

#include "assetpaths.h"
#include "shared.h"

ExodusState *exostate_ptr = nullptr;

ExodusState& exostate() {
    if (exostate_ptr == nullptr) {
        exostate_ptr = new ExodusState();
    }

    return *exostate_ptr;
}

void exostate_destroy() {
    if (exostate_ptr != nullptr) {
        delete exostate_ptr;
        exostate_ptr = nullptr;
    }
}

int QUICKSAVE_SLOT = -1;

const char* flags[] = {
    IMG_TS1_FLAG13,
    IMG_TS1_FLAG14,
    IMG_TS1_FLAG15,
    IMG_TS1_FLAG10,
    IMG_TS1_FLAG11,
    IMG_TS1_FLAG12,
    IMG_TS1_FLAG7,
    IMG_TS1_FLAG8,
    IMG_TS1_FLAG9,
    IMG_TS1_FLAG4,
    IMG_TS1_FLAG5,
    IMG_TS1_FLAG6,
    IMG_TS1_FLAG1,
    IMG_TS1_FLAG2,
    IMG_TS1_FLAG3,
};

const char* emblems[] = {
    IMG_EMB_13,
    IMG_EMB_14,
    IMG_EMB_15,
    IMG_EMB_10,
    IMG_EMB_11,
    IMG_EMB_12,
    IMG_EMB_7,
    IMG_EMB_8,
    IMG_EMB_9,
    IMG_EMB_4,
    IMG_EMB_5,
    IMG_EMB_6,
    IMG_EMB_1,
    IMG_EMB_2,
    IMG_EMB_3,
};

ExodusState::ExodusState() {
    galaxy_finalised = false;
    active_flytarget_is_guild = true;
    first_city_done = false;
    first_spaceport_done = false;

    for (int i = 0; i < N_RACES; ++i) {
        race_name_counters[i] = 0;
    }
}

void ExodusState::init(GameConfig config) {
    galaxy_finalised = false;
    size = config.size;
    n_human_players = config.n_players;
    month = 1;
    active_player = 0;
    active_star = 0;
    active_planet = -1;
    first_city_done = false;
    first_spaceport_done = false;
    int i;

    for (i = 0; i < N_RACES; ++i) {
        race_name_counters[i] = 0;
    }

    for (i = 0; i < n_human_players; ++i) {
        memcpy((void*)(&players[i]), (void*)(&config.players[i]), sizeof(Player));
    }

    // PLAYER INIT: Everyone (overridable)
    for (i = 0; i < N_PLAYERS; ++i) {
        players[i].mc = 0;

        players[i].starship.reset();
        players[i].starship.crew = 5;

        if (ENHANCED()) {
            if (config.enemy_start == ENEMY_Medium) {
                players[i].starship.bionic_probes = 2;
            }
            if (config.enemy_start == ENEMY_Strong) {
                players[i].starship.bionic_probes = 5;
            }
        }

        players[i].fleet.scouts = 0;
        players[i].fleet.transporters = 0;
        players[i].fleet.warships = 0;
        players[i].fleet.bombers = 0;
        players[i].fleet.freight.minerals = 0;
        players[i].fleet.freight.food = 0;
        players[i].fleet.freight.plutonium = 0;
        players[i].fleet.freight.robots = 0;
        players[i].fleet.freight.infantry = 0;
        players[i].fleet.freight.gliders = 0;
        players[i].fleet.freight.artillery = 0;

        /*
         * TODO_MP: Probably needs something to say "this player doesn't exist
         * *at all*; as opposed to 'has left the galaxy' or 'might return'.
         * At the moment, non-existent players still appear under relationship menu.
         */
        if (config.enemy_start == ENEMY_None) {
            players[i].exists = false;
        } else {
            players[i].exists = true;
        }

        players[i].left_galaxy = false;
    }

    int mc_weak = 300;
    int mc_medium = 200;
    int mc_strong = 50;

    if (ENHANCED()) {
        /*
         * In Classic mode, the difficulty comes mainly from starting MC.
         * In Enhanced mode, other factors increase difficulty, so we're
         * a little kinder on the MC side.
         */
        mc_weak = 300;
        mc_medium = 250;
        mc_strong = 100;
    }

    // PLAYER INIT: Human
    for (i = 0; i < n_human_players; ++i) {
        players[i].init_character(CHAR_Human);
        players[i].exists = true;
        players[i]._intro_seen = false;
        players[i].fleet_marker_idx = i;

        if (config.enemy_start == ENEMY_None) {
            players[i].mc = 1000;
            players[i].fleet.transporters = 50;
            players[i].fleet.warships = 5;
        }

        if (config.enemy_start == ENEMY_Weak) {
            players[i].mc = mc_weak;
            players[i].fleet.transporters = 50;
            players[i].fleet.warships = 5;
        }

        if (config.enemy_start == ENEMY_Medium) {
            players[i].mc = mc_medium;
            players[i].fleet.transporters = 35;
            players[i].fleet.warships = 5;
        }

        if (config.enemy_start == ENEMY_Strong) {
            players[i].mc = mc_strong;
            players[i].fleet.transporters = 20;
            players[i].fleet.freight.minerals = 20;
        }
    }

    uint32_t characters = 0;

    // PLAYER INIT: CPU
    for (; i < N_PLAYERS; ++i) {
        Character c = CHAR_Yok0;

        if (FEATURE(EF_CHARACTERS)) {
            while (true) {
                int offset = rand() % ((int)CHAR_LAST - (int)CHAR_NEW - 1);

                if (!(characters & (1 << offset))) {
                    characters = characters | (1 << offset);
                    c = (Character)((int)CHAR_NEW + 1 + offset);
                    break;
                }
            }
        } else {
            switch (RND(4)) {
                case 1:
                    c = CHAR_Yok0;
                    break;
                case 2:
                    c = CHAR_Ter0;
                    break;
                case 3:
                    c = CHAR_Urk0;
                    break;
                case 4:
                    c = CHAR_Gor0;
                    break;
            }
        }

        players[i].init_character(c);

        players[i].fleet_marker_idx = 0;
        players[i].init_alien_name(); // Orig: Assigned in PROCclear_memory
        bool flag_assigned = false;
        L.debug("Assigning flag for player %d (CPU)", i);
        while (!flag_assigned) {
            int flag_try = rand() % 15;
            bool flag_ok = true;
            for (int j = 0; j < i; ++j) {
                if (flag_try == players[j].flag_idx) {
                    flag_ok = false;
                    break;
                }
            }
            if (flag_ok) {
                players[i].flag_idx = flag_try;
                flag_assigned = true;
                L.debug("Assigned flag %d", flag_try);
            }
        }
        unsigned int bonus_mult = 0;
        if (config.enemy_start == ENEMY_Medium) bonus_mult = 1;
        if (config.enemy_start == ENEMY_Strong) bonus_mult = 2;

        players[i].mc = 300 + RND(5)*50 + bonus_mult*150;
        L.debug("Assigned %d MC to CPU player %d", players[i].mc, i);

        players[i].fleet.transporters = 50 + bonus_mult * 100;
        players[i].clear_ai_attack();
    }

    // PLAYER INIT: Everyone (not overridable)
    for (i = 0; i < N_PLAYERS; ++i) {
        char fullname[MAX_PLAYER_FULLNAME+1];
        snprintf(
            fullname,
            MAX_PLAYER_FULLNAME + 1,
            "%s %s",
            players[i].title,
            players[i].name);
        snprintf(
            players[i].full_name,
            MAX_PLAYER_FULLNAME + 1,
            fullname);
        players[i].guild_title = GUILDTITLE_None;
        players[i].dead = false;
        players[i].reputation = 3;
        players[i].init_tax();
        players[i].set_tactic(0);
        players[i].nopirates = 0;

        set_random_hostility(players[i]);

        for (int j = 0; j < OFFICER_MAX; ++j) {
            players[i].set_officer((Officer)j, OFFQ_Poor);
        }
        players[i].infraction_mask = 0;

        for (int j = 0; j < TRACE_MAX; ++j) {
            players[i].trace[j] = 0;
        }

        for (int j = 0; j < N_MARKERS; ++j) {
            players[i].star_markers[j].reset();
        }

        players[i].mission.reset();
        players[i].set_trade_charge();

        players[i].mc_at_end_of_previous_month = players[i].mc;
        players[i].mc_at_start_of_current_month = players[i].mc;

        for (int j = 0; j < MAX_MC_CATEGORIES; ++j) {
            players[i].mc_gains_lifetime[j] = 0;
            players[i].mc_losses_lifetime[j] = 0;
            players[i].mc_gains_last_month[j] = 0;
            players[i].mc_losses_last_month[j] = 0;
            players[i].mc_gains_this_month[j] = 0;
            players[i].mc_losses_this_month[j] = 0;
        }

        for (int j = 0; j < MAX_MC_HISTORY; ++j) {
            players[i].mc_history[j] = 0;
        }

        for (int j = 0; j < OFFICERS_TOTAL; ++j) {
            players[i].officers_fired_nopay[j] = false;
            players[i].officers_quit[j] = false;
            players[i].officers_deceased[j] = false;
        }
    }

    for (int j = 0; j < N_PLAYERS; ++j) {
        for (int i = 0; i < N_PLAYERS; ++i) {
            alliance_matrix[j*N_PLAYERS + i] = 0;
        }
        alliance_requests[j] = 0;
    }

    for (int j = 0; j < N_PLAYERS; ++j) {
        for (int i = 0; i < N_PLAYERS; ++i) {
            lord_discovery_matrix[j*N_PLAYERS + i] = false;
        }
    }

    aim = config.aim;
    enemy_start = config.enemy_start;

    recommended_planet_star_idx = -1;
    recommended_planet_idx = -1;

    // PLAYER INIT: Character perk init
    if (FEATURE(EF_CHARACTERS)) {
        for (i = 0; i < N_PLAYERS; ++i) {
            players[i].init_character_perks();
        }
    }

    L.debug("ExodusState init");
    L.debug("      Size: %d", size);
    L.debug("   Players: %d", n_human_players);
    for (int i = 0; i < n_human_players; ++i) {
        Player *inf = &players[i];
        L.debug("       %s %s : Flag %d", inf->title, inf->name, inf->flag_idx);
    }
    L.debug("       Aim: %d", aim);
    L.debug("   Enemies: %d", enemy_start);
}

Planet* ExodusState::select_planet_for_cpu(bool worst) {
    // Assign initial planet to CPU lords. Orig: PROCstart_the_lords
    // We follow the original's logic of iterating over stars in the
    // same order for each lord, which affects the probability of
    // neighbouring lords in the same system.
    // N.B. Star indices are not correlated with their positions.
    bool initial_planet_selected = false;
    Star *chosen_star = nullptr;
    int chosen_planet_idx = 0;
    while(!initial_planet_selected) {
        int quality = 0;

        if (worst) {
            quality = 100;
        }

        for (PlanetIteratorRandom piter; !piter.complete(); ++piter) {
            Planet *p = piter.get();
            if (!p->is_owned()) {

                if (p->is_named()) {
                    continue;
                }

                L.verb("Considering star %d planet %d", piter.get_star_idx(), piter.get_idx());
                int planet_quality = 0;
                if (p->get_class() == Forest)  planet_quality = 4;
                if (p->get_class() == Desert)  planet_quality = 2;
                if (p->get_class() == Volcano) planet_quality = 1;
                if (p->get_class() == Rock)    planet_quality = 3;
                if (p->get_class() == Ice)     planet_quality = 2;
                if (p->get_class() == Terra)   planet_quality = 5;

                bool preferred = (planet_quality > quality);

                if (worst) {
                    preferred = (planet_quality < quality);
                }

                if (preferred && RND(7) != 1) {
                    L.verb("Checking existing ownership");
                    if (!p->is_owned()) {
                        L.verb("Updated planet candidate!");
                        quality = planet_quality;
                        chosen_star = piter.get_star();
                        chosen_planet_idx = piter.get_idx();
                        initial_planet_selected = true;
                        // N.B. we don't break here - we continue iterating
                        // over stars, and are liable to change our mind if
                        // we find a better offer!
                    } else {
                        L.verb("Can't choose this planet");
                    }
                }
            }
        }
    }

    Planet *chosen_planet = chosen_star->get_planet(chosen_planet_idx);
    return chosen_planet;
}

void ExodusState::init_cpu_lords() {
    for (int i = n_human_players; i < N_PLAYERS; ++i) {
        L.verb("Choosing initial planet for CPU lord %d", i);

        Planet *chosen_planet = select_planet_for_cpu(false);

        if (!chosen_planet) {
            L.error("Could not find a planet for CPU");
        }

        chosen_planet->set_owner(i, POCR_Init);
        chosen_planet->prepare_for_cpu_lord();

        if (players[i].mc >= 190) {
            players[i].mc -= 190;
        } else {
            players[i].mc = 0;
        }

        // PROCstart_the_lords sets 'firstplanet' here - we refer to owns_a_planet()

        players[i].get_location().set(get_random_star_idx());
        players[i].get_location().unset_target();
    }

    if (get_enemy_start() == ENEMY_Strong) {
        planet_gift_event();

        for (int i = 0; i < N_PLAYERS; ++i) {
            int m = players[i].perk_art_planet_at_month;
            if (m > 0) {
                Planet *art_planet = gift_art_planet_to(i);
                if (art_planet) {
                    art_planet->finalise_construction();
                    players[i].perk_art_planet_at_month = -1;
                }
            }
        }
    }
}

void ExodusState::generate_galaxy() {
    if (galaxy_finalised) {
        L.error("Cannot regenerate galaxy after finalisation");
    }

    int n_stars = 15;
    if (size == GAL_Medium)
        n_stars = 25;
    if (size == GAL_Large)
        n_stars = 40;

    new(&galaxy) Galaxy(n_stars);
}

void ExodusState::finalise_galaxy() {
    galaxy_finalised = true;
    init_cpu_lords();
}

Galaxy* ExodusState::get_galaxy() {
    return &galaxy;
}

int ExodusState::get_n_human_players() {
    return n_human_players;
}

bool ExodusState::multiplayer() {
    return get_n_human_players() > 1;
}

int ExodusState::get_month() {
    return month;
}

int ExodusState::get_orig_month() {
    /*
     * For some reason, the original tracks months at 1 greater than what is
     * displayed in the UI. The 'months' variable is set to 2 during some of
     * the game init, in PROCstart_the_lords.
     * It's easier to equate the original and new code when we can see that
     * it's using the same month measure, so provide an interface.
     */
    return get_month() - 1;
}

Aim ExodusState::get_aim() {
    return aim;
}

GalaxySize ExodusState::get_galaxy_size() {
    return size;
}

EnemyStart ExodusState::get_enemy_start() {
    return enemy_start;
}

void ExodusState::advance_month() {
    ++month;
    newsitem_head = 0;
    reset_planet_reports();
}

bool ExodusState::final_month() {
    switch (aim) {
        case AIM_Might:
            return month >= 200;
        case AIM_Money:
            return month >= 200;
        case AIM_Civilization:
            return month >= 350;
    }

    return false;
}

bool ExodusState::mission_complete() {
    Player *player = get_active_player();
    int player_idx = get_player_idx(player);

    switch (aim) {
        case AIM_Might:
            {
                for (PlanetIterator piter; !piter.complete(); ++piter) {
                    if (piter->is_owned() && piter->get_owner() != player_idx) {
                        return false;
                    }
                }
            }
            break;
        case AIM_Money:
            {
                int income = 0;

                if (FEATURE(EF_AIM_MONEY_EXCLUDE_ARMY)) {
                    income = get_total_income_ignoring_army(player_idx);
                } else {
                    income = get_total_net_income(player_idx);
                }

                if (income < 1000) {
                    return false;
                }
            }
            break;
        case AIM_Civilization:
            {
                // SUGGEST: Actually let players know what the requirements are!

                if (!player->has_all_inventions()) {
                    return false;
                }

                // Seizing a planet doesn't count - you need to construct at least one!
                if (!player->art_planet_constructed) {
                    return false;
                }

                bool artificial_owned = false;
                int n_planets = 0;

                for (PlanetIterator piter(player_idx); !piter.complete(); ++piter) {
                    n_planets++;
                    if (piter->get_class() == Artificial) {
                        artificial_owned = true;
                    }
                }

                int lim = min(30, get_n_planets());
                if ((n_planets < lim) || (!artificial_owned)) {
                    return false;
                }
            }
            break;
    }
    return true;
}

Player* ExodusState::get_active_player() {
    return get_player((int)active_player);
}

int ExodusState::get_active_player_idx() {
    return (int)active_player;
}

Player* ExodusState::set_active_player(int idx) {
    if (idx >= N_PLAYERS) {
        L.fatal("Attempt to set invalid player active %d", idx);
    }

    L.verb("ACTIVE PLAYER: %d -> %d", active_player, idx);
    active_player = idx;
    return get_active_player();
}

Star* ExodusState::get_active_star() {
    return &(get_galaxy()->get_stars()[active_star]);
}

int ExodusState::get_active_star_idx() {
    return active_star;
}

Planet* ExodusState::get_active_planet() {
    if (active_planet < 0) {
        return nullptr;
    }

    return get_galaxy()->get_stars()[active_star].get_planet(active_planet);
}

int ExodusState::get_active_planet_idx() {
    return active_planet;
}

Planet* ExodusState::get_planet(int star_idx, int planet_idx) {
    return get_galaxy()->get_stars()[star_idx].get_planet(planet_idx);
}

FlyTarget* ExodusState::get_active_flytarget() {
    if (active_flytarget_is_guild) {
        return get_galaxy()->get_guild();
    }

    return get_active_star();
}

void ExodusState::set_active_flytarget(FlyTarget* new_target) {
    L.verb("Active target: %s", new_target->name);
    active_planet = -1;

    if (new_target == get_galaxy()->get_guild()) {
        active_flytarget_is_guild = true;
        return;
    }

    active_flytarget_is_guild = false;

    for (int i = 0; i < GALAXY_MAX_STARS; ++i) {
        if (&get_galaxy()->get_stars()[i] == new_target) {
            active_star = i;
        }
    }

}

void ExodusState::set_active_planet(int new_planet) {
    L.verb("Active planet: %d", new_planet);
    active_planet = new_planet;
}

void ExodusState::set_active_planet(Planet *new_planet) {
    Star *s = get_star_for_planet(new_planet);
    for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
        if (s->get_planet(i) == new_planet) {
            set_active_flytarget(s);
            set_active_planet(i);
            return;
        }
    }

    L.error("Planet could not be set active");
}

int ExodusState::tgt2loc(FlyTarget* tgt) {
    if (tgt == get_galaxy()->get_guild())
        return -1;
    for (int i = 0; i < GALAXY_MAX_STARS; ++i)
        if (&get_galaxy()->get_stars()[i] == tgt)
            return i;
    L.fatal("Invalid location");
    return -1;
}

FlyTarget* ExodusState::loc2tgt(int loc) {
    if (loc < 0)
        return get_galaxy()->get_guild();
    return &get_galaxy()->get_stars()[loc];
}

int ExodusState::count_visited_stars(Player* player) {
    int visited = 0;

    for (StarIterator siter; !siter.complete(); ++siter) {
        if (player->get_location().has_visited(siter.get_idx())) {
            ++visited;
        }
    }

    return visited;
}

bool ExodusState::player_has_visited_all_stars(Player* player) {
    for (StarIterator siter; !siter.complete(); ++siter) {
        if (!player->get_location().has_visited(siter.get_idx())) {
            return false;
        }
    }

    return true;
}

Player* ExodusState::get_player(int idx) {
    if (idx < 0 || idx >= N_PLAYERS) {
        L.fatal("Request for invalid player %d", idx);
        return nullptr;
    }

    return &players[idx];
}

int ExodusState::get_player_idx(Player* player) {
    for (int i = 0; i < N_PLAYERS; ++i)
        if (&players[i] == player)
            return i;

    L.fatal("Index request for invalid player");
    return 0;
}

int ExodusState::get_n_active_players() {
    int n_active_players = 0;
    for (int i = 0; i < N_PLAYERS; ++i)
        if (players[i].is_participating())
           ++n_active_players;
    return n_active_players;
}

int ExodusState::get_n_active_cpu_players() {
    int n_active_players = 0;
    for (int i = 0; i < N_PLAYERS; ++i)
        if (players[i].is_participating() && !players[i].is_human())
           ++n_active_players;
    return n_active_players;
}

Player* ExodusState::get_random_active_player() {
    int n_active_players = get_n_active_players();
    int rand_idx = rand() % n_active_players;
    for (int i = 0; i < N_PLAYERS; ++i) {
        if (players[i].is_participating()) {
            if (rand_idx-- == 0) {
                return &players[i];
            }
        }
    }
    L.fatal("Couldn't find active player which we checked exists");
    return nullptr;
}

uint32_t ExodusState::get_active_cpu_player_mask() {
    uint32_t res = 0;
    for (int i = 0; i < N_PLAYERS; ++i) {
        if (players[i].is_participating() && !players[i].is_human()) {
            res |= (1 << i);
        }
    }
    return res;
}

Star* ExodusState::get_star(int star_idx) {
    return &(get_galaxy()->get_stars()[star_idx]);
}

int ExodusState::get_star_idx(Star* star) {
    for (StarIterator siter; !siter.complete(); ++siter) {
        if (siter.get() == star) {
            return siter.get_idx();
        }
    }

    L.warn("Could not find index of star at %x", star);
    return -1;
}

int ExodusState::get_random_star_idx() {
    Galaxy *gal = get_galaxy();
    int n_stars;
    gal->get_stars(n_stars);
    return rand() % n_stars;
}

int ExodusState::get_months_between(const FlyTarget* a, const FlyTarget* b) {
    // Orig stores pixel locations instead of low-res grid values
    // See PROC_Csunpos for original's multiplaication by 43
    int dx = (a->x - b->x) * 43;
    int dy = (a->y - b->y) * 43;
    int dist = sqrt((dx*dx) + (dy*dy));
    return 1 + (dist/300);
}

int ExodusState::get_n_owned_planets() {
    return _get_n_owned_planets(true);
}

int ExodusState::get_n_unowned_planets() {
    return _get_n_owned_planets(false);
}

int ExodusState::_get_n_owned_planets(bool owned) {
    int n_planets = 0;
    for (PlanetIterator piter; !piter.complete(); ++piter) {
        Planet *p = piter.get();
        if (p->is_owned() == owned) {
            ++n_planets;
        }
    }
    return n_planets;
}

/*
 * Can an artificial planet be created OR MOVED here?
 */
ArtificialPlanetViable ExodusState::artificial_planet_viable(Star* tgt) {
    Planet *tgt_planet = tgt->get_artificial_world_slot();

    /*
     * FIXME: In THEORY we could target a star with an existing planet
     * which was scheduled to be moved away (get_star_target() is set
     * and is not equal to the star that it's current at). But this
     * complicates moving, to make sure that you don't destroy data
     * by moving the planet memory around in the wrong order. We
     * don't support thia, and just bail if a planet exists.
     */
    if (tgt_planet->exists() || tgt_planet->get_construction_phase() > 0) {
        return APV_No_StarFull;
    }

    // The slot itself is free - check if anything is scheduled to move there

    int tgt_idx = tgt2loc(tgt);
    Galaxy *gal = get_galaxy();
    int n_stars;
    Star *stars = gal->get_stars(n_stars);
    for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
        Star *s = &stars[star_idx];
        Planet *p = s->get_artificial_world_slot();
        if (p && (p->exists() || p->get_construction_phase() > 0) && p->get_star_target() == tgt_idx) {
            // This planet is scheduled to move here
            return APV_No_MoveDest;
        }
    }

    return APV_Yes;
}

bool ExodusState::construct_artificial_planet(Star* s, int player_idx, const char* name) {
    if (artificial_planet_viable(s) != APV_Yes) {
        return false;
    }

    /*
     * Should be the only call to construct_artificial_world().
     * All calls should go through ExodusState.
     * Should always return a valid pointer as we already performed a viability check.
     */
    return s->construct_artificial_world(player_idx, name) != nullptr;
}

Planet* ExodusState::get_planet_under_construction(int player_idx) {
    Galaxy *gal = get_galaxy();
    int n_stars;
    Star *stars = gal->get_stars(n_stars);
    for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
        Star *s = &stars[star_idx];
        for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
            Planet *p = s->get_planet_nocheck(planet_idx);
            if (p && !p->exists() && p->get_construction_phase() > 0 && p->get_owner() == player_idx) {
                return p;
            }
        }
    }

    return nullptr;
}

PlanetInfo ExodusState::get_random_owned_planet_info() {
    PlanetInfo info;
    int n_owned_planets = get_n_owned_planets();
    int rand_idx = rand() % n_owned_planets;
    Galaxy *gal = get_galaxy();
    int n_stars;
    Star *stars = gal->get_stars(n_stars);
    for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
        Star *s = &stars[star_idx];
        for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
            Planet *p = s->get_planet(planet_idx);
            if (p && p->exists() && p->is_owned()) {
                if (rand_idx-- == 0) {
                    info.planet = p;
                    info.star = s;
                    info.index = planet_idx;
                    return info;
                }
            }
        }
    }
    L.fatal("Couldn't find owned planet which we checked exists");
    return info;
}

int ExodusState::get_n_planets() {
    int count = 0;
    for (PlanetIterator piter; !piter.complete(); ++piter) {
        ++count;
    }
    return count;
}

int ExodusState::get_n_planets(Player* player) {
    int count = 0;
    for (PlanetIterator piter(get_player_idx(player)); !piter.complete(); ++piter) {
        ++count;
    }
    return count;
}

/*
 * FIXME: This serves as firstplanet() from orig, but orig only resets
 * the flag to 0 in PROCcheck_alive - whereas we always return an
 * up-to-date value - which means our behaviour might deviate during
 * month pass.
 */
bool ExodusState::owns_a_planet(Player* player) {
    for (PlanetIterator piter; !piter.complete(); ++piter) {
        if (piter->is_owned() && get_player(piter->get_owner()) == player) {
            return true;
        }
    }
    return false;
}

bool ExodusState::any_human_has_visited(int idx) {
    for (int i = 0; i < N_PLAYERS; ++i) {
        Player *p = get_player(i);
        if (p->is_participating() && p->is_human()) {
            if (p->get_location().has_visited(idx)) {
                return true;
            }
        }
    }
    return false;
}

bool ExodusState::active_player_local() {
    Player *player = get_active_player();
    if (player->get_location().in_flight())
        return false;
    FlyTarget *orbit = loc2tgt(player->get_location().get_target());
    return orbit == get_active_flytarget();
}

PlanetInfo ExodusState::recommend_planet() {
    PlanetInfo info;
    int quality = -1;

    for (PlanetIterator piter; !piter.complete(); ++piter) {
        Planet *p = piter.get();
        int this_quality = p->get_quality();

        Star *s = piter.get_star();

        for (int i = 0;  i < STAR_MAX_PLANETS; ++i) {
            if (i == piter.get_idx()) {
                continue;
            }
            Planet *neighbour = s->get_planet(i);
            if (neighbour && neighbour->exists() && neighbour->is_owned()) {
                this_quality += 2;
                break;
            }
        }

        if (this_quality > quality) {
            quality = this_quality;
            info.planet = p;
            info.star = s;
            info.planet_idx = piter.get_idx();
            info.star_idx = piter.get_star_idx();
            info.index = piter.get_idx();
        }
    }
    return info;
}

void ExodusState::save_recommended_planet(const PlanetInfo& info) {
    recommended_planet_star_idx = info.star_idx;
    recommended_planet_idx = info.planet_idx;
}

bool ExodusState::is_recommended_planet(Planet* p) {
    int p_idx = -1;
    int s_idx = -1;
    get_star_planet_idx(p, s_idx, p_idx);
    return (recommended_planet_star_idx = s_idx && recommended_planet_idx == p_idx);
}

Player* ExodusState::get_hostile_to(Player& p) {
    Player *result = nullptr;
    int hostile_to_idx = p.get_hostile_to();
    if (hostile_to_idx >= 0) {
        Player *hostile_to = get_player(hostile_to_idx);
        if (hostile_to && hostile_to->is_participating()) {
            result = hostile_to;
        }
    }
    return result;
}

void ExodusState::set_random_hostility(Player& p) {
    int count = 0;
    for (int i = 0; i < N_PLAYERS; ++i) {
        if ((&players[i] != &p) && players[i].is_participating()) {
            count++;
        }
    }
    int idx = rand() % count;
    for (int i = 0; i < N_PLAYERS; ++i) {
        if ((&players[i] != &p) && players[i].is_participating()) {
            if (idx == 0) {
                p.set_hostile_to(i);
                return;
            }
            idx--;
        }
    }
}

int ExodusState::count_hostile_players(int p_idx) {
    int count = 0;

    for (int i = 0; i < N_PLAYERS; ++i) {
        if (i == p_idx) {
            continue;
        }

        Player *other_player = get_player(i);

        if (other_player->is_participating() && other_player->get_hostile_to() == p_idx) {
            ++count;
        }
    }

    return count;
}

bool ExodusState::hostility_between(int a, int b) {
    if (a < 0 || b < 0) {
        return false;
    }

    Player *pa = get_player(a);
    Player *pb = get_player(b);

    return (pa->is_hostile_to(b) || pb->is_hostile_to(a));
}

bool ExodusState::is_allied(int a, int b) {
    return get_alliances(a, b) > 0;
}

bool ExodusState::is_allied_with_race(int a, Race race) {
    for (int i = 0; i < N_PLAYERS; ++i) {
        if (i == a) {
            continue;
        }

        Player *p = get_player(i);

        if (!p->is_participating()) {
            continue;
        }

        if (p->get_race() == race) {
            if (is_allied(a, i)) {
                return true;
            }
        }
    }

    return false;
}

bool ExodusState::has_alliance(int a, int b, AllianceType t) {
    return (get_alliances(a, b) & (1<<(int)t)) > 0;
}

bool ExodusState::has_only_alliance(int a, int b, AllianceType t) {
    return (get_alliances(a, b) == (1<<(int)t));
}

bool ExodusState::has_all_alliances(int a, int b) {
    return get_alliances(a, b) == 0x7;
}

void ExodusState::set_alliance(int a, int b, AllianceType t) {
    set_alliances(a, b, get_alliances(a, b) | (1<<(int)t));
}

void ExodusState::set_all_alliances(int a, int b) {
    set_alliances(a, b, 0x7);
}

void ExodusState::unset_alliance(int a, int b, AllianceType t) {
    set_alliances(a, b, get_alliances(a, b) & ~(1<<(int)t));
}

void ExodusState::unset_alliances(int a, int b) {
    set_alliances(a, b, 0);
}

int ExodusState::get_total_income_ignoring_army(int player_idx) {
    int total = 0;
    for (PlanetIterator piter(player_idx); !piter.complete(); ++piter) {
        total += piter->get_income();
    }
    return total;
}

int ExodusState::get_total_net_income(int player_idx) {
    int total = 0;
    for (PlanetIterator piter(player_idx); !piter.complete(); ++piter) {
        total += piter->get_net_income();
    }
    return total;
}

int ExodusState::get_total_army(int player_idx) {
    int total = 0;
    for (PlanetIterator piter(player_idx); !piter.complete(); ++piter) {
        total += piter->get_army_size();
    }
    return total;
}

int ExodusState::count_alliances(int a) {
    int count = 0;
    for (int b = 0; b < N_PLAYERS; ++b) {
        if (is_allied(a, b)) {
            count++;
        }
    }
    return count;
}

bool ExodusState::can_request_alliance(int other, AllianceType t) {
    if (FEATURE(EF_ALLIANCE_LIMIT)) {
        return !(alliance_requests[other] & (1 << (int)t));
    }

    return true;
}

void ExodusState::register_request_alliance(int other, AllianceType t) {
    alliance_requests[other] |= (1 << (int)t);
}

void ExodusState::reset_alliance_requests() {
    for (int i = 0; i < N_PLAYERS; ++i) {
        alliance_requests[i] = 0;
    }
}

bool ExodusState::kill(Player* p) {
    if (!p->is_participating())
        return false;

    // PROClorddies

    // N.B. race doesn't change - if it did we'd want to do this first

    if (FEATURE(EF_CHARACTERS)) {
        Character c = CHAR_Yok0;
        switch (p->get_race()) {
            case RACE_Yokon:
                c = CHAR_Yok0;
                break;
            case RACE_Teri:
                c = CHAR_Ter0;
                break;
            case RACE_Urkash:
                c = CHAR_Urk0;
                break;
            case RACE_Gordoon:
                c = CHAR_Gor0;
                break;
            default:
                break;
        }

        p->init_character(c);
    }

    p->init_alien_name();
    p->give_mc(200, MC_StartCredits);
    p->clear_hostility();

    int p_idx = get_player_idx(p);

    for (int i = 0; i < N_PLAYERS; ++i) {
        if (i == p_idx)
            continue;
        unset_alliances(p_idx, i);
    }

    return true;
}

static NewsItem mock;

NewsItem& ExodusState::register_news(NewsItemType type) {
    int s = get_active_star_idx();

    bool ok = false;
    for (int i = 0; i < N_PLAYERS; ++i) {
        Player *player = get_player(i);
        if (player->is_human() && player->get_location().has_visited(s)) {
            ok = true;
            break;
        }
    }

    if (!ok) {
        // No player has visited this system
        // This news item won't be displayed
        // Return a fake reference
        return mock;
    }

    return register_news_force(type);
}

NewsItem& ExodusState::register_news_force(NewsItemType type) {
    int s = get_active_star_idx();
    int p = get_active_planet_idx();

    if (newsitem_head >= MAX_NEWSITEMS) {
        L.debug("Too many newsitems; unable to record %d", type);
        // Return a fake reference
        return mock;
    }

    Planet *pl = get_active_planet();
    Player *owner = (pl && pl->is_owned()) ? get_player(pl->get_owner()) : nullptr;

    NewsItem &ni = newsitems[newsitem_head++];
    new (&ni) NewsItem(s, p, type);
    ni.player_owned = owner ? owner->is_human() : false;
    return ni;
}

const NewsItem& ExodusState::get_news(int i) {
    if (i >= newsitem_head) {
        new (&newsitems[i]) NewsItem(-1, -1, NI_None);
    }

    return newsitems[i];
}

bool ExodusState::get_star_planet_idx(Planet* planet, int& _star_idx, int& _planet_idx) {
    for (PlanetIterator piter; !piter.complete(); ++piter) {
        Planet *p = piter.get();
        if (p == planet) {
            _star_idx = piter.get_star_idx();
            _planet_idx = piter.get_idx();
            return true;
        }
    }

    L.error("Unable to find requested planet at %x", planet);
    return false;
}

bool ExodusState::has_artificial_planet(int player_idx) {
    for (PlanetIterator piter(player_idx); !piter.complete(); ++piter) {
        if (piter->get_class() == Artificial) {
            return true;
        }
    }

    return false;
}

/*
 * Best avoided - inefficient and if we don't already know the star
 * when we need to, that's probably an architectural issue.
 */
Star* ExodusState::get_star_for_planet(Planet* p) {
    for (PlanetIterator piter; !piter.complete(); ++piter) {
        if (piter.get() == p) {
            return piter.get_star();
        }
    }

    L.error("Could not find star for planet %s", p->get_name());
    return nullptr;
}

bool ExodusState::planet_name_taken(const char* name) {
    for (PlanetIterator piter; !piter.complete(); ++piter) {
        if (!strncmp(name, piter->get_name(), PLANET_MAX_NAME)) {
            return true;
        }
    }

    return false;
}

void ExodusState::stabilise_disappeared_planet(Planet* planet) {
    for (int i = 0; i < N_PLAYERS; ++i) {
        Player *p = get_player(i);
        if (!p->is_human() && p->is_participating()) {
            PlayerLocation &loc = p->get_location();
            int tgt_s = loc.get_target();
            int tgt_p = loc.get_planet_target();
            if (tgt_s < 0 || tgt_p < 0) {
                continue;
            }
            Galaxy *gal = get_galaxy();
            if (planet == gal->get_stars()[tgt_s].get_planet(tgt_p)) {
                L.warn("Planet that %s was targeting is no longer there", p->get_full_name());
                /*
                 * Original doesn't really handle this case.
                 * Setting AI tactic back to the default seems reasonable.
                 */
                loc.unset_target();
                p->set_tactic(0);
            }
        }
    }
}

void ExodusState::cancel_worlds_under_construction(int player_idx) {
    Planet* p = ExodusState::get_planet_under_construction(player_idx);
    if (p) {
        // Wipe it
        new (p) Planet();
    }
}

void ExodusState::disable_easy_alliances() {
    for (int i = 0; i < N_PLAYERS; ++i) {
        Player *p = get_player(i);
        if (p) {
            p->perk_easy_alliances = false;
        }
    }
}

void ExodusState::prevent_attack(Planet* p) {
    int s_idx, p_idx;
    if (get_star_planet_idx(p, s_idx, p_idx)) {
        attack_preventions[s_idx] |= (1 << p_idx);
    }
}

bool ExodusState::attack_prevented(Planet* p) {
    int s_idx, p_idx;
    if (get_star_planet_idx(p, s_idx, p_idx)) {
        return attack_preventions[s_idx] & (1 << p_idx);
    }
    return false;
}

void ExodusState::clear_attack_preventions() {
    for (int i = 0; i < GALAXY_MAX_STARS; ++i) {
        attack_preventions[i] = 0;
    }
}

void ExodusState::prevent_bombing(Planet* p) {
    int s_idx, p_idx;
    if (get_star_planet_idx(p, s_idx, p_idx)) {
        bombing_preventions[s_idx] |= (1 << p_idx);
    }
}

bool ExodusState::bombing_prevented(Planet* p) {
    int s_idx, p_idx;
    if (get_star_planet_idx(p, s_idx, p_idx)) {
        return bombing_preventions[s_idx] & (1 << p_idx);
    }
    return false;
}

void ExodusState::clear_bombing_preventions() {
    for (int i = 0; i < GALAXY_MAX_STARS; ++i) {
        bombing_preventions[i] = 0;
    }
}

int ExodusState::planet_report_count() {
    return planet_reports_head;
}

PlanetReport& ExodusState::get_planet_report(int idx) {
    if (idx >= planet_report_count()) {
        L.error("Attempt to index invalid planet report");
        return planet_reports[0];
    }

    return planet_reports[idx];
}

void ExodusState::reset_planet_reports() {
    for (int i = 0; i < planet_reports_head; ++i) {
        planet_reports[i].reset();
    }

    planet_reports_head = 0;
}

void ExodusState::save_planet_report(PlanetReport& report) {
    /*
     * We used to omit reports with no information - however
     * this turns out to be a bit confusing.
     */
    /*
    if (report.empty()) {
        return;
    }
    */

    Planet *p = report.get_planet();
    L.debug("REPORT %s (%d/%d/%d/%d)", p->is_named() ? p->get_name() : "[UNNAMED]"
                                     , report.problems_critical
                                     , report.problems_major
                                     , report.problems_minor
                                     , report.good_news);

    if (planet_reports_head >= MAX_REPORTS) {
        L.error("Should not be possible to create this many reports");
        return;
    }

    for (int i = 0; i < planet_reports_head; ++i) {
        if (report > planet_reports[i]) {
            for (int j = planet_reports_head; j > i; --j) {
                planet_reports[j] = planet_reports[j-1];
            }
            planet_reports[i] = report;
            ++planet_reports_head;
            return;
        }
    }

    planet_reports[planet_reports_head++] = report;
}

void ExodusState::finalise_planet_reports() {
    for (int i = 0; i < planet_reports_head; ++i) {
        planet_reports[i].finalise();
    }
}

void ExodusState::register_species_hostility(Player *player, int hostile_to) {
    if (player->is_human()) {
        return;
    }

    int player_idx = get_player_idx(player);

    Race race = player->get_race();

    for (int i = 0; i < N_PLAYERS; ++i) {
        Player *p = get_player(i);

        if (p == player) {
            continue;
        }

        if (!p->is_participating()) {
            continue;
        }

        if ((p->get_race() == race) && p->perk_species_hostility_link) {
            unset_alliances(player_idx, i);
            p->set_hostile_to(hostile_to);
        }
    }
}

Planet* ExodusState::gift_planet_to(int player_idx) {
    Player *p = get_player(player_idx);

    if (!(p && p->is_participating())) {
        return nullptr;
    }

    Planet *pl = select_planet_for_cpu(true);

    if (pl) {
        pl->set_owner(player_idx, POCR_Gift);
        pl->prepare_for_cpu_lord();
        L.debug("Gifted planet %s to %s", pl->get_name(), p->get_name());
    } else {
        L.debug("Unable to find planet to gift to %s", p->get_name());
    }

    return pl;
}

Planet* ExodusState::gift_art_planet_to(int player_idx) {
    Player *p = get_player(player_idx);

    if (!(p && p->is_participating())) {
        return nullptr;
    }

    L.debug("Gifting artificial planet to %s", p->get_name());

    Planet *art_planet = get_planet_under_construction(player_idx);

    Star *s = nullptr;

    if (!art_planet) {
        for (StarIteratorRandom si; !si.complete(); ++si) {
            s = si.get();
            if (artificial_planet_viable(s) == APV_Yes) {
                if (construct_artificial_planet(s, player_idx, nullptr)) {
                    art_planet = get_planet_under_construction(player_idx);
                }
            }
        }
    }

    if (art_planet) {
        while (art_planet->advance_construction_phase()) {}
        L.debug("Gifted artificial planet %s to %s at %s", art_planet, p->get_name(), s->name);
    } else {
        L.debug("Could not gift artificial planet to %s", p->get_name());
    }

    return art_planet;
}

void ExodusState::planet_gift_event() {
    if (!FEATURE(EF_PLANET_GIFT_EVENTS)) {
        return;
    }

    L.debug("--- PLANET GIFTING ---");

    // Gift a planet to:
    // * The 3 strongest AI
    // * The AI with most money
    // * The AI with most planets

    uint32_t recipients = 0;

    int total_army[N_PLAYERS];

    for (int i = 0; i < N_PLAYERS; ++i) {
        total_army[i] = -1;

        Player *p = get_player(i);

        if (p->is_human()) {
            continue;
        }

        if (!p->is_participating()) {
            continue;
        }

        total_army[i] = get_total_army(i);
    }

    for (int j = 0; j < 3; ++j) {
        int max = -1;
        int idx = -1;
        for (int i = 0; i < N_PLAYERS; ++i) {
            if (total_army[i] > max) {
                max = total_army[i];
                idx = i;
            }
        }
        if (idx >= 0) {
            recipients = recipients | (1 << idx);
            total_army[idx] = -1;
        }
    }

    int max_mc = -1;
    int max_mc_idx = -1;

    for (int i = 0; i < N_PLAYERS; ++i) {
        Player *p = get_player(i);

        if (p->is_human()) {
            continue;
        }

        if (!p->is_participating()) {
            continue;
        }

        int mc = p->get_mc();

        if (mc > max_mc) {
            if (!(recipients & (1 << i))) {
                max_mc = mc;
                max_mc_idx = i;
            }
        }
    }

    if (max_mc_idx >= 0) {
        recipients = recipients | (1 << max_mc_idx);
    }

    int max_planets = -1;
    int max_planets_idx = -1;

    for (int i = 0; i < N_PLAYERS; ++i) {
        Player *p = get_player(i);

        if (p->is_human()) {
            continue;
        }

        if (!p->is_participating()) {
            continue;
        }

        int planets = get_n_planets(p);

        if (planets > max_planets) {
            if (!(recipients & (1 << i))) {
                max_planets = planets;
                max_planets_idx = i;
            }
        }
    }

    if (max_planets_idx >= 0) {
        recipients = recipients | (1 << max_planets_idx);
    }

    for (int i = 0; i < N_PLAYERS; ++i) {
        if (recipients & (1 << i)) {
            gift_planet_to(i);
        }
    }
}

void ExodusState::run_planet_gift_events() {
    EnemyStart start = get_enemy_start();

    if (start == ENEMY_Weak) {
        return;
    }

    int m = get_month();

    if (m == 60 || m == 120) {
        planet_gift_event();
    }

    // Perk gifts
    for (int i = 0; i < N_PLAYERS; ++i) {
        Player *p = get_player(i);

        if (!p->is_participating()) {
            continue;
        }

        if (p->is_human()) {
            continue;
        }

        if (p->perk_planet_every_n_months > 0) {
            if ((m % p->perk_planet_every_n_months) == 0) {
                gift_planet_to(i);
            }
        }

        if (p->perk_art_planet_at_month > 0) {
            if (p->perk_art_planet_at_month == m) {
                gift_art_planet_to(i);
            }
        }
    }
}

static const char* PLANET_NAMES_YOK[] = {
    "Y-Aru", "Y-Bre", "Y-Cro", "Y-Dra", "Y-Eri", "Y-Fen", "Y-Gis", "Y-Hla",
    "Y-Isk", "Y-Juy", "Y-Kan", "Y-Lor", "Y-Mun", "Y-Nas", "Y-Oro", "Y-Pry",
    "Y-Qam", "Y-Rrr", "Y-Sto", "Y-Tri", "Y-Unn", "Y-Vos", "Y-Wac", "Y-Xun",
    "Y-Yii", "Y-Zar"
};

static int N_PLANET_NAMES_YOK = 26;

static const char* PLANET_NAMES_URK[] = {
    "Colony 71", "Colony 13", "Colony 44", "Colony 42",
    "Colony 94", "Colony 23", "Colony 84", "Colony 56",
    "Colony 21", "Colony 18", "Colony 26", "Colony 39",
    "Colony 48", "Colony 51", "Colony 67", "Colony 77",
    "Colony 81", "Colony 98", "Colony 17", "Colony 24",
    "Colony 32", "Colony 45", "Colony 57", "Colony 66",
    "Colony 72", "Colony 83"
};

static int N_PLANET_NAMES_URK = 26;

const char* ExodusState::get_next_planet_name_for_race(Race race) {
    int& ctr = race_name_counters[(int)race];

    if (race == RACE_Yokon && ctr < N_PLANET_NAMES_YOK) {
        return PLANET_NAMES_YOK[ctr++];
    }

    if (race == RACE_Urkash && ctr < N_PLANET_NAMES_URK) {
        return PLANET_NAMES_URK[ctr++];
    }

    return nullptr;
}

/*
 * These two functions are the *only* places outside init
 * alliance_matrix is read or written.
 * We ensure a consistent order of access into the matrix,
 * such that [b][a] will always refer to [a][b] if b > a.
 */
uint8_t ExodusState::get_alliances(int a, int b) {
    if (a == b) {
        return 0;
    }
    if (b > a) {
        return alliance_matrix[a*N_PLAYERS + b];
    }
    return alliance_matrix[b*N_PLAYERS + a];
}
void ExodusState::set_alliances(int a, int b, uint8_t alliances) {
    Player *pa = get_player(a);
    Player *pb = get_player(b);

    if (b > a) {
        alliance_matrix[a*N_PLAYERS + b] = alliances;
    } else {
        alliance_matrix[b*N_PLAYERS + a] = alliances;
    }

    if (pa->is_human() && pb->perk_easy_alliances) {
        disable_easy_alliances();
    }
    if (pb->is_human() && pa->perk_easy_alliances) {
        disable_easy_alliances();
    }

    if (pa->is_human() || pb->is_human()) {
        if (has_all_alliances(a, b)) {
            achievement_manager.unlock(ACH_AllAlliances);
        }

        int human_idx = pa->is_human() ? a : b;

        if (count_alliances(human_idx) >= 5) {
            achievement_manager.unlock(ACH_ManyAllies);
        }
    }
}

void ExodusState::register_lord_discovery(int a, int b) {
    if (b > a) {
        lord_discovery_matrix[a*N_PLAYERS + b] = true;
    } else {
        lord_discovery_matrix[b*N_PLAYERS + a] = true;
    }
}
bool ExodusState::check_lord_discovery(int a, int b) {
    if (a == b) {
        return 1;
    }
    if (b > a) {
        return lord_discovery_matrix[a*N_PLAYERS + b];
    }
    return lord_discovery_matrix[b*N_PLAYERS + a];
}

void ExodusState::save(cJSON* j) const {
    SAVE_BOOL(j, first_city_done);
    SAVE_BOOL(j, first_spaceport_done);
    SAVE_ENUM(j, size);
    SAVE_NUM(j, n_human_players);
    SAVE_NUM(j, month);
    SAVE_NUM(j, active_player);
    SAVE_NUM(j, active_star);
    SAVE_NUM(j, active_planet);
    SAVE_ARRAY_OF_SAVEABLE(j, players);
    SAVE_ENUM(j, aim);
    SAVE_ENUM(j, enemy_start);
    SAVE_SAVEABLE(j, galaxy);
    SAVE_BOOL(j, galaxy_finalised);
    SAVE_BOOL(j, active_flytarget_is_guild);
    SAVE_ARRAY_OF_NUM(j, alliance_matrix);
    SAVE_ARRAY_OF_NUM(j, lord_discovery_matrix);
    SAVE_ARRAY_OF_NUM(j, alliance_requests);
    SAVE_NUM(j, newsitem_head);
    SAVE_ARRAY_OF_SAVEABLE(j, newsitems);
    SAVE_NUM(j, planet_reports_head);
    SAVE_ARRAY_OF_SAVEABLE(j, planet_reports);
    SAVE_ARRAY_OF_NUM(j, attack_preventions);
    SAVE_ARRAY_OF_NUM(j, bombing_preventions);
    SAVE_NUM(j, recommended_planet_star_idx);
    SAVE_NUM(j, recommended_planet_idx);
    SAVE_ARRAY_OF_NUM(j, race_name_counters);
}

void ExodusState::load(cJSON* j) {
    LOAD_BOOL(j, first_city_done);
    LOAD_BOOL(j, first_spaceport_done);
    LOAD_ENUM(j, size);
    LOAD_NUM(j, n_human_players);
    LOAD_NUM(j, month);
    LOAD_NUM(j, active_player);
    LOAD_NUM(j, active_star);
    LOAD_NUM(j, active_planet);
    LOAD_ARRAY_OF_SAVEABLE(j, players);
    LOAD_ENUM(j, aim);
    LOAD_ENUM(j, enemy_start);
    LOAD_SAVEABLE(j, galaxy);
    LOAD_BOOL(j, galaxy_finalised);
    LOAD_BOOL(j, active_flytarget_is_guild);
    LOAD_ARRAY_OF_NUM(j, alliance_matrix);
    LOAD_ARRAY_OF_NUM(j, lord_discovery_matrix);
    LOAD_ARRAY_OF_NUM(j, alliance_requests);
    LOAD_NUM(j, newsitem_head);
    LOAD_ARRAY_OF_SAVEABLE(j, newsitems);
    LOAD_NUM(j, planet_reports_head);
    LOAD_ARRAY_OF_SAVEABLE(j, planet_reports);
    LOAD_ARRAY_OF_NUM(j, attack_preventions);
    LOAD_ARRAY_OF_NUM(j, bombing_preventions);
    LOAD_NUM(j, recommended_planet_star_idx);
    LOAD_NUM(j, recommended_planet_idx);
    LOAD_ARRAY_OF_NUM(j, race_name_counters);
}
