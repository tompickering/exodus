#ifndef GUARD_PLATFORM_NULL_H
#define GUARD_PLATFORM_NULL_H

#include "platform.h"

class PlatformNull : public Platform {
    public:
        PlatformNull() : Platform() {}
        virtual bool init() override;
        virtual void poll() override;
        virtual bool shutdown() override;
};

#endif
