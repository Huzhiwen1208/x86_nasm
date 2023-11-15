#ifndef __USER_SYSCALL_H__
#define __USER_SYSCALL_H__

#include "../../../kernel/include/syscall.h"
#include "../../../kernel/include/type.h"
#include "../../../kernel/include/time.h"

// u32 syscall(u32 syscall_num, u32 arg1, u32 arg2, u32 arg3);

void sleep(u32 ms) {
    syscall(SYSCALL_SLEEP, ms, 0, 0);
}

void yield() {
    syscall(SYSCALL_YIELD, 0, 0, 0);
}

void get_time_val(time_val* tv) {
    syscall(SYSCALL_GETTIME_MS, (u32)tv, 0, 0);
}

#endif