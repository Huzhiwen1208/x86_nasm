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

void allocate_one_page(u32 vaddr) {
    syscall(SYSCALL_ALLOCATE, vaddr, 0, 0);
}

u32 get_pid() {
    return syscall(SYSCALL_GETPID, 0, 0, 0);
}

u32 get_ppid() {
    return syscall(SYSCALL_GETPPID, 0, 0, 0);
}

u32 fork() {
    return syscall(SYSCALL_FORK, 0, 0, 0);
}

void exit(i32 exit_code) {
    syscall(SYSCALL_EXIT, exit_code, 0, 0);
}

i32 waitpid(i32 pid, i32* exit_code) {
    while(true) {
        i32 result_pid = syscall(SYSCALL_WAITPID, (u32)pid, (u32)exit_code, 0);
        switch (result_pid) {
        case -2:
            yield();
        default:
            return result_pid;
        }
    }
}

i32 read_disk_slave(void* buf, u32 count, u32 seek) {
    return syscall(SYSCALL_READ_SLAVE, (u32)buf, count, seek);
}

i32 read_disk_master(void* buf, u32 count, u32 seek) {
    return syscall(SYSCALL_READ_MASTER, (u32)buf, count, seek);
}

i32 write_disk_slave(void* buf, u32 count, u32 seek) {
    return syscall(SYSCALL_WRITE_SLAVE, (u32)buf, count, seek);
}

i32 write_disk_master(void* buf, u32 count, u32 seek) {
    return syscall(SYSCALL_WRITE_MASTER, (u32)buf, count, seek);
}

#endif