#include "../include/utils.h"
#include "../include/type.h"

void memcpy(void* dst, const void* src, size_t n) {
    u8* d = (u8*)dst;
    const u8* s = (const u8*)src;
    while (n--) {
        *d++ = *s++;
    }
}

// clear memory from start to start+n, byte by byte
void memfree(void* start, size_t n) {
    u8* s = (u8*)start;
    while (n--) {
        *s++ = 0;
    }
}