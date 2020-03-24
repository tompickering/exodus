#include "star.h"

#include <cstdlib>

#include "../shared.h"

Star::Star(int _x, int _y) : FlyTarget(_x, _y) {
    size = (StarSize)(rand() % (STAR_Huge + 1));
    L.debug("New star: %d %d %d", x, y, size);
}
