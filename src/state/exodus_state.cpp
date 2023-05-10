#include "exodus_state.h"

#include "exodus_features.h"

#include <cstdio>
#include <cstring>

#include "util/value.h"

#include "assetpaths.h"
#include "shared.h"

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

ExodusState::ExodusState() {
    galaxy_finalised = false;
    active_flytarget_is_guild = true;
    first_city_done = false;
    first_spaceport_done = false;
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

    for (i = 0; i < n_human_players; ++i) {
        memcpy(&players[i], &config.players[i], sizeof(Player));
    }

    // PLAYER INIT: Everyone (overridable)
    for (i = 0; i < N_PLAYERS; ++i) {
        players[i].mc = 0;

        players[i].starship.reset();
        players[i].starship.crew = 5;
        // SUGGEST: Perhaps we could start players with one bionic probe?

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

        // TODO: Set to false for CPU players in a
        // multiplayer game with no other races.
        players[i].exists = true;
        players[i].left_galaxy = false;
    }

    // PLAYER INIT: Human
    for (i = 0; i < n_human_players; ++i) {
        players[i].init_race(RACE_Human);
        players[i]._intro_seen = false;
        players[i].fleet_marker_idx = i;

        if (config.enemy_start == ENEMY_None) {
            players[i].mc = 1000;
            players[i].fleet.transporters = 50;
            players[i].fleet.warships = 5;
        }

        if (config.enemy_start == ENEMY_Weak) {
            players[i].mc = 300;
            players[i].fleet.transporters = 50;
            players[i].fleet.warships = 5;
        }

        if (config.enemy_start == ENEMY_Medium) {
            players[i].mc = 200;
            players[i].fleet.transporters = 35;
            players[i].fleet.warships = 5;
        }

        if (config.enemy_start == ENEMY_Strong) {
            players[i].mc = 50;
            players[i].fleet.transporters = 20;
            players[i].fleet.freight.minerals = 20;
        }
    }

    // PLAYER INIT: CPU
    for (; i < N_PLAYERS; ++i) {
        players[i].init_race((Race)(RACE_Human + RND(4)));
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
        snprintf(
            players[i].full_name,
            MAX_PLAYER_FULLNAME + 1,
            "%s %s",
            players[i].title,
            players[i].name);
        players[i].guild_title = GUILDTITLE_None;
        players[i].dead = false;
        players[i].reputation = 3;
        players[i].init_tax();
        players[i].set_tactic(0);
        players[i].nopirates = 0;
        set_random_hostility(players[i]);
        for (int j = 0; j < OFF_MAX; ++j) {
            players[i].set_officer((Officer)j, OFFQ_Poor);
        }
        players[i].infraction_mask = 0;

        for (int j = 0; j < TRACE_MAX; ++j) {
            players[i].trace[j] = 0;
        }

        for (int j = 0; j < N_MARKERS; ++j) {
            players[i].star_markers[j].reset();
        }
    }

    // TODO: Check whether any initial alliances are set up
    // TODO: Check all references to alliance matrix in orig code have been carried over
    for (int j = 0; j < N_PLAYERS; ++j) {
        for (int i = 0; i < N_PLAYERS; ++i) {
            alliance_matrix[j*N_PLAYERS + i] = 0;
        }
        alliance_requests[j] = 0;
    }

    aim = config.aim;
    enemy_start = config.enemy_start;
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

void ExodusState::init_cpu_lords() {
    Galaxy *gal = get_galaxy();
    int n_stars;
    Star *stars = gal->get_stars(n_stars);

    for (int i = n_human_players; i < N_PLAYERS; ++i) {
        // Assign initial planet to CPU lords. Orig: PROCstart_the_lords
        // We follow the original's logic of iterating over stars in the
        // same order for each lord, which affects the probability of
        // neighbouring lords in the same system.
        // N.B. Star indices are not correlated with their positions.
        bool initial_planet_selected = false;
        Star *chosen_star = nullptr;
        int chosen_planet_idx = 0;
        L.verb("Choosing initial planet for CPU lord %d", i);
        while(!initial_planet_selected) {
            int quality = 0;
            for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
                L.verb("Considering star %d", star_idx);
                Star *s = &stars[star_idx];
                for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                    Planet *p = s->get_planet(planet_idx);
                    if (p && p->exists() && !p->is_owned()) {
                        L.verb("Considering planet %d", planet_idx);
                        int planet_quality = 0;
                        if (p->get_class() == Forest)  planet_quality = 4;
                        if (p->get_class() == Desert)  planet_quality = 2;
                        if (p->get_class() == Volcano) planet_quality = 1;
                        if (p->get_class() == Rock)    planet_quality = 3;
                        if (p->get_class() == Ice)     planet_quality = 2;
                        if (p->get_class() == Terra)   planet_quality = 5;
                        if (planet_quality > quality && RND(7) != 1) {
                            L.verb("Checking existing ownership");
                            // TODO: Can we just check planet->is_owned() at this point?
                            bool ok = true;
                            for (int other_idx = n_human_players; other_idx < i; other_idx++) {
                                Player *other = &players[other_idx];
                                FlyTarget *other_ts = loc2tgt(other->get_location().get_target());
                                int other_tp = other->get_location().get_planet_target();
                                if (other_ts == s && other_tp == planet_idx) {
                                    ok = false;
                                    break;
                                }
                            }
                            if (ok) {
                                L.verb("Updated planet candidate!");
                                quality = planet_quality;
                                chosen_star = s;
                                chosen_planet_idx = planet_idx;
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
        }

        // We've settled on a planet - now claim it!
        Planet *chosen_planet = chosen_star->get_planet(chosen_planet_idx);
        chosen_planet->set_owner(i);
        chosen_planet->prepare_for_cpu_lord();

        if (players[i].mc >= 190) {
            players[i].mc -= 190;
        } else {
            players[i].mc = 0;
        }

        chosen_planet->set_name(chosen_planet->get_name_suggestion());

        // TODO: PROCstart_the_lords sets 'firstplanet' here
        // Check the meaning and a suitable representation

        players[i].get_location().set(get_random_star_idx());
        players[i].get_location().unset_target();
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

void ExodusState::advance_month() {
    ++month;
    newsitem_head = 0;
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

    Galaxy *gal = get_galaxy();
    int n_stars;
    Star *stars = gal->get_stars(n_stars);

    switch (aim) {
        case AIM_Might:
            {
                for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
                    Star *s = &stars[star_idx];
                    for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                        Planet *p = s->get_planet(planet_idx);
                        if (p && p->exists() && p->is_owned()) {
                            if (p->get_owner() != player_idx) {
                                return false;
                            }
                        }
                    }
                }
            }
            break;
        case AIM_Money:
            {
                // FIXME: We should really exclude army funding here
                if (get_total_net_income(player_idx) < 1000) {
                    return false;
                }
            }
            break;
        case AIM_Civilization:
            {
                // TODO: Actually let players know what the requirements are!

                if (!player->has_all_inventions()) {
                    return false;
                }

                bool artificial_owned = false;
                int n_planets = 0;

                for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
                    Star *s = &stars[star_idx];
                    for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                        Planet *p = s->get_planet(planet_idx);
                        if (p && p->exists() && p->is_owned()) {
                            if (p->get_owner() == player_idx) {
                                n_planets++;
                                if (p->get_class() == Artificial) {
                                    artificial_owned = true;
                                }
                            }
                        }
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

int ExodusState::get_random_star_idx() {
    Galaxy *gal = get_galaxy();
    int n_stars;
    gal->get_stars(n_stars);
    return rand() % n_stars;
}

int ExodusState::get_n_owned_planets() {
    return _get_n_owned_planets(true);
}

int ExodusState::get_n_unowned_planets() {
    return _get_n_owned_planets(false);
}

int ExodusState::_get_n_owned_planets(bool owned) {
    int n_planets = 0;
    Galaxy *gal = get_galaxy();
    int n_stars;
    Star *stars = gal->get_stars(n_stars);
    for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
        Star *s = &stars[star_idx];
        for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
            Planet *p = s->get_planet(planet_idx);
            if (p && p->exists() && (p->is_owned() == owned)) {
                ++n_planets;
            }
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
    Galaxy *gal = get_galaxy();
    int n_stars;
    Star *stars = gal->get_stars(n_stars);
    int count = 0;
    for (int i = 0; i < n_stars; ++i) {
        for (int j = 0; j < STAR_MAX_PLANETS; ++j) {
            Planet *p = stars[i].get_planet(j);
            if (p && p->exists()) {
                ++count;
            }
        }
    }
    return count;
}

int ExodusState::get_n_planets(Player* player) {
    Galaxy *gal = get_galaxy();
    int n_stars;
    Star *stars = gal->get_stars(n_stars);
    int count = 0;
    for (int i = 0; i < n_stars; ++i) {
        for (int j = 0; j < STAR_MAX_PLANETS; ++j) {
            Planet *p = stars[i].get_planet(j);
            if (p && p->exists() && p->is_owned() && get_player(p->get_owner()) == player) {
                ++count;
            }
        }
    }
    return count;
}

/*
 * TODO: This serves as firstplanet() from orig, but orig only resets
 * the flag to 0 in PROCcheck_alive - whereas we always return an
 * up-to-date value - which means our behaviour might * deviate during
 * month pass.
 */
bool ExodusState::owns_a_planet(Player* player) {
    Galaxy *gal = get_galaxy();
    int n_stars;
    Star *stars = gal->get_stars(n_stars);
    for (int i = 0; i < n_stars; ++i) {
        for (int j = 0; j < STAR_MAX_PLANETS; ++j) {
            Planet *p = stars[i].get_planet(j);
            if (p && p->exists() && p->is_owned() && get_player(p->get_owner()) == player) {
                return true;
            }
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

    Galaxy *gal = get_galaxy();
    int n_stars;
    Star *stars = gal->get_stars(n_stars);

    for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
        Star *s = &stars[star_idx];
        for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
            Planet *p = s->get_planet(planet_idx);
            if (!p || !p->exists()) {
                continue;
            }
            int this_quality = p->get_quality();
            for (int i = 0;  i < STAR_MAX_PLANETS; ++i) {
                if (i == planet_idx) {
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
                info.index = planet_idx;
            }
        }
    }
    return info;
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

bool ExodusState::is_allied(int a, int b) {
    return get_alliances(a, b) > 0;
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

int ExodusState::get_total_net_income(int player_idx) {
    int total = 0;
    Galaxy *gal = get_galaxy();
    int n_stars;
    Star *stars = gal->get_stars(n_stars);
    for (int star_idx = 0; star_idx < n_stars; ++star_idx) {
        Star *s = &stars[star_idx];
        for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
            Planet *p = s->get_planet(planet_idx);
            if (p && p->exists() && p->is_owned()) {
                if (p->get_owner() == player_idx) {
                    total += p->get_net_income();
                }
            }
        }
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
#if FEATURE_ALLIANCE_LIMIT
    return !(alliance_requests[other] & (1 << (int)t));
#else
    return true;
#endif
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
    p->init_alien_name();
    p->give_mc(200);
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
    int p = get_active_planet_idx();

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
    ni.player_owned = owner ? owner->is_human() : false;;
    return ni;
}

const NewsItem& ExodusState::get_news(int i) {
    if (i >= newsitem_head) {
        new (&newsitems[i]) NewsItem(-1, -1, NI_None);
    }

    return newsitems[i];
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
    if (b > a) {
        alliance_matrix[a*N_PLAYERS + b] = alliances;
    } else {
        alliance_matrix[b*N_PLAYERS + a] = alliances;
    }
}
