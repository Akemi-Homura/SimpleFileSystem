//
// Created by quals on 2018/12/11.
//

#include "DiskManager.h"

void DiskManager:: ReadFromDisk() {
    if (super_ == nullptr) {
        super_ = new Super(path_, offset_);
    }
    super_->ReadFromDisk();

    if (inode_bitmap_ == nullptr) {
        inode_bitmap_ = new Bitmap(path_, super_->offset() + super_->size(), super_->BlockSize());
    }
    inode_bitmap_->ReadFromDisk();

    if (block_bitmap_ == nullptr) {
        block_bitmap_ = new Bitmap(path_, inode_bitmap_->offset() + inode_bitmap_->size(), super_->BlockSize());
    }
    block_bitmap_->ReadFromDisk();


    int block_bits = super_->BlockSize() * 8;
    inode_offset_ = block_bitmap_->offset() + block_bitmap_->size();
    block_offset_ = inode_offset_ + block_bits * inode_size_;
}

void DiskManager::WriteToDisk() {
    super_->WriteToDisk();
    inode_bitmap_->WriteToDisk();
    block_bitmap_->WriteToDisk();
}

DiskManager::DiskManager(const char *path, int offset) : DiskPart(path, offset, 0) {
    super_ = nullptr;
    block_bitmap_ = inode_bitmap_ = nullptr;
    inode_size_ = sizeof(Inode::Model);
}

void DiskManager::Format(int block_size) {
    int block_bits = block_size * 8;

    if (super_ == nullptr) {
        super_ = new Super(path_, offset_);
    }
    super_->SetBlockNum(block_bits);
    super_->SetInodeNum(block_bits);
    super_->SetBlockSize(block_size);

    if (inode_bitmap_ == nullptr) {
        inode_bitmap_ = new Bitmap(path_, super_->offset() + super_->size(), block_size);
    }
    inode_bitmap_->zero();

    if (block_bitmap_ == nullptr) {
        block_bitmap_ = new Bitmap(path_, inode_bitmap_->offset() + inode_bitmap_->size(), block_size);
    }
    block_bitmap_->zero();

    inode_offset_ = block_bitmap_->offset() + block_bitmap_->size();
    block_offset_ = inode_offset_ + block_bits * inode_size_;
}

Super DiskManager::super() {
    return *super_;
}

Bitmap & DiskManager::block_bitmap() {
    return *block_bitmap_;
}

Bitmap & DiskManager::inode_bitmap() {
    return *inode_bitmap_;
}

Inode * DiskManager::inode(int n) {
    auto res = new Inode(path_, inode_offset_ + (n - 1) * inode_size_) ;
    res->ReadFromDisk();

    return res;
}

Block * DiskManager::block(int n) {
    auto res = new Block(path_, block_offset_ + (n - 1) * super_->BlockSize(), super_->BlockSize());
    res->ReadFromDisk();

    return res;
}

DiskManager::DiskManager(const DiskManager &rhs) : DiskPart(rhs.path_, rhs.offset_, rhs.size_) {
    delete super_;
    delete block_bitmap_;
    delete inode_bitmap_;

    super_ = new Super(*rhs.super_);
    block_bitmap_ = new Bitmap(*rhs.block_bitmap_);
    inode_bitmap_ = new Bitmap(*rhs.inode_bitmap_);
}

DiskManager::~DiskManager() {
    delete super_;
    delete block_bitmap_;
    delete inode_bitmap_;
}

