#include "star.h"

#include <cstdlib>

#include "shared.h"

Star::Star(int _x, int _y, const char* _name) : FlyTarget(_x, _y, _name) {
    size = (StarSize)(rand() % (STAR_Huge + 1));
    L.debug("Star %s\t: %d %d %d", name, x, y, size);
}

StarSize Star::get_size() const {
    return size;
}
