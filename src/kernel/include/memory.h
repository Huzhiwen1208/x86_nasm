#include "type.h"

#define PAGE_SIZE (1 << 12)
#define MAX_PAGES (8 << 12)

typedef struct address_range_descriptor {
    u64 base;
    u64 length;
    u32 type;
} ard;

typedef struct frame_allocator {
    /* 
        index refers to ppn
        0b000000xx:
            00: not in using & not valid
            01: not in using & valid
            10: in using & not valid
            11: in using & valid
    */
    u8 pages[MAX_PAGES]; 
    u32 total_pages;
    u32 free_pages;
} frame_allocator;


i32 is_valid(u32 ppn);
i32 is_in_using(u32 ppn);

void free_physical_page(u32 ppn);
u32 allocate_physical_page(); // return ppn
void memory_init(void* ards_cnt_address);

void show_physical_pages();