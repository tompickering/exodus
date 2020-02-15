#ifndef GUARD_SHARED_H
#define GUARD_SHARED_H

typedef struct {
    int x;
    int y;
} MousePos;

#include "logger.h"
#include "platform.h"

#include "exodus.h"
#include "draw/draw.h"
#include "audio/audio.h"
#include "input/input.h"

#define PROG_NAME "Exodus"

// Facility to execute a single statement or block once only.
// Requires a unique name within the context - but this is checked at compile time.
#define ONCE(once_id) static bool once_id = true; if (once_id && !(once_id = false))

extern EXODUS exodus;

extern Log::Logger L;

// We need forward-declarations here so manager
// headers which #include this file don't see
// an as-yet undefined type.
class DRAWMANAGER;
class AUDIOMANAGER;
class INPUTMANAGER;

extern DRAWMANAGER draw_manager;
extern AUDIOMANAGER audio_manager;
extern INPUTMANAGER input_manager;

#endif
