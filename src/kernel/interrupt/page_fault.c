#include "../include/type.h"
#include "../include/memory.h"
#include "../include/log.h"
#include "../include/utils.h"

void page_fault(i32 vector) {
    u32 vaddr = get_cr2();
    warn("Page fault at 0x%x", vaddr);
    allocate_page(vaddr);
}

void page_fault_init() {
    set_interrupt_handler(14, page_fault);
}