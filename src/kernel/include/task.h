#ifndef TASK_H
#define TASK_H

#include "type.h"
#include "constant.h"

typedef enum PCB_STATUS {
    Running,
    Block,
    Ready,
    Zombie
} PCB_STATUS;

typedef enum PCB_MODE {
    Kernel,
    User
} PCB_MODE;

typedef struct trap_context {
    u32 vector;

    u32 edi;
    u32 esi;
    u32 ebp;
    u32 esp;
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;
    u32 gs;
    u32 fs;
    u32 es;
    u32 ds;

    u32 error_code;

    u32 eip;
    u32 cs;
    u32 eflags;
    u32 esp3; // will not execute to user mode if without esp3 and ss3.
    u32 ss3;
} trap_context;

typedef struct PCB {
    u32 *stack;
    PCB_STATUS status;
    PCB_MODE mode;
    u32 root_ppn;
    u32 kernel_stack;
    u32 pid;
    u32 parent_pid;
    i32 exit_code;
} PCB;

typedef struct pid_allocator {
    u32 pids[TASK_SIZE >> 5];
} pid_allocator;

u32 allocate_pid();
void free_pid(u32 pid);

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
    /// @brief zombie queue
    PCB* zombies[TASK_SIZE];
} pcb_manager;

void task_init();
void task_test();

void sleep_enqueue(PCB* pcb, u32 end_time_ms);
void sleep_wakeup(u32 current_time_ms);

void wakeup(PCB* pcb);

PCB* get_current_task();
u32 pcb_fork();
void pcb_exit(i32 exit_code);
i32 pcb_waitpid(u32 pid, i32* exit_code);

#endif