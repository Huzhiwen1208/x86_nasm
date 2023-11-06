#include "../include/memory.h"
#include "../include/stdio.h"
#include "../include/type.h"

frame_allocator FRAME_ALLOCATOR;

static void frame_allocator_empty_init() {
    FRAME_ALLOCATOR.total_pages = 0;
    FRAME_ALLOCATOR.free_pages = 0;
    for (i32 i = 0; i < MAX_PAGES; i++) {
        FRAME_ALLOCATOR.pages[i] = 0b00;
    }
}

i32 is_valid(u32 ppn) {
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

static void set_valid(u32 ppn) {
    if (!is_valid(ppn)) {
        FRAME_ALLOCATOR.pages[ppn] |= 0b01;
    }
}

static void set_invalid(u32 ppn) {
    if (is_valid(ppn)) {
        FRAME_ALLOCATOR.pages[ppn] &= 0b10;
    }
}

void free_physical_page(u32 ppn) {
    assert(is_valid(ppn));
    set_not_in_using(ppn);
    FRAME_ALLOCATOR.free_pages++;
}

u32 allocate_physical_page() {
    if (FRAME_ALLOCATOR.free_pages == 0) {
        panic("No free physical page!");
    }

    for (i32 i = 0; i < MAX_PAGES; i++) {
        if (is_valid(i) && !is_in_using(i)) {
            set_in_using(i);
            FRAME_ALLOCATOR.free_pages--;
            return i;
        }
    }
}

void show_physical_pages() {
    printf("---------------show physical pages---------------\n");
    printf("Total pages: %d, Free pages: %d\n", FRAME_ALLOCATOR.total_pages, FRAME_ALLOCATOR.free_pages);
    for (i32 i = 0; i < MAX_PAGES; i++) {
        if (is_valid(i)) {
            printf("[ppn: %d] %s\n", i, is_in_using(i) ? "in using" : "not in using");
        }
    }
    printf("---------------show physical page end---------------\n");
}

void memory_init(void* ards_cnt_address) {
    frame_allocator_empty_init();
    i32 ards_cnt = *((i32*)ards_cnt_address);
    ard* ards_base = ards_cnt_address + 4;
    for (i32 i = 0; i < ards_cnt; i++) {
        ard current_ard = ards_base[i];
        u32 frame_start = (current_ard.base + PAGE_SIZE - 1) >> 12;
        u32 frame_end = (current_ard.base + current_ard.length) >> 12;
        FRAME_ALLOCATOR.total_pages += frame_end - frame_start;
        if (current_ard.type == 1 && frame_start >= 256) {
            for (i32 j = frame_start; j < frame_end; j++) {
                set_valid(j);
                free_physical_page(j);
            }
        }
    }
    show_physical_pages();
}