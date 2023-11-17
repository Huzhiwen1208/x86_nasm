#include "../user_lib/include/stdio.h"
#include "../user_lib/include/utils.h"
#include "../user_lib/include/syscall.h"

void user_thread() {
    print("entry user_thread\n");

    u32 pid = get_pid();
    u32 ppid = get_ppid();
    print("pid: %d, ppid: %d\n", pid, ppid);

    // if (fork() == 0) {
    //     u32 count = 0;
    //     print("child process\n");
    //     print("pid: %d, ppid: %d\n", get_pid(), get_ppid());
    // } else {
    //     u32 count = 0;
    //     print("parent process\n");
    //     print("pid: %d, ppid: %d\n", get_pid(), get_ppid());
    // }
    for (i32 i = 0;i < 5; i+=2) {
        sleep(1000);
        print("I: %d\n", i);
    }   

    suspend();
}

void user_thread1() {
    print("entry user_thread1\n");

    u32 pid = get_pid();
    u32 ppid = get_ppid();
    print("pid: %d, ppid: %d\n", pid, ppid);

    // if (fork() == 0) {
    //     u32 count = 0;
    //     print("child process\n");
    //     print("pid: %d, ppid: %d\n", get_pid(), get_ppid());
    // } else {
    //     u32 count = 0;
    //     print("parent process\n");
    //     print("pid: %d, ppid: %d\n", get_pid(), get_ppid());
    // }
    for (i32 i = 0;i < 5; i+=2) {
        sleep(1000);
        print("IIII: %d\n", i);
    }   

    suspend();
}