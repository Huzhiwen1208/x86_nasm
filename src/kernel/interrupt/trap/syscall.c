#include "../../include/syscall.h"
#include "../../include/stdio.h"
#include "../../include/type.h"
#include "../../include/log.h"
#include "../../include/task.h"
#include "../../include/time.h"
#include "../../include/mutex.h"
#include "../../include/fs.h"
#include "../../include/memory.h"
#include "../../include/device.h"

/// @brief test syscall
/// @param arg1 
/// @param arg2 
/// @param arg3 
/// @return 
static u32 syscall_test(u32 arg1, u32 arg2, u32 arg3) {
    printf("syscall_test: arg: {%d, %d, %d}\n", arg1, arg2, arg3);
    return 0;
}

/// @brief trigger a task switch intensively
static void syscall_yield() {
    schedule();
}

/// @brief sleep specific ms until wakeup in clock interrupt
/// @param ms 
static void syscall_sleep(u32 ms) {
    PCB* current = get_current_task();

    sleep_enqueue(current, ms + get_time_ms());
    schedule();
}

/// @brief allocate one page for vaddr(user)
/// @param vaddr   
static void syscall_allocate(u32 vaddr) {
    allocate_page(vaddr);
}

static u32 syscall_fork() {
    pcb_fork();
}

static void syscall_exit(i32 exit_code) {
    pcb_exit(exit_code);
}

static i32 syscall_waitpid(u32 pid, i32* exit_code) {
    return pcb_waitpid(pid, exit_code);
}

/// @brief syscall general entry
/// @param syscall_num 
/// @param arg1 
/// @param arg2 
/// @param arg3 
/// @return 
u32 trap_handler(u32 syscall_num, u32 arg1, u32 arg2, u32 arg3) {
    switch (syscall_num) {
    case SYSCALL_TEST:
        return syscall_test(arg1, arg2, arg3);
    case SYSCALL_YIELD:
        syscall_yield();
        break;
    case SYSCALL_SLEEP:
        syscall_sleep(arg1);
        break;
    case SYSCALL_GETTIME_MS:
        get_time((time_val*)arg1);
        break;
    case SYSCALL_MUTEX_LOCK:
        mutex_lock();
        break;
    case SYSCALL_MUTEX_UNLOCK:
        mutex_unlock();
        break;
    case SYSCALL_SPIN_LOCK:
        break;
    case SYSCALL_SPIN_UNLOCK:
        break;
    case SYSCALL_WRITE:
    case SYSCALL_READ:
        write(arg1, (char*)arg2, arg3);
        break;
    case SYSCALL_ALLOCATE:
        syscall_allocate(arg1);
        break;
    case SYSCALL_GETPID:
        return get_current_task()->pid;
    case SYSCALL_GETPPID:
        return get_current_task()->parent_pid;
    case SYSCALL_FORK:
        return syscall_fork();
    case SYSCALL_EXIT:
        syscall_exit(arg1);
        break;
    case SYSCALL_WAITPID:
        return syscall_waitpid(arg1, (i32*)arg2);
    case SYSCALL_READ_MASTER:
        return device_read(1, (void*)arg1, arg2, arg3);
    case SYSCALL_READ_SLAVE:
        return device_read(2, (void*)arg1, arg2, arg3);
    case SYSCALL_WRITE_MASTER:
        return device_write(1, (void*)arg1, arg2, arg3);
    case SYSCALL_WRITE_SLAVE:
        return device_write(2, (void*)arg1, arg2, arg3);
    default:
        panic("Unknown syscall number: %d", syscall_num);
    }
    return 0;
}

/// @brief trigger a syscall
/// @param syscall_num 
/// @param arg1 
/// @param arg2 
/// @param arg3 
/// @return 
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