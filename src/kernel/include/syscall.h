#include "type.h"

#define SYSCALL_TEST 0
#define SYSCALL_YIELD 1

u32 syscall(u32 syscall_num, u32 arg1, u32 arg2, u32 arg3);