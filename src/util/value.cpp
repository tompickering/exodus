#include "value.h"

bool between(float val, float lower, float upper) {
    return (val >= lower && val <= upper);
}

bool in(float x, float y, float ax0, float ax1, float ay0, float ay1) {
    return between(x, ax0, ax1) && between(y, ay0, ay1);
}

bool in_area(float x, float y, float ax, float ay, float w, float h) {
    return in(x, y, ax, ax+w, ay, ay+h);
}

bool in_radius(float x, float y, float ax, float ay, float r) {
    float dx = x - ax;
    float dy = y - ay;
    return dx*dx + dy*dy <= r*r;
}