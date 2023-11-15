#include "../user_lib/stdio.h"
#include "../../kernel/include/memory.h"
#include "../../kernel/include/utils.h"

void user_thread() {
    // syscall(SYSCALL_SLEEP, 3000, 0, 0);
    print("user thread\n");
    // asm volatile ("sti");
    suspend();
}