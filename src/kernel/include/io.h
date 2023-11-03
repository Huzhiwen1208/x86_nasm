#include "type.h"

void writeb(u16 port, u8 value); // write a byte to specified port
u8 readb(u16 port);              // read a byte from specified port

void writew(u16 port, u16 value); // write a word to specified port
u16 readw(u16 port);              // read a word from specified port