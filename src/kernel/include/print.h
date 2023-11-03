int printf(const char *fmt, ...);

int println(const char *fmt, ...);

void panic(const char *fmt, ...);

int sprintf(char *buf, const char *fmt, ...);

// assert
void assertion_failure(char *exp, char *file, char *base, int line);

#define assert(exp) \
    if (exp)        \
        ;           \
    else            \
        assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)

#define debug(fmt, ...) debug_info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)