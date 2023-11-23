#include "../include/utils.h"
#include "../include/type.h"

size_t length(const char* str) {
    size_t len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

bool strcmp(const char* s1, const char* s2) {
    size_t len1 = length(s1);
    size_t len2 = length(s2);
    if (len1 != len2) {
        return false;
    }
    for (size_t i = 0; i < len1; i++) {
        if (s1[i] != s2[i]) {
            return false;
        }
    }
    return true;
}

void concat(char* s1, char* s2) {
    size_t len = length(s1);
    char* ptr = s1 + len;
    char* q = s2;
    while (q != NULL) {
        *ptr++ = *q++;
    }
}

void strcpy(char* dest, char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

i32 is_digit(char c) {
    return c >= '0' && c <= '9';
}

i32 is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

i32 is_upper(char c) {
    return c >= 'A' && c <= 'Z';
}

i32 is_lower(char c) {
    return c >= 'a' && c <= 'z';
}