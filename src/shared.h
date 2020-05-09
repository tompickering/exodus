#ifndef GUARD_SHARED_H
#define GUARD_SHARED_H

#include "logger.h"
#include "platform.h"

#include "exodus.h"

#define PROG_NAME "Exodus"

#define COL_TEXT {0xFF, 0xFF, 0xFF}
#define COL_TEXT2 {0xEE, 0xEE, 0xAA}
#define COL_BORDERS {0xA0, 0xA0, 0xA0}

// Mimics BBC BASIC's RND()
#define RND(X) (1 + (rand() % (X)))

extern EXODUS exodus;

extern const float UPSCALE_X;
extern const float UPSCALE_Y;

extern Log::Logger L;

typedef struct {
    int x;
    int y;
} MousePos;

#endif
