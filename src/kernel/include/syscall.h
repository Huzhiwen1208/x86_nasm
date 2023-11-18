#include "type.h"

#define SYSCALL_TEST 0
#define SYSCALL_YIELD 1
#define SYSCALL_SLEEP 2
#define SYSCALL_GETTIME_MS 3
#define SYSCALL_MUTEX_LOCK 4
#define SYSCALL_MUTEX_UNLOCK 5
#define SYSCALL_SPIN_LOCK 6
#define SYSCALL_SPIN_UNLOCK 7
#define SYSCALL_WRITE 8
#define SYSCALL_ALLOCATE 9
#define SYSCALL_GETPID 10
#define SYSCALL_GETPPID 11
#define SYSCALL_FORK 12
#define SYSCALL_EXIT 13

u32 syscall(u32 syscall_num, u32 arg1, u32 arg2, u32 arg3);