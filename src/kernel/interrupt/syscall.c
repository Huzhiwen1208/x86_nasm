#include "../include/syscall.h"
#include "../include/stdio.h"
#include "../include/type.h"
#include "../include/log.h"

static u32 syscall_test(u32 arg1, u32 arg2, u32 arg3) {
    printf("syscall_test: arg: {%d, %d, %d}\n", arg1, arg2, arg3);
    return 0;
}

static void syscall_yield() {
    schedule();
}

u32 trap_handler(u32 syscall_num, u32 arg1, u32 arg2, u32 arg3) {
    switch (syscall_num) {
    case SYSCALL_TEST:
        return syscall_test(arg1, arg2, arg3);
    case SYSCALL_YIELD:
        syscall_yield();
        break;
    default:
        panic("Unknown syscall number: %d", syscall_num);
        break;
    }
    return 0;
}


u32 syscall(u32 syscall_num, u32 arg1, u32 arg2, u32 arg3) {
    u32 result;
    asm volatile ("movl %0, %%eax" : : "m"(syscall_num));
    asm volatile ("movl %0, %%ebx" : : "m"(arg1));
    asm volatile ("movl %0, %%ecx" : : "m"(arg2));
    asm volatile ("movl %0, %%edx" : : "m"(arg3));
    asm volatile ("int $0x80");
    asm volatile ("movl %%eax, %0" : "=m"(result));
    return result;
}