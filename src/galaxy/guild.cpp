#include "guild.h"

#include "shared.h"

Guild::Guild(int _x, int _y) : FlyTarget(_x, _y, (char*)"Space Guild Station") {
    L.debug("GUILD: %d %d", x, y);
}
