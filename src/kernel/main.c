#include "include/log.h"
#include "include/type.h"

extern void console_init();
extern void gdt_init();
extern void interrupt_init();
extern void clock_init();
extern void mapping_init();
extern void task_init();
extern void task_test();
extern u32 syscall(u32 num,u32 arg1,u32 arg2,u32 arg3);

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
    task_init();
    task_test();
}