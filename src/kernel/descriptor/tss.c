#include "../include/descriptor.h"
#include "../include/constant.h"
#include "../include/memory.h"

tss tss_entry;

/// @brief Initialize TSS, UserCode and UserData
void tss_init() {
    // construct tss_entry
    memfree((void *) &tss_entry, sizeof(tss));  // clear
    tss_entry.ss0 = KERNEL_DATA_SELECTOR;
    tss_entry.iobase = sizeof(tss);
    tss_entry.esp0 = 0x107000;

    global_descriptor* descriptor = get_from_gdt(KERNEL_TSS_SELECTOR >> 3);
    descriptor->base_low = ((u32)&tss_entry) & 0xffffff; // 24 bits!!!! 0xffff -> 0xffffff, fix error
    descriptor->base_high = (((u32)&tss_entry) >> 24 ) & 0xff;
    descriptor->limit_low = (sizeof(tss) - 1 ) & 0xffff;
    descriptor->limit_high = ((sizeof(tss) - 1 ) >> 16 ) & 0xf;
    descriptor->segment = 0;  // system segment
    descriptor->granularity = 0; // byte
    descriptor->big = 0;         
    descriptor->long_mode = 0;   // 32 bits
    descriptor->present = 1;     // present in memory
    descriptor->privilege = 0;         // DPL = 0
    descriptor->type = 0b1001;   // 0b1001: TSS(Avaliable 32-bit TSS)
    asm volatile("ltr %%ax\n" : : "a" (KERNEL_TSS_SELECTOR)); // load tss

    // construct user code and user data
    global_descriptor* user_code = get_from_gdt(USER_CODE_SELECTOR >> 3);
    user_code->base_low = 0;
    user_code->base_high = 0;
    user_code->limit_low = 0xffff;
    user_code->limit_high = 0xf;
    user_code->segment = 1;  // code segment
    user_code->granularity = 1; // 4KB
    user_code->big = 1;
    user_code->long_mode = 0;   // 32 bits
    user_code->present = 1;     // present in memory
    user_code->privilege = 3;         // DPL = 3
    user_code->type = 0b1010;   // type= 1CRA(1010)

    global_descriptor* user_data = get_from_gdt(USER_DATA_SELECTOR >> 3);
    user_data->base_low = 0;
    user_data->base_high = 0;
    user_data->limit_low = 0xffff;
    user_data->limit_high = 0xf;
    user_data->segment = 1;  // data segment
    user_data->granularity = 1; // 4KB
    user_data->big = 1;
    user_data->long_mode = 0;   // 32 bits
    user_data->present = 1;     // present in memory
    user_data->privilege = 3;         // DPL = 3
    user_data->type = 0b0010;   // 0DWA(0010)
}