//
// Created by quals on 2018/12/11.
//

#ifndef SIMPLEFILESYSTEM_SUPER_H
#define SIMPLEFILESYSTEM_SUPER_H
/**
 * 超级块
 */
#include "block.h"
#include "inode.h"

struct super {
    int block_num;
    int inode_num;
    int block_size;
    int inode_size;
    struct block *first_block;
    struct inode *first_inode;
    int inode_table_offset;
    int data_block_offset;
    unsigned char *block_bitmap;
    unsigned char *inode_bitmap;
};
#endif //SIMPLEFILESYSTEM_SUPER_H
