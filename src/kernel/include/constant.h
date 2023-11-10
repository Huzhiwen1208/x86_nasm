#ifndef __CONSTANT_H__
#define __CONSTANT_H__

#include "type.h"

// PAGE
#define PAGE_SIZE 4096
#define MAX_PAGES (8 << 12)

// GDT
#define GDT_SIZE 8192

// IDT
#define IDT_SIZE 256
#define EXCEPTION_SIZE 20
#define OUTER_INTERRUPT_SIZE 16

// TASK
#define TASK_SIZE 1024

// KEYBOARD
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_CTRL_PORT 0x64

#endif // __CONSTANT_H__