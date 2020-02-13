#ifndef GUARD_SHARED_H
#define GUARD_SHARED_H

typedef struct {
    int x;
    int y;
} MousePos;

#include "logger.h"
#include "platform.h"

#include "draw/draw.h"
#include "audio/audio.h"
#include "input/input.h"

#define PROG_NAME "Exodus"

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
