#include "type.h"

typedef struct time_val {
    u32 sec;
    u32 usec; // current us = sec * 1000 * 1000 + usec
} time_val;

void get_time(time_val* tv);