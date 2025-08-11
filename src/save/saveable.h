#ifndef GUARD_SAVEABLE_H
#define GUARD_SAVEABLE_H

#include <cstdio>
#include <cjson/cJSON.h>

#define SAVE_NUM(j, k) cJSON_AddNumberToObject(j, #k, k)
#define SAVE_BOOL(j, k) cJSON_AddNumberToObject(j, #k, (int)k)
#define SAVE_STR(j, k) cJSON_AddStringToObject(j, #k, k)
#define SAVE_ENUM(j, k) cJSON_AddNumberToObject(j, #k, (int)k)

#define SAVE_SAVEABLE(j, k) { \
    cJSON *o = cJSON_CreateObject(); \
    k.save(o); \
    cJSON_AddItemToObject(j, #k, o); \
}

#define SAVE_ARRAY_OF_NUM(j, k) { \
    cJSON* arr = cJSON_CreateArray(); \
    for (int i = 0; i < (int)(sizeof(k)/sizeof(*k)); ++i) { \
        cJSON *o = cJSON_CreateNumber(k[i]); \
        cJSON_AddItemToArray(arr, o); \
    } \
    cJSON_AddItemToObject(j, #k, arr); \
}

#define SAVE_ARRAY_OF_ENUM(j, k) { \
    cJSON* arr = cJSON_CreateArray(); \
    for (int i = 0; i < (int)(sizeof(k)/sizeof(*k)); ++i) { \
        cJSON *o = cJSON_CreateNumber((int)k[i]); \
        cJSON_AddItemToArray(arr, o); \
    } \
    cJSON_AddItemToObject(j, #k, arr); \
}

#define SAVE_ARRAY_OF_SAVEABLE(j, k) { \
    cJSON* arr = cJSON_CreateArray(); \
    for (int i = 0; i < (int)(sizeof(k)/sizeof(*k)); ++i) { \
        cJSON *o = cJSON_CreateObject(); \
        k[i].save(o); \
        cJSON_AddItemToArray(arr, o); \
    } \
    cJSON_AddItemToObject(j, #k, arr); \
}

#define LOAD_NUM(j, k) k = (decltype(k))(cJSON_GetObjectItemCaseSensitive(j, #k)->valueint)
#define LOAD_BOOL(j, k) k = (cJSON_GetObjectItemCaseSensitive(j, #k)->valueint != 0)
#define LOAD_STR(j, k) snprintf(k, sizeof(k), cJSON_GetObjectItemCaseSensitive(j, #k)->valuestring)
#define LOAD_ENUM(j, k) k = (decltype(k))(cJSON_GetObjectItemCaseSensitive(j, #k)->valueint)

#define LOAD_SAVEABLE(j, k) { \
    cJSON *o = cJSON_GetObjectItemCaseSensitive(j, #k); \
    k.load(o); \
}

#define LOAD_ARRAY_OF_NUM(j, k) { \
    cJSON* arr = cJSON_GetObjectItemCaseSensitive(j, #k); \
    for (int i = 0; i < cJSON_GetArraySize(arr); ++i) { \
        cJSON *o = cJSON_GetArrayItem(arr, i); \
        k[i] = (decltype(k[i]))(o->valueint); \
    } \
}

#define LOAD_ARRAY_OF_ENUM(j, k) { \
    cJSON* arr = cJSON_GetObjectItemCaseSensitive(j, #k); \
    for (int i = 0; i < cJSON_GetArraySize(arr); ++i) { \
        cJSON *o = cJSON_GetArrayItem(arr, i); \
        k[i] = (decltype(k[i]))(o->valueint); \
    } \
}

#define LOAD_ARRAY_OF_SAVEABLE(j, k) { \
    cJSON* arr = cJSON_GetObjectItemCaseSensitive(j, #k); \
    for (int i = 0; i < cJSON_GetArraySize(arr); ++i) { \
        cJSON *o = cJSON_GetArrayItem(arr, i); \
        k[i].load(o); \
    } \
}

class Saveable {
    virtual void save(cJSON*) const = 0;
    virtual void load(cJSON*) = 0;
};

#endif
