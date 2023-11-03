#include "include/inf.h"
#include "include/type.h"
#include "include/io.h"
#include "include/console.h"

void kernel_init() {
    console_init();
    char* message = "Hello, world!\n";
    for (int i = 0; i < 100; i++) {
        console_write(message, length(message));
        if (i == 99) {
            console_write("done\n", 5);
        }
    }
}