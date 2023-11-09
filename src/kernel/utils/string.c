#include "../include/utils.h"
#include "../include/type.h"

size_t length(const char* str) {
    size_t len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

void concat(char* s1, char* s2) {
    size_t len = length(s1);
    char* ptr = s1 + len;
    char* q = s2;
    while (q != NULL) {
        *ptr++ = *q++;
    }
}

i32 is_digit(char c) {
    return c >= '0' && c <= '9';
}