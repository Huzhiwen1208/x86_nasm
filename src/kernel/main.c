#include "include/inf.h"
#include "include/type.h"
#include "include/console.h"
#include "include/print.h"

void kernel_init() {
    console_init();
    debug("Hello, os kernel!");
}