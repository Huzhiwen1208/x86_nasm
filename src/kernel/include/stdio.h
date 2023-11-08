#include "type.h"

i32 printf(const char *fmt, ...);

i32 println(const char *fmt, ...);

void panic(const char *fmt, ...);

i32 sprintf(char *buf, const char *fmt, ...);

i32 print_with_color(u8 color, const char *fmt, ...);

// assert
void assertion_failure(char *exp, char *file, char *base, i32 line);

#define assert(exp) \
    if (exp)        \
        ;           \
    else            \
        assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)