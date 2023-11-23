#include "../include/lock.h"
#include "../include/queue.h"
#include "../include/task.h"
#include "../include/interrupt.h"
#include "../include/utils.h"

void spin_lock_lock(spin_lock* slock) {
    PCB* current = get_current_task();
    if (current == NULL) return;

    u8 interrupt_status = get_interrupt_status();
    asm volatile ("cli");
    while (slock->pcb && slock->pcb != current) {
        slock->wq.add_wait_queue(&slock->wq, current);
        schedule();
    }

    if (slock->pcb == NULL) {
        assert(slock->lock);
        assert(slock->reentrant_times == 0);
        slock->pcb = current;
        slock->reentrant_times = 1;
        slock->lock = 0;
        restore_interrupt_status(interrupt_status);
        return;
    } 

    if (slock->pcb == current) {
        assert(slock->lock == 0);
        assert(slock->reentrant_times);
        slock->reentrant_times++;
        restore_interrupt_status(interrupt_status);
        return;
    }
}

void spin_lock_unlock(spin_lock* slock) {
    PCB* current = get_current_task();
    if (current == NULL) return;

    u8 interrupt_status = get_interrupt_status();
    asm volatile ("cli");
    assert(slock->pcb == current);
    assert(slock->reentrant_times);
    assert(slock->lock == 0);

    slock->reentrant_times--;
    if (slock->reentrant_times == 0) {
        slock->lock = 1;
        slock->pcb = NULL;
        if (slock->wq.length > 0) {
            PCB* pcb = slock->wq.fetch_wait_queue(&slock->wq);
            restore_interrupt_status(interrupt_status);
            wakeup(pcb);
            schedule();
        }
    }
}

void spin_lock_init(spin_lock* slock) {
    slock->lock = 1;
    slock->pcb = NULL;
    slock->reentrant_times = 0;
    init_wait_queue(&slock->wq, default_add_wait_queue, default_fetch_wait_queue);
}