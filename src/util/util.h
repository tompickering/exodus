#ifndef GUARD_UTIL_UTIL_H
#define GUARD_UTIL_UTIL_H

#if defined(__cplusplus) && __cplusplus >= 201103L
#define NORETURN [[noreturn]]
#else
#define NORETURN __attribute__((noreturn))
#endif

#endif
