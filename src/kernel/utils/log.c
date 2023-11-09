#include "../include/type.h"
#include "../include/variable_args.h"
#include "../include/stdio.h"
#include "../include/log.h"

void debug_info(const char *file, i32 line, const char *fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);

    println_with_color(CYAN, "\n[DEBUG] %s:%d %s", file, line, buf);
}

void trace_info(const char *file, i32 line, const char *fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);

    println_with_color(GRAY, "\n[TRACE] %s:%d %s", file, line, buf);
}

void info_info(const char *file, i32 line, const char *fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);

    println_with_color(GREEN, "\n[INFO] %s:%d %s", file, line, buf);
}

void warn_info(const char *file, i32 line, const char *fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);

    println_with_color(YELLOW, "\n[WARN] %s:%d %s", file, line, buf);
}

void error_info(const char *file, i32 line, const char *fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);

    println_with_color(RED, "\n[ERROR] %s:%d %s", file, line, buf);
}