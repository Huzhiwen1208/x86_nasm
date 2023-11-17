[bits 32]
section .text

extern handler_list
%macro INTERRUPT_HANDLER 2
interrupt_handler_%1:
%ifn %2
    push 0x88888888
%endif
    ; save context 
    push ds
    push es
    push fs
    push gs
    pushad

    push %1
    call [handler_list + %1 * 4]
    add esp, 4; pop %1

    ; restore context
    popad
    pop gs
    pop fs
    pop es
    pop ds
    add esp, 4; pop 0x88888888/error code

    iret
%endmacro

INTERRUPT_HANDLER 0x00, 0; 0x00 #DE Divide Error
INTERRUPT_HANDLER 0x01, 0; 0x01 #DB Debug Exception
INTERRUPT_HANDLER 0x02, 0; 0x02 NMI Interrupt, non maskable interrupt
INTERRUPT_HANDLER 0x03, 0; 0x03 #BP Breakpoint
INTERRUPT_HANDLER 0x04, 0; 0x04 #OF Overflow
INTERRUPT_HANDLER 0x05, 0; 0x05 #BR BOUND Range Exceeded
INTERRUPT_HANDLER 0x06, 0; 0x06 #UD Invalid Opcode (Undefined Opcode)
INTERRUPT_HANDLER 0x07, 0; 0x07 #NM Device Not Available (No Math Coprocessor)
INTERRUPT_HANDLER 0x08, 1; 0x08 #DF Double Fault
INTERRUPT_HANDLER 0x09, 0; 0x09 Coprocessor Segment Overrun (reserved)
INTERRUPT_HANDLER 0x0a, 1; 0x0A #TS Invalid TSS
INTERRUPT_HANDLER 0x0b, 1; 0x0B #NP Segment Not Present
INTERRUPT_HANDLER 0x0c, 1; 0x0C #SS Stack-Segment Fault
INTERRUPT_HANDLER 0x0d, 1; 0x0D #GP General Protection
INTERRUPT_HANDLER 0x0e, 1; 0x0E #PF Page Fault
INTERRUPT_HANDLER 0x0f, 0; 0x0F reserved
INTERRUPT_HANDLER 0x10, 0; 0x10 #MF x87 FPU Floating-Point Error (Math Fault)
INTERRUPT_HANDLER 0x11, 1; 0x11 #AC Alignment Check
INTERRUPT_HANDLER 0x12, 0; 0x12 #MC Machine Check
INTERRUPT_HANDLER 0x13, 0; 0x13 #XF SIMD Floating-Point Exception

INTERRUPT_HANDLER 0x14, 0; Virtualization Exception
INTERRUPT_HANDLER 0x15, 1; Control Protection Exception
INTERRUPT_HANDLER 0x16, 0; reserved
INTERRUPT_HANDLER 0x17, 0; reserved

INTERRUPT_HANDLER 0x18, 0; reserved
INTERRUPT_HANDLER 0x19, 0; reserved
INTERRUPT_HANDLER 0x1a, 0; reserved
INTERRUPT_HANDLER 0x1b, 0; reserved

INTERRUPT_HANDLER 0x1c, 0; reserved
INTERRUPT_HANDLER 0x1d, 0; reserved
INTERRUPT_HANDLER 0x1e, 0; reserved
INTERRUPT_HANDLER 0x1f, 0; reserved

INTERRUPT_HANDLER 0x20, 0; clock interrupt
INTERRUPT_HANDLER 0x21, 0; keyboard interrupt
INTERRUPT_HANDLER 0x22, 0; cascade
INTERRUPT_HANDLER 0x23, 0; com2
INTERRUPT_HANDLER 0x24, 0; com1
INTERRUPT_HANDLER 0x25, 0; sb16
INTERRUPT_HANDLER 0x26, 0; floppy
INTERRUPT_HANDLER 0x27, 0
INTERRUPT_HANDLER 0x28, 0; realtime clock
INTERRUPT_HANDLER 0x29, 0
INTERRUPT_HANDLER 0x2a, 0
INTERRUPT_HANDLER 0x2b, 0; nic
INTERRUPT_HANDLER 0x2c, 0
INTERRUPT_HANDLER 0x2d, 0
INTERRUPT_HANDLER 0x2e, 0; harddisk1
INTERRUPT_HANDLER 0x2f, 0; harddisk2

section .data
global handler_entry_table
handler_entry_table:
    dd interrupt_handler_0x00
    dd interrupt_handler_0x01
    dd interrupt_handler_0x02
    dd interrupt_handler_0x03
    dd interrupt_handler_0x04
    dd interrupt_handler_0x05
    dd interrupt_handler_0x06
    dd interrupt_handler_0x07
    dd interrupt_handler_0x08
    dd interrupt_handler_0x09
    dd interrupt_handler_0x0a
    dd interrupt_handler_0x0b
    dd interrupt_handler_0x0c
    dd interrupt_handler_0x0d
    dd interrupt_handler_0x0e
    dd interrupt_handler_0x0f
    dd interrupt_handler_0x10
    dd interrupt_handler_0x11
    dd interrupt_handler_0x12
    dd interrupt_handler_0x13
    dd interrupt_handler_0x14
    dd interrupt_handler_0x15
    dd interrupt_handler_0x16
    dd interrupt_handler_0x17
    dd interrupt_handler_0x18
    dd interrupt_handler_0x19
    dd interrupt_handler_0x1a
    dd interrupt_handler_0x1b
    dd interrupt_handler_0x1c
    dd interrupt_handler_0x1d
    dd interrupt_handler_0x1e
    dd interrupt_handler_0x1f
    dd interrupt_handler_0x20
    dd interrupt_handler_0x21
    dd interrupt_handler_0x22
    dd interrupt_handler_0x23
    dd interrupt_handler_0x24
    dd interrupt_handler_0x25
    dd interrupt_handler_0x26
    dd interrupt_handler_0x27
    dd interrupt_handler_0x28
    dd interrupt_handler_0x29
    dd interrupt_handler_0x2a
    dd interrupt_handler_0x2b
    dd interrupt_handler_0x2c
    dd interrupt_handler_0x2d
    dd interrupt_handler_0x2e
    dd interrupt_handler_0x2f