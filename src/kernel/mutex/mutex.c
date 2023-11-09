#include "../include/mutex.h"
#include "../include/type.h"
#include "../include/queue.h"
#include "../include/interrupt.h"
#include "../include/stdio.h"
#include "../include/memory.h"
#include "../include/task.h"

mutex mutex1;

/*
    typedef struct pcb_queue {
        PCB* pcb;
        struct pcb_queue* next;
    } pcb_queue;

    typedef struct wait_queue {
        pcb_queue* head;
        pcb_queue* tail;
        u32 length;

        /// @brief should implement by self when using this function, add pcb
        /// @param wq 
        /// @param pcb 
        void (*add_wait_queue)(struct wait_queue*, PCB*);
        /// @brief should implement by self when using this function, fetch pcb
        /// @param wq 
        /// @return 
        PCB* (*fetch_wait_queue)(struct wait_queue*);
    } wait_queue;
*/
void add_wait_queue(wait_queue* wq, PCB* pcb) {
    assert(pcb->status == Running);
    pcb->status = Block;

    pcb_queue* node = (pcb_queue*)get_paddr_from_ppn(
        allocate_physical_page_for_kernel());
    node->pcb = pcb;
    node->next = 0x0;

    wq->tail->next = node;
    wq->tail = node;
    wq->length++;
}

PCB* fetch_wait_queue(wait_queue* wq) {
    assert(wq->length > 0);
    pcb_queue* node = wq->head->next;
    PCB* pcb = node->pcb;
    assert(pcb->status == Block);
    
    wq->head->next = node->next;
    wq->length--;
    if (wq->length == 0) {
        wq->tail = wq->head;
    }
    free_physical_page(get_ppn_from_paddr_floor((u32)node));
    return pcb;
}

void mutex_lock() {
    u8 interrupt_status = get_interrupt_status();
    asm volatile ("cli");

    // if locking, block
    while (mutex1.lock == 0) {
        add_wait_queue(&mutex1.wq, get_current_task());
        schedule();
    }

    mutex1.lock = 0;
    restore_interrupt_status(interrupt_status);
}

void mutex_unlock() {
    u8 interrupt_status = get_interrupt_status();
    asm volatile ("cli");
    assert(mutex1.lock == 0);

    // unlock
    mutex1.lock = 1;

    if (mutex1.wq.length > 0) {
        PCB* pcb = fetch_wait_queue(&mutex1.wq);
        wakeup(pcb);
        schedule();
    }
    restore_interrupt_status(interrupt_status);
}

void mutex_init() {
    mutex1.lock = 1;
    init_wait_queue(&mutex1.wq, add_wait_queue, fetch_wait_queue);
}