#include "include/type.h"
#include "include/console.h"
#include "include/stdio.h"
#include "include/log.h"
#include "include/descriptor.h"
#include "include/task.h"
#include "include/interrupt.h"

void thread_a() {
    for (i32 i = 0; i < 2000; i++) {
        printf("A");
        schedule();
    }
}

void thread_b() {
    for (i32 i = 0; i < 2000; i++) {
        printf("B");
        schedule();
    }
}

void thread_c() {
    for (i32 i = 0; i < 2000; i++) {
        printf("C");
        schedule();
    }
}

void kernel_main() {
    console_init();
    println_with_color(LIGHT_BLUE, "XiaoYan! XiaoYan! XiaoYan! XiaoYan!");
    info("GDT initializing...");
    gdt_init();
    trace("Hello, os kernel!");

    // pcb_manager_init();

    // create_task(thread_a, 0x100000);
    // create_task(thread_b, 0x200000);
    // create_task(thread_c, 0x300000);
    // schedule();

    interrupt_init();
    asm volatile("sti\n");

    for (i32 i = 0; i < 10000; i++) {
        println("Love you!!!!!!!!!!!!!");
        u32 cnt = 100000000;
        while(cnt--);
    }
}