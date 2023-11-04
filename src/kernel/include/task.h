#include "type.h"

#define TASK_SIZE 1024

typedef struct PCB {
    u32* stack;
} PCB;

typedef struct saved_register {
    u32 edi;
    u32 esi;
    u32 ebx;
    u32 ebp;
    u32 eip;
} saved_register;

typedef struct pcb_manager {
    u32 front;
    u32 rear;
    PCB* current;
    PCB* tasks[TASK_SIZE];
} pcb_manager;

void pcb_manager_init();
void enqueue(PCB* pcb);
PCB* dequeue();
int is_empty();
int is_full();

void create_task(void (*entry)(), PCB* pcb);
PCB *get_current_task();

void schedule();

