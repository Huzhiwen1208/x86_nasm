#include "type.h"

#define SYSCALL_TEST 0
#define SYSCALL_YIELD 1
#define SYSCALL_SLEEP 2
#define SYSCALL_GETTIME_MS 3
#define SYSCALL_MUTEX_LOCK 4
#define SYSCALL_MUTEX_UNLOCK 5

u32 syscall(u32 syscall_num, u32 arg1, u32 arg2, u32 arg3);