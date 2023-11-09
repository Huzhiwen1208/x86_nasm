#ifndef TASK_H
#define TASK_H

#include "type.h"

#define TASK_SIZE 1024

enum PCB_STATUS {
    Running,
    Block,
    Ready
};

typedef struct PCB {
    u32* stack;
    enum PCB_STATUS status;
    u64 sleep_start_jeffy;
} PCB;

typedef struct saved_register {
    u32 edi;
    u32 esi;
    u32 ebx;
    u32 ebp;
    u32 eip;
} saved_register;

typedef struct sleep_pcb {
    PCB* pcb;
    u32 sleep_end_time_ms;
    struct sleep_pcb* next;
} sleep_pcb;

typedef struct pcb_manager {
    PCB* current;
    u32 front;
    u32 rear;
    PCB* tasks[TASK_SIZE];

    /// @brief sleep queue, priority queue
    sleep_pcb* sleep_pcb_list;
} pcb_manager;

void task_init();
void task_test();

void sleep_enqueue(PCB* pcb, u32 end_time_ms);
void sleep_wakeup(u32 current_time_ms);

void wakeup(PCB* pcb);

PCB* get_current_task();

#endif