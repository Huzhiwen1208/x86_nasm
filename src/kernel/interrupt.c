#include "include/type.h"
#include "include/log.h"
#include "include/descriptor.h"
#include "include/interrupt.h"
#include "include/io.h"
#include "include/stdio.h"
#include "include/task.h"

#define PIC_M_CTRL 0x20 // Main PIC control port
#define PIC_M_DATA 0x21 // Main PIC data port
#define PIC_S_CTRL 0xa0 // Slave PIC control port
#define PIC_S_DATA 0xa1 // Slave PIC data port
#define PIC_EOI 0x20    // End-of-interrupt command code

// methods
// exception handler
void exception_handler(i32 vector) {
    trace("Exception: %d has been invoked", vector);
    while (1);
}

// outer interrupt handler
void outer_handler(int vector)
{
    trace("Outer interrupt: %d has been invoked", vector);
    send_eoi(vector);
    schedule();
}

// Initialize Programmable Interrupt Controller
void pic_init() {
    writeb(PIC_M_CTRL, 0b00010001);
    writeb(PIC_M_DATA, 0x20);
    writeb(PIC_M_DATA, 0b00000100);
    writeb(PIC_M_DATA, 0b00000001);

    writeb(PIC_S_CTRL, 0b00010001);
    writeb(PIC_S_DATA, 0x28);
    writeb(PIC_S_DATA, 2);
    writeb(PIC_S_DATA, 0b00000001);

    writeb(PIC_M_DATA, 0b11111110);
    writeb(PIC_S_DATA, 0b11111111);
}

// PIC: notify CPU that interrupt has been handled
void send_eoi(int vector) {
    if (vector >= 0x20 && vector < 0x28) {
        writeb(PIC_M_CTRL, PIC_EOI);
    }

    if (vector >= 0x28 && vector < 0x30) {
        writeb(PIC_M_CTRL, PIC_EOI);
        writeb(PIC_S_CTRL, PIC_EOI);
    }
}


// IDT
struct interrupt_descriptor idt[IDT_SIZE];
struct descriptor_pointer idt_pointer;

extern void* handler_entry_table[IDT_SIZE];
void* handler_list[IDT_SIZE];

void idt_init() {
    for (i32 i = 0; i < EXCEPTION_SIZE + OUTER_INTERRUPT_SIZE; i++) {
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

    for (i32 i = 0; i < OUTER_INTERRUPT_SIZE; i++) {
        handler_list[i + EXCEPTION_SIZE] = outer_handler;
    }

    asm volatile ("lidt %0" : : "m"(idt_pointer));
}

void interrupt_init() {
    pic_init();
    idt_init();
}