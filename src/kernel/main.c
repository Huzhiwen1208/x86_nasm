#include "inf.h"

void kernel_init() {
    char os_init_message[] = "InfOS v0.0.1 Initializing";
    char* gpu = (char*)0xb8000;
    for (int i = 0; i < sizeof(os_init_message); i++) {
        gpu[i*2] = os_init_message[i];
    }
}