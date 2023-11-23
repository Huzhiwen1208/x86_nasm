#include "../include/memory.h"
#include "../include/stdio.h"
#include "../include/type.h"
#include "../include/utils.h"
#include "../include/constant.h"

typedef struct heap_block {
    u32 base;
    u32 size;
    bool used;
    bool allocated;
    i32 next;
} heap_block;

typedef struct buddy_heap {
    u32 total_size;
    heap_block free_list[MAX_FREE_BLOCK];
} buddy_heap;

static buddy_heap heap_allocator;

static i32 new_heap_block() {
    for (int i = 0; i < MAX_FREE_BLOCK; i++) {
        if (!heap_allocator.free_list[i].used) {
            heap_allocator.free_list[i].used = true;
            return i;
        }
    }
    return -1;
}

static void free_heap_block(i32 idx) {
    heap_allocator.free_list[idx].used = false;
}

static void make_heap_block(heap_block* block, i32 index) {
    heap_allocator.free_list[index].base = block->base;
    heap_allocator.free_list[index].size = block->size;
    heap_allocator.free_list[index].used = block->used;
    heap_allocator.free_list[index].next = block->next;
}

void buddy_heap_init() {
    u32 base = get_paddr_from_ppn(
        allocate_physical_page_for_kernel()
    );
    heap_allocator.total_size = PAGE_SIZE;
    i32 block_idx = new_heap_block();
    heap_block* block = &heap_allocator.free_list[block_idx];
    block->base = base;
    block->size = 0;
    block->next = 1;

    block_idx = new_heap_block();
    block = &heap_allocator.free_list[block_idx];
    block->base = base;
    block->size = PAGE_SIZE;
    block->next = -1;
}

void* buddy_alloc(u32 size) {
    if (size == 0) {
        return NULL;
    }
    if (size > heap_allocator.total_size) {
        return NULL;
    }

    if (heap_allocator.free_list[0].next == -1) {
        return NULL;
    }

    i32 p = heap_allocator.free_list[0].next; // head node
    i32 pre = 0;
    while (p != -1) {
        if (!heap_allocator.free_list[p].allocated && 
            heap_allocator.free_list[p].size >= size ) {
            while(heap_allocator.free_list[p].size >= size*2) {
                i32 block1_idx = new_heap_block();
                i32 block2_idx = new_heap_block();
                heap_block* block1 = &heap_allocator.free_list[block1_idx];
                heap_block* block2 = &heap_allocator.free_list[block2_idx];

                block1->base = heap_allocator.free_list[p].base;
                block1->size = heap_allocator.free_list[p].size / 2;
                block1->next = block2_idx;

                block2->base = heap_allocator.free_list[p].base + block1->size;
                block2->size = heap_allocator.free_list[p].size / 2;
                block2->next = heap_allocator.free_list[p].next;

                free_heap_block(p);
                heap_allocator.free_list[pre].next = block1_idx;

                p = block1_idx;
            }
            heap_allocator.free_list[p].allocated = true;
            return (void*)heap_allocator.free_list[p].base;
        }
        pre = p;
        p = heap_allocator.free_list[p].next;
    }
    // TODO: allocate a new page
    panic("buddy_alloc: no enough memory\n");
    return NULL;
}

void buddy_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    u32 base = (u32)ptr;
    i32 p = heap_allocator.free_list[0].next;
    i32 pre = 0;
    while (p != -1) {
        if (heap_allocator.free_list[p].base == base) {
            u32 len = heap_allocator.free_list[p].size;
            len <<= 1;
            // buddy is on the left
            if ((base - heap_allocator.free_list[0].base) % len != 0)  p = pre;

            i32 right = heap_allocator.free_list[p].next;
            while (right != -1
                && !heap_allocator.free_list[right].allocated 
                && heap_allocator.free_list[right].size == heap_allocator.free_list[p].size
            ) {
                heap_allocator.free_list[p].size <<= 1;
                heap_allocator.free_list[p].next = heap_allocator.free_list[right].next;
                free_heap_block(right);
                right = heap_allocator.free_list[p].next;
            }
            heap_allocator.free_list[p].allocated = false;
            return;
        }
        pre = p;
        p = heap_allocator.free_list[p].next;
    }

    ptr = NULL;
    return;
}