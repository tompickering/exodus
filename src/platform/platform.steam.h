#ifndef GUARD_PLATFORM_STEAM_H
#define GUARD_PLATFORM_STEAM_H

#include "platform.h"

#include "steam/steam_api.h"

class SteamInternals {
    public:
        SteamInternals();

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

    private:
        SteamInternals *internals;
};

#endif
