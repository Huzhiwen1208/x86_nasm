#include "include/type.h"
#include "include/console.h"
#include "include/stdio.h"
#include "include/log.h"
#include "include/descriptor.h"
#include "include/task.h"
#include "include/interrupt.h"
#include "include/memory.h"

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
    info("Clock initializing...");
    clock_init();
    info("Clock initialized successfully!");
    asm volatile("cli");

    // pcb_manager_init();
    // create_task(thread_a, 0x100000);
    // create_task(thread_b, 0x200000);
    // create_task(thread_c, 0x300000);
}