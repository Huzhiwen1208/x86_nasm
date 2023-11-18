#include "../user_lib/include/stdio.h"
#include "../user_lib/include/utils.h"
#include "../user_lib/include/syscall.h"

void user_thread() {
    print("entry user_thread\n");

    if (fork() == 0) {
        u32 count = 0;
        print("child process\n");
        u32 pid = get_pid();
        u32 ppid = get_ppid();
        print("pid: %d, ppid: %d\n", pid, ppid);
        for (int i = 0; i < 5; i++) {
            count++;
            print("child process: %d\n", count);
            sleep(10);
            if (count == 3) {
                print("child process exit\n");
                exit(0);
            }
        }
    } else {
        u32 count = 0;
        print("parent process\n");
        u32 pid = get_pid();
        u32 ppid = get_ppid();
        print("pid: %d, ppid: %d\n", pid, ppid);
        for (int i = 0; i < 5; i++) {
            count++;
            sleep(10);
            print("parent process: %d\n", count);
        }
    }
    
    // while(1) {
    //     u32 a = 3;
    //     u32 b = 4;
    //     u32 c = a + b;
    // }

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
    while(1) {
        u32 a = 3;
        u32 b = 4;
        u32 c = a + b;
    }

    suspend();
}