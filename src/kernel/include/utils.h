#ifndef __UTILS_H__
#define __UTILS_H__

#include "type.h"

// assert
void assertion_failure(char *exp, char *file, char *base, i32 line);

#define assert(exp) \
    if (exp)        \
        ;           \
    else            \
        assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)

size_t length(const char* str);
void concat(char* s1, char* s2);
i32 is_digit(char c);

void memcpy(void* dst, const void* src, size_t n);
void memfree(void* start, size_t n);

void suspend();

#endif