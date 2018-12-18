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

class DiskManager {
public:

    void Format(int block_size);

    void InitializeFileSystem(const char *path);

    void ReadFileSystemFromDisk(const char *path, int offset = 0);

    int GetInodeOffset(int n);

    int GetBlockOffset(int n);

    int GetBlockNum() const;

    int GetBlockSize() const;

    int GetInodeNum() const;

    int GetSuperOffset() const;

    int GetInodeBitmapOffset() const;

    int GetBlockBitmapOffset() const;

    int GetInodeSize() const;

    static DiskManager *GetInstance();

private:
    DiskManager() = default;

    static DiskManager *sInstance;
    int block_num_;
    int block_size_;
    int inode_num_;
    int super_offset_;
    int inode_bitmap_offset_;
    int block_bitmap_offset_;
    int inode_offset_;
    int block_offset_;
    int inode_size_;
};

#endif //SIMPLEFILESYSTEM_DISKMANAGER_H
