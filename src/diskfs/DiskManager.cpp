//
// Created by quals on 2018/12/11.
//

#include <src/interface/MDir.h>
#include <stdexcept>
#include <string.h>
#include "DiskManager.h"

int DiskManager::GetInodeOffset(int n) {
    return inode_offset_ + n * inode_size_;
}

int DiskManager::GetBlockOffset(int n) {
    return block_offset_ + n * block_size_;
}

int DiskManager::GetBlockNum() const {
    return block_num_;
}

int DiskManager::GetBlockSize() const {
    return block_size_;
}

int DiskManager::GetInodeNum() const {
    return inode_num_;
}

int DiskManager::GetSuperOffset() const {
    return super_offset_;
}

int DiskManager::GetInodeBitmapOffset() const {
    return inode_bitmap_offset_;
}

int DiskManager::GetBlockBitmapOffset() const {
    return block_bitmap_offset_;
}

int DiskManager::GetInodeSize() const {
    return inode_size_;
}

void DiskManager::Format(int block_size) {
    int block_bits = block_size * 8;

    block_num_ = block_bits;
    inode_num_ = block_bits;
    block_size_ = block_size;
    inode_size_ = sizeof(Inode::Model);
    super_offset_ = 0;
    inode_bitmap_offset_ = super_offset_ + sizeof(Super::Model);
    block_bitmap_offset_ = inode_bitmap_offset_ + block_size;
    inode_offset_ = block_bitmap_offset_ + block_size;
    block_offset_ = inode_offset_ + inode_num_ * inode_size_;
}


void DiskManager::InitializeFileSystem(const char *path) {
    Super super(path, GetSuperOffset());
    super.SetBlockNum(GetBlockNum());
    super.SetInodeNum(GetInodeNum());
    super.SetBlockSize(GetBlockSize());
    super.WriteToDisk();

    Bitmap inode_bitmap(path, GetInodeBitmapOffset(), GetBlockSize());
    inode_bitmap.zero();
    inode_bitmap.WriteToDisk();

    Bitmap block_bitmap(path, GetBlockBitmapOffset(), GetBlockSize());
    block_bitmap.zero();
    block_bitmap.WriteToDisk();

    OccupyInode(path, MDir::ROOT_DIR_INODE_NUM);
    MDir root(path, MDir::ROOT_DIR_INODE_NUM);
    root.Initialize(MDir::ROOT_DIR_INODE_NUM, MDir::ROOT_DIR_NAME);
}

void DiskManager::ReadFileSystemFromDisk(const char *path, int offset) {
    Super super(path, offset);
    super.ReadFromDisk();

    Format(super.BlockSize());
}

DiskManager *DiskManager::sInstance = nullptr;

DiskManager *DiskManager::GetInstance() {
    if (sInstance == nullptr) {
        sInstance = new DiskManager;
    }
    return sInstance;
}

/**
 * 根据文件路径查找inode节点编号，没有的话返回-1，步骤如下：
 *
 * @param path 文件路径
 * @return inode节点编号
 */
int DiskManager::namei(const char *path, const char *filepath) {
    const char *delim = "/";
    int inode_num = MDir::ROOT_DIR_INODE_NUM;

    if (filepath[0] != delim[0]) {
        char msg[40];
        sprintf(msg, "file path don't start with %s : %s", delim, filepath);
        throw std::runtime_error(msg);
    }

    MDir root(path, inode_num);
    auto items = root.GetDirItems();
    for (int i = 2; i < items.size(); i++) {
        if (strcmp(items[i].model_.name_, filepath + 1) == 0) {
            return items[i].model_.inode_num_;
        }
    }

    return -1;
}

int DiskManager::GetFreeInodeNum(const char *path) {
    Bitmap inode_bitmap(path, inode_bitmap_offset_, block_size_);
    inode_bitmap.ReadFromDisk();

    for (int i = 0; i < inode_bitmap.len(); i++) {
        if (!inode_bitmap[i]) {
            return i;
        }
    }
    return -1;
}

int DiskManager::GetFreeBlockNum(const char *path) {
    Bitmap block_bitmap(path, block_bitmap_offset_, block_size_);
    block_bitmap.ReadFromDisk();
    for (int i = 0; i < block_bitmap.len(); i++) {
        if (!block_bitmap[i]) {
            return i;
        }
    }
    return -1;
}

void DiskManager::OccupyInode(const char *path, int n) {
    Bitmap inode_bitmap(path, inode_bitmap_offset_, block_size_);
    inode_bitmap.ReadFromDisk();
    inode_bitmap[n] = true;
    inode_bitmap.WriteToDisk();
}

void DiskManager::ReleaseInode(const char *path, int n) {
    Bitmap inode_bitmap(path, inode_bitmap_offset_, block_size_);
    inode_bitmap.ReadFromDisk();
    inode_bitmap[n] = false;
    inode_bitmap.WriteToDisk();
}

void DiskManager::OccupyBlock(const char *path, int n) {
    Bitmap block_bitmap(path, block_bitmap_offset_, block_size_);
    block_bitmap.ReadFromDisk();
    block_bitmap[n] = true;
    block_bitmap.WriteToDisk();
}


void DiskManager::ReleaseBlock(const char *path, int n) {
    Bitmap block_bitmap(path, block_bitmap_offset_, block_size_);
    block_bitmap.ReadFromDisk();
    block_bitmap[n] = false;
    block_bitmap.WriteToDisk();
}

int DiskManager::MallocBlock(const char *path) {
    int free_block_num = GetFreeBlockNum(path);
    OccupyBlock(path, free_block_num);
    return free_block_num;
}

int DiskManager::MallocInode(const char *path) {
    int free_inode_num = GetFreeInodeNum(path);
    OccupyInode(path, free_inode_num);
    return free_inode_num;
}

bool DiskManager::IsBlockFree(const char *path, int n) {
    Bitmap block_bitmap(path, block_bitmap_offset_, block_size_);
    block_bitmap.ReadFromDisk();
    return !block_bitmap[n];
}


