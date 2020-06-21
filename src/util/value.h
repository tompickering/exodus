#ifndef GUARD_UTIL_VALUE_H
#define GUARD_UTIL_VALUE_H

bool between(float val, float lower, float upper);
bool in(float x, float y, float ax, float ay, float w, float h);
bool in_area(float x, float y, float ax, float ay, float w, float h);
bool in_radius(float x, float y, float ax, float ay, float r);

int min(int, int);
int max(int, int);
int clamp(int, int, int);

#endif
