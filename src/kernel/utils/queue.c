#include "../include/type.h"
#include "../include/queue.h"
#include "../include/memory.h"

void init_wait_queue(
    wait_queue* wq,
    void (*add_wait_queue)(struct wait_queue*, PCB*),
    PCB* (*fetch_wait_queue)(struct wait_queue*)
) {
    wq->length = 0;
    wq->add_wait_queue = add_wait_queue;
    wq->fetch_wait_queue = fetch_wait_queue;
    
    // head node
    pcb_queue* q = (pcb_queue*)get_paddr_from_ppn(
        allocate_physical_page_for_kernel());
    
    wq->head = q;
    wq->tail = q;
}

