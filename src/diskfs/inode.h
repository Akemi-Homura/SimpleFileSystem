//
// Created by quals on 2018/12/11.
//

#ifndef SIMPLEFILESYSTEM_INODE_H
#define SIMPLEFILESYSTEM_INODE_H
/**
 * inode节点
 */
#include "block.h"

#define _FILE_NAME_MAXSIZE 255

struct inode {
    char name[_FILE_NAME_MAXSIZE];
    char type;
    int flag;
    int size;
    struct block direct_block[10];
    struct block indirect_block_1[2];
    struct block indirect_block_2;
};
#endif //SIMPLEFILESYSTEM_INODE_H
