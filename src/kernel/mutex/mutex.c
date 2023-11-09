#include "../include/mutex.h"
#include "../include/type.h"
#include "../include/queue.h"
#include "../include/interrupt.h"
#include "../include/memory.h"
#include "../include/task.h"
#include "../include/utils.h"

mutex mutex1;

void mutex_lock() {
    u8 interrupt_status = get_interrupt_status();
    asm volatile ("cli");

    // if locking, block
    while (mutex1.lock == 0) {
        mutex1.wq.add_wait_queue(&mutex1.wq, get_current_task());
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
        PCB* pcb = mutex1.wq.fetch_wait_queue(&mutex1.wq);
        wakeup(pcb);
        schedule();
    }
    restore_interrupt_status(interrupt_status);
}

void mutex_init() {
    mutex1.lock = 1;
    init_wait_queue(&mutex1.wq, default_add_wait_queue, default_fetch_wait_queue);
}