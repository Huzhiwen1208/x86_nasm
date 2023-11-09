#include "../include/type.h"
#include "../include/queue.h"
#include "../include/memory.h"
#include "../include/utils.h"

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

void default_add_wait_queue(wait_queue* wq, PCB* pcb) {
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

PCB* default_fetch_wait_queue(wait_queue* wq) {
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
