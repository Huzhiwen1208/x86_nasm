#include "../include/task.h"
#include "../include/constant.h"
#include "../include/syscall.h"
#include "../include/stdio.h"
#include "../include/log.h"
#include "../include/time.h"
#include "../include/memory.h"
#include "../include/utils.h"

/// @brief the global task manager
pcb_manager PCB_MANAGER;
pid_allocator PID_ALLOCATOR;
/// @brief the idle task
PCB* idle_pcb;

u32 allocate_pid() {
    for (i32 i = 0;i < TASK_SIZE >> 5; i ++) {
        if (PID_ALLOCATOR.pids[i] != 0xffffffff) {
            for (i32 j = 0;j < 32; j ++) {
                if ((PID_ALLOCATOR.pids[i] & (1 << j)) == 0) {
                    PID_ALLOCATOR.pids[i] |= (1 << j);
                    return (i << 5) + j;
                }
            }
        }
    }

    return -1;
}
void free_pid(u32 pid) {
    u32 index = pid >> 5;
    u32 offset = pid - (index << 5);
    PID_ALLOCATOR.pids[index] &= ~(1 << offset);
}

/// @brief initialize the task manager
void pcb_manager_init() {
    PCB_MANAGER.front = 0;
    PCB_MANAGER.rear = 0;
    PCB_MANAGER.current = 0;

    PCB_MANAGER.sleep_pcb_list = 0x0;
}

/// @brief add a task to the ready queue
/// @param pcb 
void enqueue(PCB* pcb) {
    assert(pcb->status == Ready);
    if (is_full()) {
        panic("PCB queue is full");
    }

    PCB_MANAGER.tasks[PCB_MANAGER.rear] = pcb;
    PCB_MANAGER.rear = (PCB_MANAGER.rear + 1) % TASK_SIZE;
}

/// @brief get a task(pcb) from the ready queue by FIFO
/// @return pcb 
PCB* dequeue() {
    if (is_empty()) {
        panic("dequeue should not reach here");
    }

    PCB* result = PCB_MANAGER.tasks[PCB_MANAGER.front];
    PCB_MANAGER.tasks[PCB_MANAGER.front] = 0x0; // free the memory
    PCB_MANAGER.front = (PCB_MANAGER.front + 1) % TASK_SIZE;
    return result;
}

void zombied(PCB* pcb) {
    for(i32 i = 0;i < TASK_SIZE; i++) {
        if (PCB_MANAGER.zombies[i] == NULL) {
            PCB_MANAGER.zombies[i] = pcb;
            return;
        }
    }
    panic("zombied tasks is full");
}

PCB* get_zombied_task_by_ppid(u32 ppid, u32 pid) {
    PCB* result = NULL;

    for (i32 i = 0;i < TASK_SIZE; i++) {
        if (PCB_MANAGER.zombies[i] == NULL)
            continue;
        if (PCB_MANAGER.zombies[i]->parent_pid == ppid &&
            (PCB_MANAGER.zombies[i]->pid == pid || pid == -1)) {
            result = PCB_MANAGER.zombies[i];
            PCB_MANAGER.zombies[i] = NULL;
            break;
        }
    }

    return result;
}

i32 is_empty() {
    return PCB_MANAGER.front == PCB_MANAGER.rear;
}

i32 is_full() {
    return (PCB_MANAGER.rear + 1) % TASK_SIZE == PCB_MANAGER.front;
}

/// @brief create a task and add it to the ready queue, the entry is the thread function
///        the memory location of the pcb is the physical address => pcb, which should align to 4KB(One Page)
/// @param entry 
/// @param pcb 
static void create_kernel_task(void (*entry)(), PCB* pcb) {
    u32 stack = (u32)pcb + (u32)PAGE_SIZE;

    stack -= sizeof(saved_register);

    saved_register* sr = (saved_register*)stack;
    sr->eip = (u32)entry;
    sr->ebp = 0x0;
    sr->ebx = 0x0;
    sr->esi = 0x0;
    sr->edi = 0x0;

    pcb->stack = stack;
    pcb->status = Ready;
    pcb->mode = Kernel;
    pcb->pid = allocate_pid();
    pcb->root_ppn = get_root_ppn();
    pcb->kernel_stack = get_paddr_from_ppn(allocate_physical_page_for_kernel()) + PAGE_SIZE;

    enqueue(pcb);
}

void goto_user() {
    PCB* cur = get_current_task();
    u32 ctx_addr = cur->kernel_stack - sizeof(trap_context);
    asm volatile ("movl %0, %%esp" : : "m"(ctx_addr));
    asm volatile ("jmp __restore");
}
static void create_user_task(void (*user_entry)(), PCB* pcb) {
    u32 stack = (u32)pcb + (u32)PAGE_SIZE;

    stack -= sizeof(saved_register);

    saved_register* sr = (saved_register*)stack;
    sr->eip = (u32)goto_user;
    sr->ebp = 0x0;
    sr->ebx = 0x0;
    sr->esi = 0x0;
    sr->edi = 0x0;

    pcb->stack = stack;
    pcb->status = Ready;
    pcb->mode = User;
    pcb->pid = allocate_pid();
    pcb->root_ppn = copy_root_ppn();
    pcb->kernel_stack = get_paddr_from_ppn(allocate_physical_page_for_kernel()) + PAGE_SIZE;

    trap_context* ctx = (trap_context*)((u32)pcb->kernel_stack - sizeof(trap_context));
    ctx->vector=0x80;
    ctx->edi=0;
    ctx->esi=0;
    ctx->ebp=0;
    ctx->esp=0;
    ctx->ebx=0;
    ctx->edx=0;
    ctx->ecx=0;
    ctx->eax=0;
    ctx->gs=USER_DATA_SELECTOR;
    ctx->fs=USER_DATA_SELECTOR;
    ctx->es=USER_DATA_SELECTOR;
    ctx->ds=USER_DATA_SELECTOR;
    ctx->error_code=0x88888888;
    ctx->eip=user_entry;
    ctx->cs=USER_CODE_SELECTOR;
    ctx->eflags=(0 << 12 | 0b10 | 1 << 9);
    ctx->esp3=USER_STACK_TOP;
    ctx->ss3=USER_DATA_SELECTOR;

    enqueue(pcb);
}

/// @brief get the current task on cpu
/// @return pcb(task)
PCB* get_current_task() {
    return PCB_MANAGER.current;
}

/// @brief fetch a task from the ready queue, if it is empty, return the idle task
/// @return pcb 
PCB* fetch_task() {
    if(is_empty() && PCB_MANAGER.current != NULL && PCB_MANAGER.current->status == Running) {
        return PCB_MANAGER.current;
    }

    if (is_empty()) {
        return idle_pcb;
    }

    return dequeue();
}

/// @brief run the first task
static void run_first_task() {
    PCB* first_task = fetch_task();
    PCB_MANAGER.current = first_task;
    PCB unused;
    unused.stack = 0x0;
    PCB* unused_ptr = &unused;
    first_task->status = Running;
    if (first_task->mode == User) {
        set_tss_esp0(first_task->kernel_stack);
        set_cr3(get_paddr_from_ppn(first_task->root_ppn));
    }
    __switch(unused_ptr, first_task);
}

/// @brief schedule the tasks intensively
void schedule() {
    if (PCB_MANAGER.current == NULL) {
        run_first_task();
        return;
    }
    PCB* next_task = fetch_task();
    PCB* current = PCB_MANAGER.current;

    switch (current->status) {
        case Running:
            current->status = Ready;
            if (current->pid) enqueue(current);
            break;
        case Block:
        case Zombie:
            break;
        case Ready:
        default:
            panic("pigs might fly!");
    }
    if (next_task->mode == User) {
        set_tss_esp0(next_task->kernel_stack);
    }
    if (next_task->root_ppn != get_root_ppn()) {
        set_cr3(get_paddr_from_ppn(next_task->root_ppn));
    }

    PCB_MANAGER.current = next_task;
    PCB_MANAGER.current->status = Running;
    __switch(current, next_task);
}

/// @brief wakeup a task => add it to the ready queue
/// @param pcb 
void wakeup(PCB* pcb) {
    assert(pcb->status == Block);
    pcb->status = Ready;
    enqueue(pcb);
}

/// @brief judge whether the sleep queue is empty
/// @return bool refers to whether the sleep queue is empty
static i8 sleep_queue_is_empty() {
    return PCB_MANAGER.sleep_pcb_list == 0x0;
}

/// @brief block the current task until the end_time_ms arrives
/// @param pcb 
/// @param end_time_ms 
void sleep_enqueue(PCB* pcb, u32 end_time_ms) {
    assert(pcb->status == Running);
    pcb->status = Block;

    if (sleep_queue_is_empty()) {
        PCB_MANAGER.sleep_pcb_list = (sleep_pcb*)get_paddr_from_ppn(
            allocate_physical_page_for_kernel());

        PCB_MANAGER.sleep_pcb_list->pcb = pcb;
        PCB_MANAGER.sleep_pcb_list->sleep_end_time_ms = end_time_ms;
        PCB_MANAGER.sleep_pcb_list->next = 0x0;
        return;
    }

    sleep_pcb* p = PCB_MANAGER.sleep_pcb_list;
    sleep_pcb* prep = NULL;
    while(p) {
        if (p->sleep_end_time_ms > end_time_ms) {
            if (prep) {
                prep->next = (sleep_pcb*)get_paddr_from_ppn(
                    allocate_physical_page_for_kernel());
                prep->next->pcb = pcb;
                prep->next->sleep_end_time_ms = end_time_ms;
                prep->next->next = p;
                return;
            }else {
                sleep_pcb* new_node = (sleep_pcb*)get_paddr_from_ppn(
                    allocate_physical_page_for_kernel());
                new_node->pcb = pcb;
                new_node->sleep_end_time_ms = end_time_ms;
                new_node->next = p;
                PCB_MANAGER.sleep_pcb_list = new_node;
                return;
            }
        }
        prep = p;
        p = p->next;
    }

    // insert to the end
    prep->next = (sleep_pcb*)get_paddr_from_ppn(
        allocate_physical_page_for_kernel());
    prep->next->pcb = pcb;
    prep->next->sleep_end_time_ms = end_time_ms;
    prep->next->next = 0x0;
}

/// @brief wake up all the tasks in the sleep queue whose end_time_ms is less than current_time_ms
/// @param current_time_ms 
void sleep_wakeup(u32 current_time_ms) {
    if (sleep_queue_is_empty()) {
        // debug("sleep queue is empty");
        return;
    }

    while (!sleep_queue_is_empty()) {
        sleep_pcb* p = PCB_MANAGER.sleep_pcb_list;
        if (p->sleep_end_time_ms > current_time_ms) {
            return;
        }
        wakeup(p->pcb);
        PCB_MANAGER.sleep_pcb_list = p->next;
        free_physical_page((u32)p >> 12);
    }
}


/// @brief idle task
static void idle() {
    while(true) {
        asm volatile ("sti");
        asm volatile ("hlt");
        schedule();
    }
}

/// @brief initialize the task manager and idle task
void task_init() {
    pcb_manager_init();
    create_kernel_task(idle, get_paddr_from_ppn(allocate_physical_page_for_kernel()));
    idle_pcb = fetch_task();
}

void go_to_user_mode(void* user_entry) {
    PCB* current = get_current_task();

    set_tss_esp0(current->kernel_stack);
    current->root_ppn = copy_root_ppn();
    set_cr3(get_paddr_from_ppn(current->root_ppn));

    trap_context* ctx = (trap_context*)((u32)current->kernel_stack - sizeof(trap_context));
    ctx->vector=0x80;
    ctx->edi=0;
    ctx->esi=0;
    ctx->ebp=0;
    ctx->esp=0;
    ctx->ebx=0;
    ctx->edx=0;
    ctx->ecx=0;
    ctx->eax=0;
    ctx->gs=USER_DATA_SELECTOR;
    ctx->fs=USER_DATA_SELECTOR;
    ctx->es=USER_DATA_SELECTOR;
    ctx->ds=USER_DATA_SELECTOR;
    ctx->error_code=0x88888888;
    ctx->eip=user_entry;
    ctx->cs=USER_CODE_SELECTOR;
    ctx->eflags=(0 << 12 | 0b10 | 1 << 9);
    ctx->esp3=USER_STACK_TOP;
    ctx->ss3=USER_DATA_SELECTOR;

    asm volatile ("movl %0, %%esp" : : "m"(ctx));
    asm volatile ("jmp __restore");
}

// test
extern void user_thread();
extern void user_thread1();

void thread_a() {
    go_to_user_mode(user_thread);
}

void thread_b() {
    asm volatile ("sti");
    go_to_user_mode(user_thread);
}

/// @brief test the task manager
void task_test() {
    // create_kernel_task(thread_a, get_paddr_from_ppn(allocate_physical_page_for_kernel()));
    create_user_task(user_thread, get_paddr_from_ppn(allocate_physical_page_for_kernel()));
    asm volatile ("sti");
}

void build_child_stack(PCB* child, PCB* parent) {
    u32 stack = (u32)child + (u32)PAGE_SIZE;

    stack -= sizeof(saved_register);

    saved_register* sr = (saved_register*)stack;
    sr->eip = (u32)goto_user;
    sr->ebp = 0x0;
    sr->ebx = 0x0;
    sr->esi = 0x0;
    sr->edi = 0x0;

    child->stack = stack;

    u32 child_kernel_stack = child->kernel_stack;
    u32 parent_kernel_stack = parent->kernel_stack;
    memcpy(child_kernel_stack-PAGE_SIZE, parent_kernel_stack-PAGE_SIZE, PAGE_SIZE);

    trap_context* ctx = (trap_context*)(child_kernel_stack - sizeof(trap_context));
    ctx->eax = 0;
}

// fork
u32 pcb_fork() {
    PCB* parent = get_current_task();
    PCB* child = (PCB*)get_paddr_from_ppn(allocate_physical_page_for_kernel());
    child->status = Ready;
    child->mode = parent->mode;
    child->pid = allocate_pid();
    child->parent_pid = parent->pid;
    child->root_ppn = copy_root_ppn_recursion();
    child->kernel_stack = get_paddr_from_ppn(allocate_physical_page_for_kernel()) + PAGE_SIZE;

    build_child_stack(child, parent);
    enqueue(child);
    schedule();
    return 1;
}

void free_page_table(PCB* pcb) {
    u32 root_ppn = pcb->root_ppn;
    free_page_table_recursion(root_ppn);
}

void free_kernel_task(PCB* pcb) {
    u32 kernel_stack = pcb->kernel_stack;
    free_physical_page(get_ppn_from_paddr_floor(kernel_stack - PAGE_SIZE));
}

void change_parent_pid(u32 pid, u32 parent_pid) {
    for (i32 i = 0;i < TASK_SIZE; i ++) {
        if (PCB_MANAGER.tasks[i] != NULL && PCB_MANAGER.tasks[i]->parent_pid == pid) {
            PCB_MANAGER.tasks[i]->parent_pid = parent_pid;
        }
    }
}

// exit
void pcb_exit(i32 exit_code) {
    PCB* current = get_current_task();
    // free page table 
    free_page_table(current);
    // free kernel stack
    free_kernel_task(current);
    // free pid
    free_pid(current->pid);
    // change parent pid of children to parent pid of current task
    change_parent_pid(current->pid, current->parent_pid);

    // status to zombie
    current->status = Zombie;
    current->exit_code = exit_code;
    zombied(current);
    schedule();
}

// waitpid
i32 pcb_waitpid(u32 pid, i32* exit_code) {
    // find child pid that its status is Zombie
    PCB* cur = get_current_task();
    PCB* activate_child = NULL;
    for (i32 i = 0; i < TASK_SIZE; i++) {
        if (PCB_MANAGER.tasks[i] != NULL &&
            PCB_MANAGER.tasks[i]->parent_pid == cur->pid &&
            (pid == -1 || PCB_MANAGER.tasks[i]->pid == pid)
        ) {
            activate_child = PCB_MANAGER.tasks[i];
        }
    }

    PCB* zombie_child = get_zombied_task_by_ppid(cur->pid, pid);
    // has no specified child
    if (activate_child == NULL && zombie_child == NULL) {
        return -1;
    }

    if (zombie_child == NULL) {
        return -2;
    }

    u32 found_pid = zombie_child->pid;
    *exit_code = zombie_child->exit_code;
    free_page_table(zombie_child);
    return found_pid;
}