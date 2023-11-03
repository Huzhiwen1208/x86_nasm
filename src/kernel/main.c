#include "include/inf.h"
#include "include/type.h"
#include "include/io.h"
#include "include/console.h"
#include "include/print.h"

void kernel_init() {
    console_init();
    char* message = "Hello, world!";
    for (int i = 0; i < 100; i++) {
       println("%s, %d", message, i);
        if (i == 99) {
            printf("Kernel init done.\n");
        }
    }
    assert(2 == 2);
}