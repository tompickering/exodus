#include "flytarget.h"

#include <cstring>

FlyTarget::FlyTarget(int _x, int _y, bool _is_star, const char* _name)
    : x(_x), y(_y), is_star(_is_star) {
    strncpy(name, _name, FT_MAX_NAME);
}
