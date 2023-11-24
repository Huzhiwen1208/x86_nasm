extern void console_init();
extern void gdt_tss_init();
extern void interrupt_init();
extern void clock_init();
extern void mapping_init();
extern void task_init();
extern void task_test();
extern void mutex_init();
extern void keyboard_init();
extern void buddy_heap_init();
extern void ide_init();
extern void fs_init();
extern void create_file(char* name);

void kernel_main() {
    console_init();
    gdt_tss_init();
    interrupt_init();
    mapping_init();
    mutex_init();
    keyboard_init();
    buddy_heap_init();
    ide_init();
    fs_init();
    create_file("test");
    create_file("asdad");
    // task_init();
    // task_test();
}