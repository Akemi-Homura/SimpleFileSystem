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

class DiskManager : public DiskPart {
public:
    void ReadFromDisk() override;

    void WriteToDisk() override;

    DiskManager(const char *path, int offset);

    DiskManager(const DiskManager& rhs);

    ~DiskManager();

    void Format(int block_size);

    Super super();

    Bitmap & block_bitmap();

    Bitmap & inode_bitmap();

    Inode * inode(int n);

    Block * block(int n);

private:
    Super* super_;
    Bitmap* block_bitmap_;
    Bitmap* inode_bitmap_;
    int inode_offset_;
    int block_offset_;
    int inode_size_;
};


#endif //SIMPLEFILESYSTEM_DISKMANAGER_H
