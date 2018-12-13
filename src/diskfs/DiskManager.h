//
// Created by quals on 2018/12/11.
//

#ifndef SIMPLEFILESYSTEM_DISKMANAGER_H
#define SIMPLEFILESYSTEM_DISKMANAGER_H

#include "DiskPart.h"
#include "Super.h"
#include "Bitmap.h"
#include "Inode.h"
#include "Block.h"

#ifdef DEBUG_MODE_TESTING

#include "gtest/gtest_prod.h"

#endif

class DiskManager : public DiskPart {
public:
    void ReadFromDisk() override;

    void WriteToDisk() override;

    DiskManager(const char *path, int offset);

    DiskManager(const DiskManager &rhs);

    ~DiskManager();

    void Format(int block_size);

    Inode inode(int n);

    Block block(int n);

private:
    Super *super_;
    Bitmap *block_bitmap_;
    Bitmap *inode_bitmap_;
    int inode_offset_;
    int block_offset_;
    int inode_size_;
#ifdef DEBUG_MODE_TESTING

    FRIEND_TEST(DiskManagerFixture, DiskManagerTest);

#endif
};


#endif //SIMPLEFILESYSTEM_DISKMANAGER_H
