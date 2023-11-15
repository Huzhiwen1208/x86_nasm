#include "../../kernel/include/syscall.h"
#include "../../kernel/include/variable_args.h"
#include "../../kernel/include/constant.h"
#include "include/stdio.h"

i32 print(const char *fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    u32 len = vsprintf(buf, fmt, args);
    va_end(args);

    return (i32)syscall(SYSCALL_WRITE, STDOUT, (u32)buf, len);
}