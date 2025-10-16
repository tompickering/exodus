#ifndef GUARD_PLATFORM_STEAM_H
#define GUARD_PLATFORM_STEAM_H

#include "platform.h"

class PlatformSteam : public Platform {
    public:
        PlatformSteam() : Platform() {}
        virtual bool init() override;
        virtual void poll() override;
        virtual bool shutdown() override;
};

#endif
