#ifndef GUARD_EXODUS_SDL_H
#define GUARD_EXODUS_SDL_H

#include "exodus.h"

class ExodusSDL : public Exodus {
    public:
        ExodusSDL();
        virtual ~ExodusSDL() override;
    protected:
        virtual bool init() override;
        virtual void cleanup() override;
};

#endif
