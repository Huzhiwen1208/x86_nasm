#include "../include/memory.h"
#include "../include/stdio.h"
#include "../include/type.h"
#include "../include/utils.h"

/// @brief  the physical frame allocator
frame_allocator FRAME_ALLOCATOR;
/// @brief  the kernel root ppn
u32 KERNEL_ROOT_PPN;

// vaddr: [10][10][12] 
//      => [first page index][second page index][physical page offset]

/// @brief get the first page index of a virtual address
/// @param vaddr 
/// @return the first page index
u32 get_first_page_index(u32 vaddr) {
    return vaddr >> 22 & 0x3ff;
}

u32 get_second_page_index(u32 vaddr) {
    return vaddr >> 12 & 0x3ff;
}

u32 get_physical_page_offset(u32 vaddr) {
    return vaddr & 0xfff;
}

/// @brief get root page table paddr, which is the root ppn << 12
/// @return root page table paddr
page_table_entry* get_root_page_table() {
    return (page_table_entry*)(get_root_ppn() << 12);
}

/// @brief get second page table paddr by vaddr
/// @param vaddr 
/// @return second page table start paddr
page_table_entry* get_second_page_table(u32 vaddr) {
    page_table_entry* root_page_table = get_root_page_table();
    u32 first_page_index = get_first_page_index(vaddr);
    page_table_entry* pte = root_page_table + first_page_index;
    if (pte->present == 0) {
        panic("Page table not present!");
    }
    return (page_table_entry*)(pte->index << 12);
}

/// @brief get ppn of vaadr
/// @param vaddr 
/// @return ppn
u32 get_physical_page_number(u32 vaddr) {
    page_table_entry* second_page_table = get_second_page_table(vaddr);
    u32 second_page_index = get_second_page_index(vaddr);
    page_table_entry* pte = second_page_table + second_page_index;

    return pte->index;
}

/// @brief get paddr of vaddr
/// @param vaddr 
/// @return paddr
u32 get_physical_address(u32 vaddr) {
    u32 ppn = get_physical_page_number(vaddr);
    u32 offset = get_physical_page_offset(vaddr);

    return (ppn << 12) + offset;
}

/// @brief get paddr by ppn
/// @param ppn 
/// @return paddr
u32 get_paddr_from_ppn(u32 ppn) {
    return ppn << 12;
}

u32 get_ppn_from_paddr_floor(u32 paddr) {
    return paddr >> 12;
}

u32 get_ppn_from_paddr_ceil(u32 paddr) {
    return paddr >> 12;
}

/// @brief initialize physical frame allocator
///         the previous 4M(1024 frams) is for kernel
static void frame_allocator_empty_init() {
    FRAME_ALLOCATOR.total_pages = 0;
    FRAME_ALLOCATOR.free_pages = 0;
    FRAME_ALLOCATOR.kernel_pages = 1024;
    FRAME_ALLOCATOR.kernel_free_pages = 0;
    for (i32 i = 0; i < MAX_PAGES; i++) {
        FRAME_ALLOCATOR.pages[i] = 0b00; // not in using & not belong to kernel
        if (i < 1024) {
            FRAME_ALLOCATOR.pages[i] = 0b01; // not in using & belong to kernel
        }
    }
}

i32 is_belong_kernel(u32 ppn) {
    return FRAME_ALLOCATOR.pages[ppn] & 0b01;
}

i32 is_in_using(u32 ppn) {
    return FRAME_ALLOCATOR.pages[ppn] & 0b10;
}

static void set_not_in_using(u32 ppn) {
    if (is_in_using(ppn)) {
        FRAME_ALLOCATOR.pages[ppn] &= 0b01;
    }
}

static void set_in_using(u32 ppn) {
    if (!is_in_using(ppn)) {
        FRAME_ALLOCATOR.pages[ppn] |= 0b10;
    }
}

/// @brief free a physical page by ppn, but not clear the content
/// @param ppn 
void free_physical_page(u32 ppn) {
    set_not_in_using(ppn);
    FRAME_ALLOCATOR.free_pages++;
    if (is_belong_kernel(ppn)) {
        FRAME_ALLOCATOR.kernel_free_pages++;
    }
}

/// @brief allocate a physical page from 4M - 32M, for user
/// @return ppn
u32 allocate_physical_page() {
    if (FRAME_ALLOCATOR.free_pages == 0) {
        panic("No free physical page!");
    }

    for (i32 i = 0; i < MAX_PAGES; i++) {
        if (!is_belong_kernel(i) && !is_in_using(i)) {
            set_in_using(i);
            FRAME_ALLOCATOR.free_pages--;
            return i;
        }
    }
}

/// @brief allocate a physical page from 1M - 4M, for kernel
/// @return ppn
u32 allocate_physical_page_for_kernel() {
    if (FRAME_ALLOCATOR.free_pages == 0) {
        panic("No free physical page!");
    }

    for (i32 i = 0; i < MAX_PAGES; i++) {
        if (is_belong_kernel(i) && !is_in_using(i)) {
            set_in_using(i);
            FRAME_ALLOCATOR.free_pages--;
            FRAME_ALLOCATOR.kernel_free_pages--;
            return i;
        }
    }
}

static void show_physical_pages() {
    printf("---------------show physical pages---------------\n");
    printf("Total pages: %d, Free pages: %d\n", FRAME_ALLOCATOR.total_pages, FRAME_ALLOCATOR.free_pages);
    for (i32 i = 0; i < MAX_PAGES; i++) {
        if (is_belong_kernel(i)) {
            printf("[ppn: %d] %s\n", i, is_in_using(i) ? "in using" : "not in using");
        }
    }
    printf("---------------show physical page end---------------\n");
}

u32 get_root_ppn() {
    return KERNEL_ROOT_PPN;
}

/// @brief set root ppn to cr3
/// @param pde 
void set_cr3(u32 pde) {
    // pde is root ppn
    assert(pde % PAGE_SIZE == 0);
    asm volatile ("movl %%eax, %%cr3" :: "a"(pde));
}

u32 get_cr2() {
    asm volatile ("movl %cr2, %eax");
}

static void enable_page() {
    asm volatile ("movl %cr0, %eax");
    asm volatile ("orl $0x80000000, %eax");
    asm volatile ("movl %eax, %cr0");
}

void disable_page() {
    asm volatile ("movl %cr0, %eax");
    asm volatile ("andl $0x7fffffff, %eax");
    asm volatile ("movl %eax, %cr0");
}

/// @brief default initialize a page table entry
/// @param pte 
/// @param index 
static void pte_init(page_table_entry* pte, u32 index) {
    memfree((void*)pte, sizeof(page_table_entry));

    pte->present = 1;
    pte->write = 1;
    pte->user = 1;

    pte->index = index;
}

page_table_entry* find_pte(u32 vaddr) {
    page_table_entry* pte = get_root_page_table();
    u32 first_page_index = get_first_page_index(vaddr);
    pte += first_page_index;
    if (pte->present == 0) {
        return NULL;
    }

    pte = (page_table_entry*)(pte->index << 12); // second page table
    u32 second_page_index = get_second_page_index(vaddr);
    pte += second_page_index;
    if (pte->present == 0) {
        return NULL;
    }

    return pte;
}

page_table_entry* find_pte_create(u32 vaddr) {
    page_table_entry* pte = get_root_page_table();
    u32 first_page_index = get_first_page_index(vaddr);
    pte += first_page_index;
    if (pte->present == 0) {
        u32 second_page_table_idx = allocate_physical_page();
        pte_init(pte, second_page_table_idx);
    }

    pte = (page_table_entry*)(pte->index << 12); // second page table
    u32 second_page_index = get_second_page_index(vaddr);
    pte += second_page_index;
    if (pte->present == 0) {
        u32 physical_page = allocate_physical_page();
        pte_init(pte, physical_page);
    }

    return pte;
}

u32 translate_vaddr(u32 vaddr) {
    page_table_entry* pte = find_pte(vaddr);
    if (pte == NULL) {
        return NULL;
    }

    u32 ppn = pte->index;
    u32 offset = get_physical_page_offset(vaddr);
    return (ppn << 12) + offset;
}

void allocate_page(u32 vaddr) {
    disable_page();
    find_pte_create(vaddr);
    enable_page();
}

/// @brief initialize page mapping to MMU and enable page
void mapping_init() {
    KERNEL_ROOT_PPN = allocate_physical_page_for_kernel();
    page_table_entry* root_ppn = (page_table_entry*)(KERNEL_ROOT_PPN << 12);
    memfree((void*)root_ppn, PAGE_SIZE);

    u32 kernel_page_table = allocate_physical_page_for_kernel();
    pte_init(root_ppn, kernel_page_table);

    page_table_entry* kernel_page_entry = (page_table_entry*)(kernel_page_table << 12);
    memfree((void*)kernel_page_entry, PAGE_SIZE);

    for (i32 i = 0; i < 1024; i++) {
        if (i == 0) continue; // ensure 0x0 is null pointer, but we can't access 0x0 ~ 0xfff yet(first page)

        pte_init(kernel_page_entry + i, i);
        if (i < FRAME_ALLOCATOR.kernel_pages - FRAME_ALLOCATOR.kernel_free_pages) 
            set_in_using(i);
    }

    set_cr3(KERNEL_ROOT_PPN << 12);
    enable_page();
}

/// @brief get free pages and total pages from ards, 
///         we cant use page now because we haven't enable page
/// @param ards_cnt_address 
void memory_init(void* ards_cnt_address) {
    frame_allocator_empty_init();
    i32 ards_cnt = *((i32*)ards_cnt_address);
    ard* ards_base = ards_cnt_address + 4;
    for (i32 i = 0; i < ards_cnt; i++) {
        ard current_ard = ards_base[i];
        u32 frame_start = (current_ard.base + PAGE_SIZE - 1) >> 12;
        u32 frame_end = (current_ard.base + current_ard.length) >> 12;
        FRAME_ALLOCATOR.total_pages += frame_end - frame_start;
        if (current_ard.type == 1 && frame_start >= 256) { // manage 1M - 32M
            for (i32 j = frame_start; j < frame_end; j++) {
                free_physical_page(j);
            }
        }
    }

    // show_physical_pages();
}