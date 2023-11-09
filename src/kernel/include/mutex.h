#ifndef MUTEX_H
#define MUTEX_H

#include "type.h"
#include "queue.h"

typedef struct mutex {
    i8 lock;
    wait_queue wq;
} mutex;

void mutex_lock();

void mutex_unlock();

void mutex_init();

#endif