#include "include/type.h"
#include "include/console.h"
#include "include/stdio.h"
#include "include/log.h"
#include "include/descriptor.h"
#include "include/task.h"
#include "include/interrupt.h"

void _ofp thread_a() {
    asm volatile("sti");
    for (;;) {
        printf("A");
    }
}

void _ofp thread_b() {
    asm volatile("sti");
    for (;;) {
        printf("B");
    }
}

void _ofp thread_c() {
    asm volatile("sti");
    for (;;) {
        printf("C");
    }
}

void kernel_main() {
    console_init();
    info("GDT initializing...");
    gdt_init();
    info("GDT initialized successfully!");
    info("IDT & PIC initializing...");
    interrupt_init();
    info("IDT & PIC initialized successfully!");

    i32 cnt = 0;
    while(1) {
        printf("A %d\n", cnt);
        cnt = cnt == 0 ? 1 : 0;
    }
    // pcb_manager_init();
    // create_task(thread_a, 0x100000);
    // create_task(thread_b, 0x200000);
    // create_task(thread_c, 0x300000);
    // asm volatile("sti");
}