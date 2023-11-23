#include "../include/fs.h"
#include "../include/type.h"
#include "../include/constant.h"
#include "../include/console.h"
#include "../include/interrupt.h"

i32 write(u32 fd, char *buf, u32 len) {
    switch (fd) {
    case STDOUT:
    case STDERR:
        return console_write(buf, len);
    case STDIN:
        return keyboard_read(buf, len);
    default:
        panic("write: fd: %d not supported", fd);
    }

    return -1;
}