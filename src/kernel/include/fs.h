#ifndef __FS_H__
#define __FS_H__

#include "type.h"
#include "device.h"
#include "memory.h"

typedef struct bit_map {
    u32 blocks;
    u32 start_block_id;
} bit_map;

bit_map* new_bit_map(u32 start_block_id, u32 blocks);

typedef struct super_block {
    u32 magic;
    u32 total_blocks;
    u32 inode_bitmap_blocks;
    u32 inode_area_blocks;
    u32 data_bitmap_blocks;
    u32 data_area_blocks;
} super_block;

typedef enum file_type {
    FILE_TYPE_FILE,
    FILE_TYPE_DIR,
    FILE_TYPE_DEV,
} file_type;

typedef struct disk_inode {
    u32 size;
    file_type type;
    u32 ref_count;
    u32 direct[27];
    u32 indirect1;
    u32 indirect2;
} disk_inode;

disk_inode* new_empty_disk_inode();

typedef struct dir_entry {
    char name[28];
    u32 inode_id;
} dir_entry;

typedef struct yan_fs {
    device* dev; // should be disk ide-0-1
    bit_map* inode_bitmap;
    bit_map* data_bitmap;
    u32 inode_area_start_block_id;
    u32 data_area_start_block_id;
} yan_fs;

yan_fs* create(device* dev, u32 total_blocks, u32 inode_bitmap_blocks);
u32 allocate_one_inode(yan_fs* fs);
void free_one_inode(yan_fs* fs, u32 inode_id);
u32 allocate_one_data_block(yan_fs* fs);
void free_one_data_block(yan_fs* fs, u32 block_id);

i32 write(u32 fd, char* buf, u32 len);

void fs_init();
#endif