//
// Created by quals on 2018/12/11.
//

#include "DiskManager.h"

int DiskManager::GetInodeOffset(int n) {
    return inode_offset_ + (n - 1) * inode_size_;
}

int DiskManager::GetBlockOffset(int n) {
    return block_offset_ + (n - 1) * block_size_;
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
