#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "type.h"
#include "task.h"

typedef enum device_type {
    DEV_NULL,
    DEV_CHAR,
    DEV_BLOCK
} device_type;

typedef enum device_subtype {
    DEV_CONSOLE = 1,
    DEV_KEYBOARD,
    DEV_DISK
} device_subtype;

typedef struct request_param {
    u32 dev_num;
    u32 cmd;
    u32 idx;
    u32 count;
    u8* buf;
    PCB* pcb;
    i32 flags;
} request_param;

typedef struct device {
    char name[16];
    device_type type;
    device_subtype subtype;
    u32 dev_num;
    u32 parent;
    void* ptr;

    i32 (*ioctl)(void* dev_num, i32 cmd, void* args);
    i32 (*read)(void* dev_num, void* buf, u32 count, u32 idx);
    i32 (*write)(void* dev_num, void* buf, u32 count, u32 idx);
} device;

u32 install_device(i32 type, i32 subtype, void* ptr, char* name, i32 parent, void* ioctl, void* read, void* write);
u32 uninstall_device(u32 dev_num);
device* find_device_by_id(u32 dev_num);
device* find_device_by_name(char* name);

i32 device_ioctl(u32 dev_num, i32 cmd, void* args);
i32 device_read(u32 dev_num, void* buf, u32 count, u32 seek);
i32 device_write(u32 dev_num, void* buf, u32 count, u32 seek);
#endif