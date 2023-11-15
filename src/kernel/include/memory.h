#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "type.h"
#include "constant.h"

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
            00: not in using & belong to kernel
            01: not in using & not belong to kernel
            10: in using & belong to kernel
            11: in using & not belong to kernel
    */
    u8 pages[MAX_PAGES];
    u32 total_pages;
    u32 free_pages;
    u32 kernel_pages; // total pages for kernel
    u32 kernel_free_pages;
} frame_allocator;

i32 is_belong_kernel(u32 ppn);
i32 is_in_using(u32 ppn);
void free_physical_page(u32 ppn);
u32 allocate_physical_page(); // return ppn
page_table_entry* get_root_page_table();
page_table_entry* get_second_page_table(u32 vaddr);
u32 get_paddr_from_ppn(u32 ppn);
u32 get_ppn_from_paddr_floor(u32 paddr);
u32 get_ppn_from_paddr_ceil(u32 paddr);
page_table_entry* find_pte(u32 vaddr);
page_table_entry* find_pte_create(u32 vaddr);
u32 translate_vaddr(u32 vaddr); // return paddr
void disable_page();

void memory_init(void* ards_cnt_address);
void mapping_init();

void* buddy_alloc(u32 size);
void buddy_heap_init();

#endif