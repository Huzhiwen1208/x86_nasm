#include "../include/io.h"
#include "../include/interrupt.h"
#include "../include/io.h"
#include "../include/log.h"
#include "../include/task.h"
#include "../include/stdio.h"
#include "../include/time.h"

#define COUNTER0 0x40 // PIT counter 0 port, used for system timer clock
#define COUNTER1 0x41 // PIT counter 1 port, used for DRAM refresh
#define COUNTER2 0x42 // PIT counter 2 port, used for speaker

/*
| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| SC1 | SC0 | RL1 | RL0 | M2  | M1  | M0  | BCD |
SC: Select Counter
    - 00 counter_0
    - 01 counter_1
    - 10 counter_2
    - 11 invalidation
RL: Read/Load
    - 00 lock memory, only cpu can read/write
    - 01 only read/write low byte
    - 10 only read/write high byte
    - 11 read/write low byte first, then read/write high byte
M: Mode
    - 000：trigger interrupt when counter is 0
    - 001：hardware retriggerable one-shot
    - x10：rate generator
    - x11：square wave generator
    - 100：software triggered strobe
    - 101：hardware triggered strobe
BCD: Binary Coded Decimal
    - 0: 16-bit binary
    - 1: four-digit BCD
*/
#define PIT_CTRL 0x43 // PIT control port

#define HZ 100 // system timer clock frequency
#define OSCILLATOR 1193182 // oscillator frequency
#define CLOCK_COUNTER (OSCILLATOR / HZ) // clock counter
#define JIFFY (1000 / HZ) // jiffy ms
const float tick_s = 1.0 / OSCILLATOR; // one tick in s

u64 jiffies; // system jiffies since boot

static u16 read_counter0() {
    writeb(PIT_CTRL, 0b00110100); // 00 00 000 0
    u8 low = readb(COUNTER0);
    u8 high = readb(COUNTER0);
    return (high << 8) | low;
}

// the max usec of 32bits is 4294967295, which is about 1.19 hours
// should optimize this function to support larger time
void get_time(time_val* tv) {
    u32 usec = jiffies * JIFFY * 1000 + (CLOCK_COUNTER - read_counter0()) * (tick_s * 1000 * 1000);
    tv->sec = usec / (1000 * 1000);
    tv->usec = usec % (1000 * 1000);
}

u32 get_time_ms() {
    return jiffies * JIFFY + (CLOCK_COUNTER - read_counter0()) * (tick_s * 1000);
}

static void clock_handler(i32 vector) {
    jiffies ++;
    assert(vector == 0x20);
    send_eoi(vector);

    // checkout sleep queue
    sleep_wakeup(get_time_ms());
    schedule();
}

// Initialize Programmable Interval Timer
static void pit_init() {
    writeb(PIT_CTRL, 0b00110100); // 00 11 010 0
    writeb(COUNTER0, CLOCK_COUNTER & 0xff);
    writeb(COUNTER0, CLOCK_COUNTER >> 8);
}

void clock_init() {
    pit_init();
    set_interrupt_handler(0x20, clock_handler);
    set_interrupt_mask(0x20);   
    asm volatile ("cli");
    jiffies = 0;
}