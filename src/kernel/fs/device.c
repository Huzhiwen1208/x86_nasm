#include "../include/device.h"
#include "../include/utils.h"

#define DEVICE_MAX 32
static device device_table[DEVICE_MAX];

void device_init() {
    for (int i = 0; i < DEVICE_MAX; i++) {
        strcpy(device_table[i].name, "null");
        device_table[i].type = DEV_NULL;
        device_table[i].subtype = 0;
        device_table[i].dev_num = i;
        device_table[i].parent = 0;
        device_table[i].read = NULL;
        device_table[i].write = NULL;
        device_table[i].ioctl = NULL;
    }
}

u32 get_unused_device() {
    for (int i = 1; i < DEVICE_MAX; i++) {
        if (strcmp(device_table[i].name, "null")) {
            return i;
        }
    }
    return -1;
}

u32 install_device(i32 type, i32 subtype, void* ptr, char* name, i32 parent, void* ioctl, void* read, void* write) {
    device* dev = &device_table[get_unused_device()];
    strcpy(dev->name, name);
    dev->type = type;
    dev->subtype = subtype;
    dev->ptr = ptr;
    dev->parent = parent;
    dev->ioctl = ioctl;
    dev->read = read;
    dev->write = write;
    return dev->dev_num;
}

u32 uninstall_device(u32 dev_num) {
    device* dev = &device_table[dev_num];
    strcpy(dev->name, "null");
    dev->type = DEV_NULL;
    dev->subtype = 0;
    dev->ptr = NULL;
    dev->parent = 0;
    dev->ioctl = NULL;
    dev->read = NULL;
    dev->write = NULL;
    return dev->dev_num;
}

device* find_device_by_id(u32 dev_num) {
    device* dev = &device_table[dev_num];
    if (dev->type == DEV_NULL) {
        return NULL;
    }
    return dev;
}

device* find_device_by_name(char* name) {
    for (int i = 0; i < DEVICE_MAX; i++) {
        if (strcmp(device_table[i].name, name)) {
            return &device_table[i];
        }
    }
    return NULL;
}

i32 device_ioctl(u32 dev_num, i32 cmd, void* args) {
    device* dev = find_device_by_id(dev_num);
    if (dev == NULL) {
        return -1;
    }
    if (dev->ioctl == NULL) {
        return -1;
    }
    return dev->ioctl(dev_num, cmd, args);
}

i32 device_read(u32 dev_num, void* buf, u32 count, u32 seek) {
    device* dev = find_device_by_id(dev_num);
    if (dev == NULL) {
        return -1;
    }
    if (dev->read == NULL) {
        return -1;
    }
    return dev->read(dev->ptr, buf, count, seek);
}

i32 device_write(u32 dev_num, void* buf, u32 count, u32 seek) {
    device* dev = find_device_by_id(dev_num);
    if (dev == NULL) {
        return -1;
    }
    if (dev->write == NULL) {
        return -1;
    }
    return dev->write(dev->ptr, buf, count, seek);
}