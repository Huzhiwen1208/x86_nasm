#include "../user_lib/include/stdio.h"
#include "../user_lib/include/utils.h"
#include "../user_lib/include/syscall.h"

void user_thread() {
    print("entry user thread\n");
    u32 vaddr = 0x1000123;
    char* message = (char*)vaddr;
    *message = 'A';
    print("message: %c\n", *message);

    u32 pid = get_pid();
    u32 ppid = get_ppid();
    print("pid: %d, ppid: %d\n", pid, ppid);
    suspend();
}