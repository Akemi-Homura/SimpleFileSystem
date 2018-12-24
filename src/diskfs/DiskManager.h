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

    int GetFreeInodeNum(const char *path);

    bool IsBlockFree(const char *path, int n);

    int GetFreeBlockNum(const char *path);

    void OccupyInode(const char *path, int n);

    int MallocBlock(const char*path);

    int MallocInode(const char* path);

    void ReleaseInode(const char *path, int n);

    void OccupyBlock(const char *path, int n);

    void ReleaseBlock(const char *path, int n);

    int namei(const char *path, const char *filepath);

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
    DiskManager(const DiskManager&);
    DiskManager&operator=(const DiskManager&);

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
