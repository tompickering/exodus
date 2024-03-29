#ifndef GUARD_MODE_BASE_H
#define GUARD_MODE_BASE_H

#include "exodus.h"
#include "platform.h"
#include "timer/timer.h"

#include "draw/draw.h"
#include "audio/audio.h"
#include "input/input.h"

#include "state/exodus_state.h"
#include "state/ephemeral_state.h"

#define MAX_STATIC_IDS 64

extern DRAWMANAGER draw_manager;
extern AUDIOMANAGER audio_manager;
extern INPUTMANAGER input_manager;

extern ExodusState exostate;
extern EphemeralState ephstate;

class ModeBase {
    public:
        ModeBase(const char*);
        ModeBase(const char*, bool);
        virtual ~ModeBase();
        virtual void enter();
        virtual void enter(unsigned int);
        virtual void exit();
        virtual ExodusMode update(float) = 0;
        virtual bool should_push_to_stack();
        bool draw_cursor;
        const char* get_name();
        const char *name;
    protected:
        SprID id(unsigned int);
        bool stackable;
    private:
        unsigned int id_max;
        void register_ids(unsigned int);
        void unregister_ids();
        SprID ids[MAX_STATIC_IDS];
};

#endif
