#ifndef GUARD_MODE_BASE_H
#define GUARD_MODE_BASE_H

#include "../exodus.h"
#include "../platform.h"
#include "../timer/timer.h"

class ModeBase {
    public:
        ModeBase(const char*);
        ModeBase(const char*, bool);
        virtual ~ModeBase();
        virtual void enter();
        virtual void exit();
        virtual ExodusMode update(float) = 0;
        bool draw_cursor;
        const char* get_name();
        const char *name;
};

#endif
