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

typedef struct pcb_manager {
    PCB* current;
    u32 front;
    u32 rear;
    PCB* tasks[TASK_SIZE];
    PCB* wait_tasks[TASK_SIZE];
    u32 wait_front;
    u32 wait_rear;
} pcb_manager;

void task_init();
void task_test();
pcb_manager get_pcb_manager();