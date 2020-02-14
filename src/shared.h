#ifndef GUARD_SHARED_H
#define GUARD_SHARED_H

#include "logger.h"
#include "platform.h"

#include "exodus.h"

#define PROG_NAME "Exodus"

extern EXODUS exodus;

extern const float UPSCALE_X;
extern const float UPSCALE_Y;

extern Log::Logger L;

typedef struct {
    int x;
    int y;
} MousePos;

#endif
