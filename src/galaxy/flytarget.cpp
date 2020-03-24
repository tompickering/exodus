#include "flytarget.h"

#include <cstring>

FlyTarget::FlyTarget(int _x, int _y, const char* _name) : x(_x), y(_y) {
    strncpy(name, _name, FT_MAX_NAME);
}
