#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "type.h"

void exception_handler(i32 vector);

void send_eoi(int vector);

void interrupt_init();

void set_interrupt_mask(i32 vector);

void clear_interrupt_mask(i32 vector);

void set_interrupt_handler(i32 vector, void* handler);

u8 get_interrupt_status();

void restore_interrupt_status(u8 status);

#endif