#include "include/log.h"
#include "include/type.h"
#include "include/time.h"

extern void console_init();
extern void gdt_tss_init();
extern void interrupt_init();
extern void clock_init();
extern void mapping_init();
extern void task_init();
extern void task_test();
extern void mutex_init();
extern void spin_lock_init();
void keyboard_init();

void kernel_main() {
    console_init();
    info("GDT & TSS initializing...");
    gdt_tss_init();
    info("GDT & TSS initialized successfully!");
    info("IDT & PIC initializing...");
    interrupt_init();
    info("IDT & PIC initialized successfully!");
    info("Clock initializing...");
    clock_init();
    info("Clock initialized successfully!");
    info("Memory page initializing...");
    mapping_init();
    info("Memory page initialized successfully!");

    mutex_init();
    spin_lock_init();
    keyboard_init();
    task_init();
    task_test();
}