#include "../include/task.h"
#include "../include/constant.h"

pcb_manager PCB_MANAGER;

void pcb_manager_init() {
    PCB_MANAGER.front = 0;
    PCB_MANAGER.rear = 0;
    PCB_MANAGER.current = 0;
}

void enqueue(PCB* pcb) {
    if (is_full()) {
        panic("PCB queue is full");
    }

    PCB_MANAGER.tasks[PCB_MANAGER.rear] = pcb;
    PCB_MANAGER.rear = (PCB_MANAGER.rear + 1) % TASK_SIZE;
}

PCB* dequeue() {
    if (is_empty()) {
        panic("PCB queue is empty");
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

void create_task(void (*entry)(), PCB* pcb) {
    u32 stack = pcb + PAGE_SIZE;

    stack -= sizeof(saved_register);

    saved_register* sr = (saved_register*)stack;
    sr->eip = (u32)entry;
    sr->ebp = 0x1;
    sr->ebx = 0x2;
    sr->esi = 0x3;
    sr->edi = 0x4;

    pcb->stack = stack;

    enqueue(pcb);
}

PCB* get_current_task() {
    return PCB_MANAGER.current;
}

PCB* fetch_task() {
    if (is_empty()) {
        panic("PCB queue is empty");
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
    enqueue(PCB_MANAGER.current);
    PCB_MANAGER.current = next_task;
    __switch(current, next_task);
}