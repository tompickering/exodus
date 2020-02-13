#ifndef GUARD_STATE_BASE_H
#define GUARD_STATE_BASE_H

#include "../platform.h"
#include "../timer/timer.h"

class StateBase {
    public:
        StateBase(const char*);
        StateBase(const char*, bool);
        virtual ~StateBase();
        virtual void enter();
        virtual void exit();
        virtual void update(float) = 0;
        bool draw_cursor;
    protected:
        const char *name;
};

#endif