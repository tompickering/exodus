#include "str.h"

#include <cstring>
#include <cctype>

#define CAP_MAX_LEN 512

void capitalise(char* str, int len) {
    for (int i = 0; i < len; ++i) {
        str[i] = toupper(str[i]);
    }
}

char _tmp_caps[CAP_MAX_LEN];

const char* tmp_caps(const char* str) {
    size_t i;
    for (i = 0; i < strnlen(str, CAP_MAX_LEN - 1); ++i) {
        if (str[i] == '\0') break;
        _tmp_caps[i] = toupper(str[i]);
    }
    _tmp_caps[i] = '\0';
    return (const char*)(&_tmp_caps);
}
