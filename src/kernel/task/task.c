#include "../include/task.h"
#include "../include/constant.h"
#include "../include/syscall.h"
#include "../include/stdio.h"
#include "../include/log.h"
#include "../include/time.h"
#include "../include/memory.h"

pcb_manager PCB_MANAGER;
PCB* idle_pcb;
void pcb_manager_init() {
    PCB_MANAGER.front = 0;
    PCB_MANAGER.rear = 0;
    PCB_MANAGER.current = 0;

    PCB_MANAGER.wait_front = 0;
    PCB_MANAGER.wait_rear = 0;

    PCB_MANAGER.sleep_pcb_list = 0x0;
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
    u32 stack = (u32)pcb + (u32)PAGE_SIZE;

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
    first_task->status = Running;
    __switch(unused_ptr, first_task);
}

void schedule() {
    if (PCB_MANAGER.current == NULL) {
        run_first_task();
        return;
    }
    PCB* next_task = fetch_task();
    PCB* current = PCB_MANAGER.current;
    if (current->status != Block) {
        enqueue(PCB_MANAGER.current);
    }
    PCB_MANAGER.current = next_task;
    PCB_MANAGER.current->status = Running;
    __switch(current, next_task);
}

static void wakeup(PCB* pcb) {
    assert(pcb->status == Block);
    pcb->status = Ready;
    enqueue(pcb);
}

static void block() {
    PCB* current = PCB_MANAGER.current;
    assert(current->status == Running);
    current->status = Block;
    PCB_MANAGER.wait_tasks[PCB_MANAGER.wait_rear] = current;
    PCB_MANAGER.wait_rear = (PCB_MANAGER.wait_rear + 1) % TASK_SIZE;
}

static void block_wakeup() {
    PCB* result = PCB_MANAGER.wait_tasks[PCB_MANAGER.wait_front];
    PCB_MANAGER.wait_tasks[PCB_MANAGER.wait_front] = 0x0; // free the memory
    PCB_MANAGER.wait_front = (PCB_MANAGER.wait_front + 1) % TASK_SIZE;

    result->status = Ready;
    enqueue(result);
}

static i8 sleep_queue_is_empty() {
    return PCB_MANAGER.sleep_pcb_list == 0x0;
}

void sleep_enqueue(PCB* pcb, u32 end_time_ms) {
    assert(pcb->status == Running);
    pcb->status = Block;

    if (sleep_queue_is_empty()) {
        PCB_MANAGER.sleep_pcb_list = (sleep_pcb*)get_paddr_from_ppn(
            allocate_physical_page_for_kernel());

        PCB_MANAGER.sleep_pcb_list->pcb = pcb;
        PCB_MANAGER.sleep_pcb_list->sleep_end_time_ms = end_time_ms;
        PCB_MANAGER.sleep_pcb_list->next = 0x0;
        return;
    }

    sleep_pcb* p = PCB_MANAGER.sleep_pcb_list;
    sleep_pcb* prep = NULL;
    while(p) {
        if (p->sleep_end_time_ms > end_time_ms) {
            if (prep) {
                prep->next = (sleep_pcb*)get_paddr_from_ppn(
                    allocate_physical_page_for_kernel());
                prep->next->pcb = pcb;
                prep->next->sleep_end_time_ms = end_time_ms;
                prep->next->next = p;
                return;
            }else {
                sleep_pcb* new_node = (sleep_pcb*)get_paddr_from_ppn(
                    allocate_physical_page_for_kernel());
                new_node->pcb = pcb;
                new_node->sleep_end_time_ms = end_time_ms;
                new_node->next = p;
                PCB_MANAGER.sleep_pcb_list = new_node;
                return;
            }
        }
        prep = p;
        p = p->next;
    }

    // insert to the end
    prep->next = (sleep_pcb*)get_paddr_from_ppn(
        allocate_physical_page_for_kernel());
    prep->next->pcb = pcb;
    prep->next->sleep_end_time_ms = end_time_ms;
    prep->next->next = 0x0;
}

void sleep_wakeup(u32 current_time_ms) {
    if (sleep_queue_is_empty()) {
        debug("sleep queue is empty");
        return;
    }

    while (!sleep_queue_is_empty()) {
        sleep_pcb* p = PCB_MANAGER.sleep_pcb_list;
        if (p->sleep_end_time_ms > current_time_ms) {
            return;
        }
        wakeup(p->pcb);
        PCB_MANAGER.sleep_pcb_list = p->next;
        free_physical_page((u32)p >> 12);
    }
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
    time_val tv;
    get_time(&tv);
    printf("A-> time val: {%d %d}\n", tv.sec, tv.usec);
    syscall(SYSCALL_SLEEP, 500, 0, 0);
    get_time(&tv);
    printf("A-> after sleep time val: {%d %d}\n", tv.sec, tv.usec);
    asm volatile ("cli");
    while(true);
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
    // create_task(thread_b, get_paddr_from_ppn(allocate_physical_page_for_kernel()));
    asm volatile ("sti");
}