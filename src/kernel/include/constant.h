#ifndef __CONSTANT_H__
#define __CONSTANT_H__

#include "type.h"

// PAGE
#define PAGE_SIZE 4096
#define MAX_PAGES (8 << 12)

// GDT
#define GDT_SIZE 8192
#define KERNEL_CODE_SELECTOR (1 << 3)
#define KERNEL_DATA_SELECTOR (2 << 3)
#define KERNEL_TSS_SELECTOR (3 << 3)
#define USER_CODE_SELECTOR (4 << 3 | 0b11)  // DPL = 3
#define USER_DATA_SELECTOR (5 << 3 | 0b11)  // DPL = 3

// IDT
#define IDT_SIZE 256
#define EXCEPTION_SIZE 20
#define OUTER_INTERRUPT_SIZE 16

// TASK
#define TASK_SIZE 1024
#define USER_STACK_TOP 0x8000000

// KEYBOARD
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_CTRL_PORT 0x64

// Stdio
#define STDIN 0
#define STDOUT 1
#define STDERR 2

// Heap_Block
#define MAX_FREE_BLOCK 256

// DISK PIO
#define SECTOR_BYTE_SIZE 512
#define IDE_CTL_COUNT 2  // the count of ide control
#define IDE_DISK_COUNT 2 // count of disk of every ide control

// FS
#define FS_MAGIC 0x20001208
#define INODE_PER_BLOCK 4

#endif // __CONSTANT_H__