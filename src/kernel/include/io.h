#include "type.h"
#include "variable_args.h"

// assert
void assertion_failure(char *exp, char *file, char *base, int line);

#define assert(exp) \
    if (exp)        \
        ;           \
    else            \
        assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)

// port I/O
void writeb(u16 port, u8 value); // write a byte to specified port
u8 readb(u16 port);              // read a byte from specified port
void writew(u16 port, u16 value); // write a word to specified port
u16 readw(u16 port);              // read a word from specified port

// std I/O
int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);
int printf(const char *fmt, ...);
