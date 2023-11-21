#ifndef __LOCK_H__
#define __LOCK_H__

#include "type.h"
#include "queue.h"
#include "task.h"

typedef struct spin_lock {
    i8 lock;
    PCB* pcb; // current holder
    u32 reentrant_times;
    wait_queue wq;
} spin_lock;

void spin_lock_init(spin_lock* slock);

void spin_lock_lock(spin_lock* slock);

void spin_lock_unlock(spin_lock* slock);

#endif