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
    info("Memory page initializing...");
    mapping_init();
    info("Memory page initialized successfully!");

    pcb_manager_init();
    create_task(thread_a, get_paddr_from_ppn(allocate_physical_page_for_kernel()));
    create_task(thread_b, get_paddr_from_ppn(allocate_physical_page_for_kernel()));
    create_task(thread_c, get_paddr_from_ppn(allocate_physical_page_for_kernel()));
    asm volatile("sti");
}