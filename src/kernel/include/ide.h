#ifndef __IDE_H__
#define __IDE_H__

#include "type.h"
#include "lock.h"
#include "constant.h"

typedef struct ide_disk {
    char name[8];
    struct ide_control_t *ctl;
    u8 selector;
    bool is_master;
    u32 total_sector;
    u32 c, h, s; // CHS
} ide_disk;

typedef struct ide_control_t {
    char name[8];
    spin_lock slock;
    u16 iobase; // register base addr of io
    ide_disk disks[IDE_DISK_COUNT];
    ide_disk *now_disk;
    u8 control_byte; // reset driver
} ide_control_t;

typedef struct ide_params_t {
    u16 config;
    u16 cylinders;
    u16 r0;
    u16 heads;
    u16 r1[5 - 3];
    u16 sectors;
    u16 r2[9 - 6];
    u8 serial[20];
    u16 r3[22 - 19];
    u8 firmware[8];
    u8 model[40];
    u8 drq_sectors;
    u8 r4[3];
    u16 capabilities;
    u16 r5[59 - 49];
    u32 total_sector;
    u16 r6;
    u16 mdma_mode;
    u8 r7;
    u8 pio_mode;
    u16 r8[79 - 64];
    u16 major_version;
    u16 minor_version;
    u16 commmand_sets[87 - 81];
    u16 r9[118 - 87];
    u16 support_settings;
    u16 enable_settings;
    u16 r10[221 - 120];
    u16 transport_major;
    u16 transport_minor;
    u16 r11[254 - 223];
    u16 integrity;
} _no_align;

typedef struct ide_params_t ide_params;

/// @brief
/// @param disk
/// @param buf
/// @param count: sector count
/// @param lba
/// @return
i32 ide_pio_read(ide_disk *disk, void *buf, u8 count, u32 lba);
i32 ide_pio_write(ide_disk *disk, void *buf, u8 count, u32 lba);
void ide_init();

#endif