#include "type.h"

#define PAGE_SIZE (1 << 12)
#define MAX_PAGES (8 << 12)

typedef struct address_range_descriptor {
    u64 base;
    u64 length;
    u32 type;
} ard;

typedef struct page_table_entry {
    u8 present : 1;  // present in memory?
    u8 write : 1;    // 0: readonly, 1: rw
    u8 user : 1;     // U
    u8 pwt : 1;      // page write through?
    u8 pcd : 1;      // page cache disable?
    u8 accessed : 1; // A
    u8 dirty : 1;    // D
    u8 pat : 1;      // page attribute table, 0: 4K, 1: 4M
    u8 global : 1;   // G
    u8 ignored : 3;  // unused
    u32 index : 20;  // page index
} _no_align;

typedef struct page_table_entry page_table_entry;

u32 get_root_ppn();
void set_cr3(u32 pde); // page dir entry => root ppn

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
void mapping_init();


page_table_entry* get_root_page_table();
page_table_entry* get_second_page_table(u32 vaddr);