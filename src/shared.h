#ifndef GUARD_SHARED_H
#define GUARD_SHARED_H

#include <cstdlib>
#include <cstdint>

#include "logger.h"
#include "platform.h"
#include "assetpaths.h"

#include "exodus.h"

#define PROG_NAME "Exodus"

#define STRINGIFY(x) #x
#define STR(x) STRINGIFY(x)

#define COL_TEXT {0xFF, 0xFF, 0xFF}
#define COL_TEXT2 {0xEE, 0xEE, 0xAA}
#define COL_TEXT3 {0xEE, 0xEE, 0x30}
#define COL_TEXT_SPEECH {0x22, 0xEE, 0xEE}
#define COL_TEXT_GREYED {0x50, 0x50, 0x50}
#define COL_TEXT_BAD {0xFF, 0x00, 0x00}
#define COL_BORDERS {0xA0, 0xA0, 0xA0}
#define COL_TOOL {0x00, 0x00, 0x00}
#define COL_TOOL_HL {0x00, 0x00, 0xFF}

#define BORDER 4

#define TEXT_Y_SEP 20

// Mimics BBC BASIC's RND()
#define RND(X) (1 + (rand() % (X)))

#define onein(X) ((rand() % (X)) == 0)

extern EXODUS exodus;

extern Log::Logger L;

typedef struct {
    int x;
    int y;
} MousePos;

#endif
