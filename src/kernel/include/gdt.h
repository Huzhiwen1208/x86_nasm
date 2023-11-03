#include "type.h"

#define GDT_SIZE 8192

typedef struct global_descriptor_pointer {
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