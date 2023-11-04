#include "include/type.h"
#include "include/log.h"
#include "include/descriptor.h"
#include "include/interrupt.h"

// method
void exception_handler(i32 vector) {
    trace("Exception: %d has been invoked", vector);
    while (1);
}

// IDT
struct interrupt_descriptor idt[IDT_SIZE];
struct descriptor_pointer idt_pointer;

extern void* handler_entry_table[IDT_SIZE];
void* handler_list[IDT_SIZE];

void idt_init() {
    for (i32 i = 0; i < IDT_SIZE; i++) {
        void* trap_handler = handler_entry_table[i];
        idt[i].offset_low = (u32)trap_handler & 0xffff;
        idt[i].selector = 1 << 3;
        idt[i].reserved = 0;
        idt[i].type = 0b1110;
        idt[i].segment = 0;
        idt[i].DPL = 0;
        idt[i].present = 1;
        idt[i].offset_high = ((u32)trap_handler >> 16) & 0xffff;
    }
    idt_pointer.limit = sizeof(struct interrupt_descriptor) * IDT_SIZE - 1;
    idt_pointer.base = (u32)idt;

    for (i32 i = 0; i < EXCEPTION_SIZE; i++) {
        handler_list[i] = exception_handler;
    }

    asm volatile ("lidt %0" : : "m"(idt_pointer));
}