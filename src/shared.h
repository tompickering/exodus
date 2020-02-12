#ifndef GUARD_SHARED_H
#define GUARD_SHARED_H

#include "logger.h"
#include "platform.h"

#define PROG_NAME "Exodus"

extern Log::Logger L;

typedef struct {
    int x;
    int y;
} MousePos;

#endif
