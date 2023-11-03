#include "include/type.h"
#include "include/console.h"
#include "include/stdio.h"
#include "include/log.h"
#include "include/gdt.h"

void kernel_main() {
    console_init();
    debug("GDT initializing...");
    gdt_init();
    debug("Hello, os kernel!");
}