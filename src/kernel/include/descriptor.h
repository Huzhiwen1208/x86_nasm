#include "type.h"
#include "constant.h"

typedef struct descriptor_pointer {
    u16 limit;
    u32 base;
} _no_align;

typedef struct global_descriptor {
    u16 limit_low;
    u32 base_low: 24;
    u8 type: 4;
    u8 segment: 1;
    u8 privilege: 2;
    u8 present: 1;
    u8 limit_high: 4;
    u8 unused: 1;
    u8 long_mode: 1;
    u8 big: 1;
    u8 granularity: 1;
    u8 base_high;
} _no_align;

typedef struct global_descriptor global_descriptor;

global_descriptor* get_from_gdt(u16 index);

typedef struct interrupt_descriptor {
    u16 offset_low; // 0 - 15 low bit, offset in segment
    u16 selector;   // code selector
    u8 reserved;    // unused
    u8 type : 4;    // task gate/interrupt gate/trap gate -> 0x0110/0x1110/...
    u8 segment : 1; // 0: system segment, 1: code segment, should be 0
    u8 DPL : 2;     // DPL, should be 0
    u8 present : 1; // present in memory?
    u16 offset_high;    // 16-31 high bit offset in segment
} _no_align;

void idt_init();

typedef struct tss {
    u32 backlink; // segment selector of former task
    u32 esp0;     // stack pointer of ring0
    u32 ss0;      // stack segment selector of ring0
    u32 esp1;
    u32 ss1; 
    u32 esp2;
    u32 ss2; 
    u32 cr3;
    u32 eip;
    u32 flags;
    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u32 es;
    u32 cs;
    u32 ss;
    u32 ds;
    u32 fs;
    u32 gs;
    u32 ldtr;          // local descriptor table segment selector
    u16 trace : 1;     // if 1, will cause an interrupt when a task switch occurs
    u16 reversed : 15; // unused
    u16 iobase;
    u32 ssp;
} _no_align;
typedef struct tss tss;

void tss_init();


void gdt_tss_init();