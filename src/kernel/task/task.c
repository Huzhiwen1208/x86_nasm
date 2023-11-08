#include "../include/task.h"
#include "../include/constant.h"
#include "../include/syscall.h"
#include "../include/stdio.h"
#include "../include/log.h"
#include "../include/time.h"

pcb_manager PCB_MANAGER;
PCB* idle_pcb;
void pcb_manager_init() {
    PCB_MANAGER.front = 0;
    PCB_MANAGER.rear = 0;
    PCB_MANAGER.current = 0;
}

pcb_manager get_pcb_manager() {
    return PCB_MANAGER;
}

// methods
void enqueue(PCB* pcb) {
    if (is_full()) {
        panic("PCB queue is full");
    }

    PCB_MANAGER.tasks[PCB_MANAGER.rear] = pcb;
    PCB_MANAGER.rear = (PCB_MANAGER.rear + 1) % TASK_SIZE;
}

PCB* dequeue() {
    if (is_empty()) {
        panic("dequeue should not reach here");
    }

    PCB* result = PCB_MANAGER.tasks[PCB_MANAGER.front];
    PCB_MANAGER.tasks[PCB_MANAGER.front] = 0x0; // free the memory
    PCB_MANAGER.front = (PCB_MANAGER.front + 1) % TASK_SIZE;
    return result;
}

i32 is_empty() {
    return PCB_MANAGER.front == PCB_MANAGER.rear;
}

i32 is_full() {
    return (PCB_MANAGER.rear + 1) % TASK_SIZE == PCB_MANAGER.front;
}

static void create_task(void (*entry)(), PCB* pcb) {
    u32 stack = pcb + PAGE_SIZE;

    stack -= sizeof(saved_register);

    saved_register* sr = (saved_register*)stack;
    sr->eip = (u32)entry;
    sr->ebp = 0x1;
    sr->ebx = 0x2;
    sr->esi = 0x3;
    sr->edi = 0x4;

    pcb->stack = stack;
    pcb->status = Ready;

    enqueue(pcb);
}

PCB* get_current_task() {
    return PCB_MANAGER.current;
}

PCB* fetch_task() {
    if (is_empty()) {
        return idle_pcb;
    }

    return dequeue();
}

static void run_first_task() {
    PCB* first_task = fetch_task();
    PCB_MANAGER.current = first_task;
    PCB unused;
    unused.stack = 0x0;
    PCB* unused_ptr = &unused;
    __switch(unused_ptr, first_task);
}

void schedule() {
    if (PCB_MANAGER.current == NULL) {
        run_first_task();
        return;
    }
    PCB* next_task = fetch_task();
    PCB* current = PCB_MANAGER.current;
    if (current->status == Block) {
        PCB_MANAGER.wait_tasks[PCB_MANAGER.wait_rear] = current;
        PCB_MANAGER.wait_rear = (PCB_MANAGER.wait_rear + 1) % TASK_SIZE;
    }else {
        enqueue(PCB_MANAGER.current);
    }
    PCB_MANAGER.current = next_task;
    current->status = Running;
    __switch(current, next_task);
}

static void block() {
    PCB* current = PCB_MANAGER.current;
    assert(current->status == Running);
    current->status = Block;
}

static PCB* wakeup() {
    PCB* result = PCB_MANAGER.wait_tasks[PCB_MANAGER.wait_front];
    PCB_MANAGER.wait_tasks[PCB_MANAGER.wait_front] = 0x0; // free the memory
    PCB_MANAGER.wait_front = (PCB_MANAGER.wait_front + 1) % TASK_SIZE;
    assert(result->status == Block);
    result->status = Ready;
    return result;
}

static void idle() {
    while(true) {
        trace("Executing idle thread");
        asm volatile ("sti");
        asm volatile ("hlt");
        schedule();
    }
}

void task_init() {
    pcb_manager_init();
    create_task(idle, get_paddr_from_ppn(allocate_physical_page_for_kernel()));
    idle_pcb = fetch_task();
}

void thread_a() {
    asm volatile ("sti");
    for (i32 i = 0; ; i++ ) {
        time_val tv;
        syscall(SYSCALL_GETTIME_MS, &tv, 0, 0);
        printf("A-> time val: {%d %d}\n", tv.sec, tv.usec);
        while(true);
    }
}
void thread_b() {
    asm volatile ("sti");
    for (i32 i = 0; ; i++ ) {
        time_val tv;
        syscall(SYSCALL_GETTIME_MS, &tv, 0, 0);
        printf("B-> time val: {%d %d}\n", tv.sec, tv.usec);
        while(true);
    }
}
void task_test() {
    create_task(thread_a, get_paddr_from_ppn(allocate_physical_page_for_kernel()));
    create_task(thread_b, get_paddr_from_ppn(allocate_physical_page_for_kernel()));
    asm volatile ("sti");
}