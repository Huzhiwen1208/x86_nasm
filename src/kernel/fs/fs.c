#include "../include/fs.h"
#include "../include/memory.h"
#include "../include/constant.h"
#include "../include/ide.h"
#include "../include/device.h"
#include "../include/lock.h"
#include "../include/log.h"

yan_fs* fs;

bit_map* new_bit_map(u32 start_block_id, u32 blocks) {
    bit_map* bm = (bit_map*)buddy_alloc(sizeof(bit_map));
    bm->blocks = blocks;
    bm->start_block_id = start_block_id;


    return bm;
}

disk_inode* new_empty_disk_inode() {
    disk_inode* inode = (disk_inode*)buddy_alloc(sizeof(disk_inode));
    inode->size = 0;
    inode->type = FILE_TYPE_FILE;
    inode->ref_count = 0;
    for (int i = 0; i < 27; i++) {
        inode->direct[i] = 0;
    }
    inode->indirect1 = 0;
    inode->indirect2 = 0;
    return inode;
}

u32 allocate_one_inode(yan_fs* fs) {
    u8* buf = (u8*)buddy_alloc(SECTOR_BYTE_SIZE);

    for (i32 block_offset = 0; block_offset < fs->inode_bitmap->blocks; block_offset++) {
        u32 block_id = fs->inode_bitmap->start_block_id + block_offset;
        fs->dev->read(fs->dev->ptr, buf, 1, block_id);
        for (i32 byte = 0; byte < SECTOR_BYTE_SIZE; byte++) {
            u8 byte_value = buf[byte];
            for (i32 bit = 0; bit < 8; bit++) {
                if ((byte_value & (1 << bit)) == 0) {
                    buf[byte] |= (1 << bit);
                    fs->dev->write(fs->dev->ptr, buf, 1, block_id);
                    u32 inode_id = block_offset * SECTOR_BYTE_SIZE * 8 + byte * 8 + bit;
                    buddy_free(buf);
                    return inode_id;
                }
            }
        }
    }
    panic("no free inode");
}

void free_one_inode(yan_fs* fs, u32 inode_id) {
    u32 block_offset = inode_id / (SECTOR_BYTE_SIZE * 8);
    u32 byte = (inode_id % (SECTOR_BYTE_SIZE * 8)) / 8;
    u32 bit = inode_id % 8;

    u32 block_id = fs->inode_bitmap->start_block_id + block_offset;
    u8* buf = (u8*)buddy_alloc(SECTOR_BYTE_SIZE);
    fs->dev->read(fs->dev->ptr, buf, 1, block_id);
    buf[byte] &= ~(1 << bit);
    fs->dev->write(fs->dev->ptr, buf, 1, block_id);
    buddy_free(buf);
}

disk_inode* get_disk_inode_by_inode_id(yan_fs* fs, u32 inode_id) {
    disk_inode* inode = (disk_inode*)buddy_alloc(SECTOR_BYTE_SIZE);
    u32 inode_block_id = fs->inode_area_start_block_id + inode_id / INODE_PER_BLOCK;
    u32 inode_offset = inode_id % INODE_PER_BLOCK;
    fs->dev->read(fs->dev->ptr, inode, 1, inode_block_id);
    return inode + inode_offset;
}

void write_disk_inode_by_inode_id(yan_fs* fs, u32 inode_id, disk_inode* d_inode) {
    disk_inode* inode = (disk_inode*)buddy_alloc(SECTOR_BYTE_SIZE);
    u32 inode_block_id = fs->inode_area_start_block_id + inode_id / INODE_PER_BLOCK;
    u32 inode_offset = inode_id % INODE_PER_BLOCK;
    fs->dev->read(fs->dev->ptr, inode, 1, inode_block_id);
    inode[inode_offset] = *d_inode;
    fs->dev->write(fs->dev->ptr, inode, 1, inode_block_id);
    buddy_free(inode);
}

u32 allocate_one_data_block(yan_fs* fs) {
    u8* buf = (u8*)buddy_alloc(SECTOR_BYTE_SIZE);

    for (i32 block_offset = 0; block_offset < fs->data_bitmap->blocks; block_offset++) {
        u32 block_id = fs->data_bitmap->start_block_id + block_offset;
        fs->dev->read(fs->dev->ptr, buf, 1, block_id);
        for (i32 byte = 0; byte < SECTOR_BYTE_SIZE; byte++) {
            u8 byte_value = buf[byte];
            for (i32 bit = 0; bit < 8; bit++) {
                if ((byte_value & (1 << bit)) == 0) {
                    buf[byte] |= (1 << bit);
                    fs->dev->write(fs->dev->ptr, buf, 1, block_id);
                    u32 block_id = block_offset * SECTOR_BYTE_SIZE * 8 + byte * 8 + bit + fs->data_area_start_block_id;
                    buddy_free(buf);
                    return block_id;
                }
            }
        }
    }
    panic("no free data block");
}

void free_one_data_block(yan_fs* fs, u32 block_id) {
    block_id -= fs->data_area_start_block_id;
    u32 block_offset = block_id / (SECTOR_BYTE_SIZE * 8);
    u32 byte = (block_id % (SECTOR_BYTE_SIZE * 8)) / 8;
    u32 bit = block_id % 8;

    u8* buf = (u8*)buddy_alloc(SECTOR_BYTE_SIZE);
    fs->dev->read(fs->dev->ptr, buf, 1, block_id);
    buf[byte] &= ~(1 << bit);
    fs->dev->write(fs->dev->ptr, buf, 1, block_id);
    buddy_free(buf);
}

dir_entry* find_dir_entry_by_name(char* name) {
    disk_inode* inode = get_disk_inode_by_inode_id(fs, 0);
    if (inode->type != FILE_TYPE_DIR) {
        panic("not a directory");
    }
    u32 file_size = inode->size;
    if (file_size == 0) {
        return NULL;
    }

    u32 blocks = (file_size + SECTOR_BYTE_SIZE - 1) / SECTOR_BYTE_SIZE;
    for (i32 i = 0; i < 27; i++) {
        u32 block_id = inode->direct[i];
        if (block_id == 0) {
            return NULL;
        }
        u8* buf = (u8*)buddy_alloc(SECTOR_BYTE_SIZE);
        fs->dev->read(fs->dev->ptr, buf, 1, block_id);
        for (i32 j = 0; j < SECTOR_BYTE_SIZE; j += sizeof(dir_entry)) {
            dir_entry* entry = (dir_entry*)(buf + j);
            if (strcmp(entry->name, name)) {
                buddy_free(buf);
                return entry;
            }
        }
        buddy_free(buf);
    }

    // indirect1
    u32 block_id = inode->indirect1;
    if (block_id == 0) {
        return NULL;
    }

    u8* buf = (u8*)buddy_alloc(SECTOR_BYTE_SIZE);
    fs->dev->read(fs->dev->ptr, buf, 1, block_id);
    for (i32 i = 0; i < SECTOR_BYTE_SIZE; i += sizeof(u32)) {
        u32 block_id = *((u32*)(buf + i));
        if (block_id == 0) {
            buddy_free(buf);
            return NULL;
        }
        u8* buf = (u8*)buddy_alloc(SECTOR_BYTE_SIZE);
        fs->dev->read(fs->dev->ptr, buf, 1, block_id);
        for (i32 j = 0; j < SECTOR_BYTE_SIZE; j += sizeof(dir_entry)) {
            dir_entry* entry = (dir_entry*)(buf + j);
            if (strcmp(entry->name, name) == 0) {
                buddy_free(buf);
                return entry;
            }
        }
        buddy_free(buf);
    }

    // indirect2
    block_id = inode->indirect2;
    if (block_id == 0) {
        return NULL;
    }

    fs->dev->read(fs->dev->ptr, buf, 1, block_id);
    for (i32 i = 0; i < SECTOR_BYTE_SIZE; i += sizeof(u32)) {
        u32 block_id = *((u32*)(buf + i));
        if (block_id == 0) {
            buddy_free(buf);
            return NULL;
        }
        u8* buf = (u8*)buddy_alloc(SECTOR_BYTE_SIZE);
        fs->dev->read(fs->dev->ptr, buf, 1, block_id);
        for (i32 j = 0; j < SECTOR_BYTE_SIZE; j += sizeof(u32)) {
            u32 block_id = *((u32*)(buf + j));
            if (block_id == 0) {
                buddy_free(buf);
                return NULL;
            }
            u8* buf = (u8*)buddy_alloc(SECTOR_BYTE_SIZE);
            fs->dev->read(fs->dev->ptr, buf, 1, block_id);
            for (i32 k = 0; k < SECTOR_BYTE_SIZE; k += sizeof(dir_entry)) {
                dir_entry* entry = (dir_entry*)(buf + k);
                if (strcmp(entry->name, name) == 0) {
                    buddy_free(buf);
                    return entry;
                }
            }
            buddy_free(buf);
        }
        buddy_free(buf);
    }
    buddy_free(inode);
    return NULL;
}

dir_entry* new_dir_entry(char* name, u32 inode_id) {
    dir_entry* entry = (dir_entry*)buddy_alloc(sizeof(dir_entry));
    strcpy(entry->name, name);
    entry->inode_id = inode_id;
    return entry;
}

void append_dir_entry(dir_entry* entry) {
    disk_inode* inode = get_disk_inode_by_inode_id(fs, 0);
    if (inode->type != FILE_TYPE_DIR) {
        panic("not a directory");
    }
    u32 file_size = inode->size;
    u32 blocks = (file_size + SECTOR_BYTE_SIZE - 1) / SECTOR_BYTE_SIZE;
    u32 offset = file_size % SECTOR_BYTE_SIZE / sizeof(dir_entry);
    inode->size += sizeof(dir_entry);

    if (blocks == 0) {
        inode->direct[0] = allocate_one_data_block(fs);
        u8* buf = (u8*)buddy_alloc(SECTOR_BYTE_SIZE);
        dir_entry* empty_entry = (dir_entry*)(buf + offset * sizeof(dir_entry));
        *empty_entry = *entry;
        fs->dev->write(fs->dev->ptr, buf, 1, inode->direct[0]);
        buddy_free(buf);
        write_disk_inode_by_inode_id(fs, 0, inode);
        return;
    }

    // direct
    if (blocks <= 27) {
        u32 block_id = inode->direct[blocks - 1];
        u8* buf = (u8*)buddy_alloc(SECTOR_BYTE_SIZE);
        fs->dev->read(fs->dev->ptr, buf, 1, block_id);
        dir_entry* empty_entry = (dir_entry*)(buf + offset * sizeof(dir_entry));
        *empty_entry = *entry;
        fs->dev->write(fs->dev->ptr, buf, 1, block_id);
        buddy_free(buf);
    }

    // TODO: indirect
}

void create_file(char* name) {
    dir_entry* entry = find_dir_entry_by_name(name);
    if (entry != NULL) {
        error("file already exists");
        return;
    }

    disk_inode* inode = new_empty_disk_inode();
    inode->ref_count = 1;
    u32 inode_id = allocate_one_inode(fs);
    write_disk_inode_by_inode_id(fs, inode_id, inode);

    dir_entry* new_entry = new_dir_entry(name, inode_id);
    append_dir_entry(new_entry);
    buddy_free(new_entry);
}

yan_fs* create(device* dev, u32 total_blocks, u32 inode_bitmap_blocks) {
    yan_fs* fs = (yan_fs*)buddy_alloc(sizeof(yan_fs));
    fs->dev = dev;

    // 1. create super block
    super_block* sb = (super_block*)get_paddr_from_ppn(
        allocate_physical_page_for_kernel()
    );
    sb->magic = FS_MAGIC;
    sb->total_blocks = total_blocks;

    // 2. create inode bitmap
    sb->inode_bitmap_blocks = inode_bitmap_blocks;
    bit_map* inode_bitmap = new_bit_map(1, inode_bitmap_blocks);
    u32 inode_count = inode_bitmap_blocks * SECTOR_BYTE_SIZE * 8;
    u32 inode_area_blocks = inode_count / INODE_PER_BLOCK;
    sb->inode_area_blocks = inode_area_blocks;

    fs->inode_bitmap = inode_bitmap;
    fs->inode_area_start_block_id = inode_bitmap_blocks + 1;

    // 3. create data bitmap
    u32 remain_blocks = total_blocks - inode_bitmap_blocks - inode_area_blocks - 1;
    u32 data_bitmap_blocks = remain_blocks / (SECTOR_BYTE_SIZE / 8 + 1);
    sb->data_bitmap_blocks = data_bitmap_blocks;
    bit_map* data_bitmap = new_bit_map(inode_bitmap_blocks + inode_area_blocks + 1, data_bitmap_blocks);
    u32 data_area_blocks = remain_blocks - data_bitmap_blocks;
    sb->data_area_blocks = data_area_blocks;

    fs->data_bitmap = data_bitmap;
    fs->data_area_start_block_id = inode_bitmap_blocks + inode_area_blocks + data_bitmap_blocks + 1;

    // 4. write super block
    dev->write(dev->ptr, sb, 1, 0);
    // 6. create root directory
    u32 root_inode_id = allocate_one_inode(fs);
    disk_inode* root_inode = new_empty_disk_inode();
    root_inode->type = FILE_TYPE_DIR;
    root_inode->ref_count = 1;
    write_disk_inode_by_inode_id(fs, root_inode_id, root_inode);
    buddy_free(root_inode);

    // 7. free memory
    free_physical_page(get_ppn_from_paddr_floor((u32)sb));

    fs->slock = (spin_lock*)buddy_alloc(sizeof(spin_lock));
    return fs;
}

void fs_init() {
    device* dev = find_device_by_name("ide-0-1");
    fs = create(dev, 64000, 5);
    spin_lock_init(fs->slock);
}