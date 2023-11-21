#include "../include/type.h"
#include "../include/log.h"
#include "../include/descriptor.h"
#include "../include/interrupt.h"
#include "../include/io.h"
#include "../include/stdio.h"
#include "../include/task.h"
#include "../include/stdio.h"
#include "../include/utils.h"

#define PIC_M_CTRL 0x20 // Main PIC control port
#define PIC_M_DATA 0x21 // Main PIC data port
#define PIC_S_CTRL 0xa0 // Slave PIC control port
#define PIC_S_DATA 0xa1 // Slave PIC data port
#define PIC_EOI 0x20    // End-of-interrupt command code

/// @brief entry table in asm, register to IDT
extern void* handler_entry_table[IDT_SIZE];
/// @brief all traps handler in asm, register to IDT[0x80]
extern void __all_traps();
/// @brief used in asm
void* handler_list[IDT_SIZE];

// methods
/// @brief default exception handler
/// @param vector
void exception_handler(i32 vector) {
    trace("Exception: %d has been invoked", vector);
    while (1);
}

/// @brief default outer interrupt handler
/// @param vector 
void outer_handler_default(int vector)
{
    trace("Outer interrupt: %d has been invoked", vector);
    send_eoi(vector);
}

/// @brief open interrupt for specific vector
///             0 refers to open, 1 refers to close
/// @param vector 
void set_interrupt_mask(i32 vector) {
    assert(vector >= 0x20 && vector < 0x30);
    vector -= 0x20;

    if (vector < 8) {
        u8 mask = readb(PIC_M_DATA);
        mask &= ~(1 << vector);
        writeb(PIC_M_DATA, mask);
    } else {
        u8 mask = readb(PIC_S_DATA);
        mask &= ~(1 << (vector - 8));
        writeb(PIC_S_DATA, mask);
    }
}

/// @brief close interrupt for specific vector
/// @param vector 
void clear_interrupt_mask(i32 vector) {
    assert(vector >= 0x20 && vector < 0x30);
    vector -= 0x20;

    if (vector < 8) {
        u8 mask = readb(PIC_M_DATA);
        mask |= (1 << vector);
        writeb(PIC_M_DATA, mask);
    } else {
        u8 mask = readb(PIC_S_DATA);
        mask |= (1 << (vector - 8));
        writeb(PIC_S_DATA, mask);
    }
}

/// @brief set interrupt handler for specific vector, override default handler
/// @param vector 
/// @param handler 
void set_interrupt_handler(i32 vector, void* handler) {
    handler_list[vector] = handler;
}

/// @brief Initialize Programmable Interrupt Controller
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

/// @brief PIC: notify CPU that interrupt has been handled
void send_eoi(int vector) {
    if (vector >= 0x20 && vector < 0x28) {
        writeb(PIC_M_CTRL, PIC_EOI);
    }

    if (vector >= 0x28 && vector < 0x30) {
        writeb(PIC_M_CTRL, PIC_EOI);
        writeb(PIC_S_CTRL, PIC_EOI);
    }
}


/// @brief IDT
struct interrupt_descriptor idt[IDT_SIZE];
/// @brief IDT pointer
struct descriptor_pointer idt_pointer;

void idt_init() {
    for (i32 i = 0; i < EXCEPTION_SIZE + OUTER_INTERRUPT_SIZE; i++) {
        void* interrupt_handler = handler_entry_table[i];
        idt[i].offset_low = (u32)interrupt_handler & 0xffff;
        idt[i].selector = 1 << 3;
        idt[i].reserved = 0;
        idt[i].type = 0b1110;
        idt[i].segment = 0;
        idt[i].DPL = 0;
        idt[i].present = 1;
        idt[i].offset_high = ((u32)interrupt_handler >> 16) & 0xffff;
    }
    // syscall idt
    idt[0x80].offset_low = (u32)__all_traps & 0xffff;
    idt[0x80].selector = 1 << 3;
    idt[0x80].reserved = 0;
    idt[0x80].type = 0b1110;
    idt[0x80].segment = 0;
    idt[0x80].DPL = 3;
    idt[0x80].present = 1;
    idt[0x80].offset_high = ((u32)__all_traps >> 16) & 0xffff;

    idt_pointer.limit = sizeof(struct interrupt_descriptor) * IDT_SIZE - 1;
    idt_pointer.base = (u32)idt;

    // exception interrupt handler
    for (i32 i = 0; i < EXCEPTION_SIZE; i++) {
        set_interrupt_handler(i, exception_handler);
    }

    // outeral interrupt handler
    for (i32 i = 0; i < OUTER_INTERRUPT_SIZE; i++) {
        set_interrupt_handler(i+EXCEPTION_SIZE, outer_handler_default);
    }

    asm volatile ("lidt %0" : : "m"(idt_pointer));
}

void interrupt_init() {
    info("IDT & PIC initializing...");
    pic_init();
    idt_init();
    page_fault_init();
    clock_init();
    info("IDT & PIC initialized successfully!");
}

/// @brief get interrupt status(IF)
/// @return status 0: close, 1: open
u8 get_interrupt_status() {
    asm volatile ("pushf");
    asm volatile ("pop %eax");
    asm volatile ("and $0x200, %eax");
    asm volatile ("shr $9, %eax");
}

/// @brief restore former interrupt status
/// @param status 
void restore_interrupt_status(u8 status) {
    if (status) {
        asm volatile ("sti");
    }else {
        asm volatile ("cli");
    }
}