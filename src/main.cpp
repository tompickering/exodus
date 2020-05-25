#include "platform.h"
#include "exodus.h"

#include <ctime>
#include <cstdlib>

EXODUS exodus;

int main(int argc, char** argv) {
    srand((unsigned int)time(nullptr));
    return exodus.run(argc, argv);
}
