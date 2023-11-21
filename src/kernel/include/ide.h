#ifndef __IDE_H__
#define __IDE_H__

#include "type.h"
#include "lock.h"
#include "constant.h"

typedef struct ide_disk {
    char name[8];
    struct ide_control_t* ctl;
    u8 selector;
    bool is_master; 
} ide_disk;

typedef struct ide_control_t {
    char name[8];
    spin_lock slock;
    u16 iobase;   // register base addr of io 
    ide_disk disks[IDE_DISK_COUNT];
    ide_disk* now_disk;
} ide_control_t;

/// @brief 
/// @param disk 
/// @param buf 
/// @param count: sector count
/// @param lba
/// @return 
i32 ide_pio_read(ide_disk* disk, void* buf, u8 count, u32 lba);
i32 ide_pio_write(ide_disk* disk, void* buf, u8 count, u32 lba);

void ide_init();

#endif