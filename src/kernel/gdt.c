#include "include/descriptor.h"
#include "include/utils.h"

struct global_descriptor gdt[GDT_SIZE];
struct descriptor_pointer gdt_ptr;

void gdt_init() {
    
    // load current gdt of system to kernel gdt_ptr (from loader.asm)
    asm volatile("sgdt %0" : "=m" (gdt_ptr));
    
    // copy memory of current gdt to kernel

    memcpy(&gdt, (void *) gdt_ptr.base, gdt_ptr.limit + 1);

    // set limit and base of gdt_ptr
    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base = (u32) &gdt;

    // load kernel gdt_ptr to system
    asm volatile("lgdt %0" : : "m" (gdt_ptr));
}