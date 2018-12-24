//
// Created by quals on 2018/12/11.
//

#ifndef SIMPLEFILESYSTEM_INODE_H
#define SIMPLEFILESYSTEM_INODE_H

#include <string>
#include "DiskPart.h"

#ifdef DEBUG_MODE_TESTING

#include <gtest/gtest_prod.h>

#endif

class DiskManager;

class Inode : public DiskPart {
public:
    static const int DIRECT_BLOCK_NUM = 12;
    static const int BLOCK_INDEX_SIZE = sizeof(int);
    enum FileType {
        kDirectory,
        kNormal
    };

    struct Model {
        char name_[255];
        FileType type_;
        int size_;
        int direct_block_[DIRECT_BLOCK_NUM];
        int indirect_block_2_;
        int indirect_block_3_;
    };

    void ReadFromDisk() override;

    void WriteToDisk() override;

    Inode(const char *path, int offset);

    Inode() = delete;

    void Initialize(const char *name, FileType type);

    void Trunc();

    int WriteData2Block(const void *buf, int size, int offset = 0);

    int ReadDataFromBlock(void *buf, int size, int offset = 0);

    Model model_;
private:
    DiskManager *manager;

    int GetBlockIndex(int block_num);

#ifdef DEBUG_MODE_TESTING

    FRIEND_TEST(InterfaceFixture, InodeTest);

#endif
};


#endif
