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

void gdt_init();


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