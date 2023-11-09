#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "type.h"
#include "task.h"

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

/// @brief init wait queue
void init_wait_queue( wait_queue* wq,
    void (*add_wait_queue)(struct wait_queue*, PCB*),
    PCB* (*fetch_wait_queue)(struct wait_queue*)
);

void default_add_wait_queue(wait_queue* wq, PCB* pcb);
PCB* default_fetch_wait_queue(wait_queue* wq);

#endif