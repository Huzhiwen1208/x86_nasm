#include "include/type.h"
#include "include/console.h"
#include "include/stdio.h"
#include "include/log.h"
#include "include/gdt.h"

void kernel_main() {
    console_init();
    info("GDT initializing...");
    gdt_init();
    trace("Hello, os kernel!");
}