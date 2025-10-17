#ifndef GUARD_PLATFORM_STEAM_H
#define GUARD_PLATFORM_STEAM_H

#ifdef STEAM

#include "platform.h"

#include "steam/steam_api.h"

#include "achievements/achievements.h"

class SteamInternals {
    public:
        SteamInternals();

        bool load_achievements();
        void unlock_achievement(Achievement);

        void store_stats();

    protected:
        ISteamUser *user;
        ISteamUserStats *user_stats;
};

class PlatformSteam : public Platform {
    public:
        PlatformSteam() : Platform() {}
        ~PlatformSteam();
        virtual bool init() override;
        virtual void poll() override;
        virtual bool shutdown() override;

        void unlock_achievement(Achievement);

    private:
        bool do_store_stats;
        SteamInternals *internals;
};

#endif

#endif
