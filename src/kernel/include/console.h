#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "type.h"

void console_init();
void console_write_with_color(const char* buffer, u32 len, u8 color);
void console_write(const char* buffer, u32 len);

#endif // __CONSOLE_H__