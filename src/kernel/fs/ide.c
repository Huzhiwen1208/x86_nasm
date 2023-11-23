#include "../include/type.h"
#include "../include/io.h"
#include "../include/ide.h"
#include "../include/constant.h"
#include "../include/stdio.h"
#include "../include/lock.h"
#include "../include/memory.h"
#include "../include/log.h"
#include "../include/utils.h"
#include "../include/device.h"

#define IDE_TIMEOUT 60000
// IDE 寄存器基址
#define IDE_IOBASE_PRIMARY 0x1F0   // 主通道基地址
#define IDE_IOBASE_SECONDARY 0x170 // 从通道基地址

// IDE 寄存器偏移
#define IDE_DATA 0x0000       // 数据寄存器
#define IDE_ERR 0x0001        // 错误寄存器
#define IDE_FEATURE 0x0001    // 功能寄存器
#define IDE_SECTOR 0x0002     // 扇区数量
#define IDE_LBA_LOW 0x0003    // LBA 低字节
#define IDE_CHS_SECTOR 0x0003 // CHS 扇区位置
#define IDE_LBA_MID 0x0004    // LBA 中字节
#define IDE_CHS_CYL 0x0004    // CHS 柱面低字节
#define IDE_LBA_HIGH 0x0005   // LBA 高字节
#define IDE_CHS_CYH 0x0005    // CHS 柱面高字节
#define IDE_HDDEVSEL 0x0006   // 磁盘选择寄存器
#define IDE_STATUS 0x0007     // 状态寄存器
#define IDE_COMMAND 0x0007    // 命令寄存器
#define IDE_ALT_STATUS 0x0206 // 备用状态寄存器
#define IDE_CONTROL 0x0206    // 设备控制寄存器
#define IDE_DEVCTRL 0x0206    // 驱动器地址寄存器

// IDE 命令

#define IDE_CMD_READ 0x20       // 读命令
#define IDE_CMD_WRITE 0x30      // 写命令
#define IDE_CMD_IDENTIFY 0xEC   // 识别命令
#define IDE_CMD_DIAGNOSTIC 0x90 // 诊断命令

#define IDE_CMD_READ_UDMA 0xC8  // UDMA 读命令
#define IDE_CMD_WRITE_UDMA 0xCA // UDMA 写命令

#define IDE_CMD_PIDENTIFY 0xA1 // 识别 PACKET 命令
#define IDE_CMD_PACKET 0xA0    // PACKET 命令

// ATAPI 命令
#define IDE_ATAPI_CMD_REQUESTSENSE 0x03
#define IDE_ATAPI_CMD_READCAPICITY 0x25
#define IDE_ATAPI_CMD_READ10 0x28

#define IDE_ATAPI_FEATURE_PIO 0
#define IDE_ATAPI_FEATURE_DMA 1

// IDE 控制器状态寄存器
#define IDE_SR_NULL 0x00 // NULL
#define IDE_SR_ERR 0x01  // Error
#define IDE_SR_IDX 0x02  // Index
#define IDE_SR_CORR 0x04 // Corrected data
#define IDE_SR_DRQ 0x08  // Data request
#define IDE_SR_DSC 0x10  // Drive seek complete
#define IDE_SR_DWF 0x20  // Drive write fault
#define IDE_SR_DRDY 0x40 // Drive ready
#define IDE_SR_BSY 0x80  // Controller busy

// IDE 控制寄存器
#define IDE_CTRL_HD15 0x00 // Use 4 bits for head (not used, was 0x08)
#define IDE_CTRL_SRST 0x04 // Soft reset
#define IDE_CTRL_NIEN 0x02 // Disable interrupts

// IDE 错误寄存器
#define IDE_ER_AMNF 0x01  // Address mark not found
#define IDE_ER_TK0NF 0x02 // Track 0 not found
#define IDE_ER_ABRT 0x04  // Abort
#define IDE_ER_MCR 0x08   // Media change requested
#define IDE_ER_IDNF 0x10  // Sector id not found
#define IDE_ER_MC 0x20    // Media change
#define IDE_ER_UNC 0x40   // Uncorrectable data error
#define IDE_ER_BBK 0x80   // Bad block

#define IDE_LBA_MASTER 0b11100000 // 主盘 LBA
#define IDE_LBA_SLAVE 0b11110000  // 从盘 LBA

ide_control_t ide_control[IDE_CTL_COUNT];

void disk_wait(ide_control_t* ctl) {
    while (true) {
        // should read status from alternate status register 
        // else will cause error: interrupt status register is not ready
        u8 status = readb(ctl->iobase + IDE_ALT_STATUS); 
        if (status & IDE_SR_ERR) {
            spin_lock_unlock(&ctl->slock);
            panic("ide_pio_read: disk error");
        }

        if (status & IDE_SR_BSY) {
            continue;
        }

        if (status & IDE_SR_DRDY) {
            break;
        }
    }
}

i32 ide_pio_read(ide_disk* disk, void* buf, u8 count, u32 lba) {
    if (count == 0) {
        return -1;
    }

    ide_control_t* ctl = disk->ctl;
    spin_lock_lock(&ctl->slock);

    // disk select
    writeb(ctl->iobase + IDE_HDDEVSEL, disk->selector);
    ctl->now_disk = disk;

    // disk wait
    disk_wait(ctl);

    // select sector
    writeb(ctl->iobase + IDE_SECTOR, count);
    writeb(ctl->iobase + IDE_LBA_LOW, lba & 0xFF);
    writeb(ctl->iobase + IDE_LBA_MID, (lba >> 8) & 0xFF);
    writeb(ctl->iobase + IDE_LBA_HIGH, (lba >> 16) & 0xFF);
    writeb(ctl->iobase + IDE_HDDEVSEL, disk->selector | ((lba >> 24) & 0xF));

    // send command: read
    writeb(ctl->iobase + IDE_COMMAND, IDE_CMD_READ);

    // read data
    for (i32 i = 0; i < count; i++) {
        disk_wait(ctl);
        for (i32 j = 0; j < 256; j++) {
            u16 data = readw(ctl->iobase + IDE_DATA);
            ((u16*)buf)[i * 256 + j] = data;
        }
    }

    spin_lock_unlock(&ctl->slock);
    return 0;
}

i32 ide_pio_write(ide_disk* disk, void* buf, u8 count, u32 lba) {
    if (count == 0) {
        return -1;
    }

    ide_control_t* ctl = disk->ctl;
    spin_lock_lock(&ctl->slock);

    // disk select
    writeb(ctl->iobase + IDE_HDDEVSEL, disk->selector);
    ctl->now_disk = disk;

    // disk wait
    disk_wait(ctl);

    // select sector
    writeb(ctl->iobase + IDE_SECTOR, count);
    writeb(ctl->iobase + IDE_LBA_LOW, lba & 0xFF);
    writeb(ctl->iobase + IDE_LBA_MID, (lba >> 8) & 0xFF);
    writeb(ctl->iobase + IDE_LBA_HIGH, (lba >> 16) & 0xFF);
    writeb(ctl->iobase + IDE_HDDEVSEL, disk->selector | ((lba >> 24) & 0xF));

    // send command: write
    writeb(ctl->iobase + IDE_COMMAND, IDE_CMD_WRITE);

    // write data
    for (i32 i = 0; i < count; i++) {
        disk_wait(ctl);
        for (i32 j = 0; j < 256; j++) {
            u16 data = ((u16*)buf)[i * 256 + j];
            writew(ctl->iobase + IDE_DATA, data);
        }
    }

    spin_lock_unlock(&ctl->slock);
    return 0;
}

static void ide_control_init() {
    void* buf = get_paddr_from_ppn(
        allocate_physical_page_for_kernel()
    );
    for (i32 i = 0; i < IDE_CTL_COUNT; i++) {
        ide_control_t* ctl = &ide_control[i];
        sprintf(ctl->name, "ide-%d", i);
        spin_lock_init(&ctl->slock);
        ctl->now_disk = NULL;
        ctl->iobase = i == 0 ? IDE_IOBASE_PRIMARY : IDE_IOBASE_SECONDARY;
        ctl->control_byte = readb(ctl->iobase + IDE_CONTROL);

        for (i32 j = 0; j < IDE_DISK_COUNT; j++) {
            ide_disk* disk = &ctl->disks[j];
            sprintf(disk->name, "ide-%d-%d", i, j);
            disk->ctl = ctl;
            disk->selector = j == 0 ? IDE_LBA_MASTER : IDE_LBA_SLAVE;
            disk->is_master = j == 0;

            // send command: identify
            writeb(ctl->iobase + IDE_HDDEVSEL, disk->selector);
            ctl->now_disk = disk;

            writeb(ctl->iobase + IDE_COMMAND, IDE_CMD_IDENTIFY);
            memfree(buf, PAGE_SIZE);
            ide_params* params = (ide_params*)buf;
            for (i32 k = 0; k < 256; k++) {
                u16 data = readw(ctl->iobase + IDE_DATA);
                ((u16*)buf)[k] = data;
            }

            // check if disk exists
            if (params->total_sector == 0) {
                continue;
            }

            disk->total_sector = params->total_sector;
            disk->c = params->cylinders;
            disk->h = params->heads;
            disk->s = params->sectors;
            info("find disk: %s, total_sector: %d, c: %d, h: %d, s: %d",
                disk->name, disk->total_sector, disk->c, disk->h, disk->s);
            
            install_device(DEV_BLOCK, DEV_DISK, disk, disk->name, 0, NULL, ide_pio_read, ide_pio_write);
        }
    }
    free_physical_page(get_ppn_from_paddr_floor(buf));
}

void ide_init() {
    device_init();
    ide_control_init();
}