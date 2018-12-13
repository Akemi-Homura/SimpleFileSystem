//
// Created by quals on 2018/12/11.
//

#include "Super.h"
#include <src/file_operation_helper/FileOperationHelper.h>

void Super::ReadFromDisk() {
    oread(path_, &model_, size_, offset_);
}

void Super::WriteToDisk() {
    owrite(path_, &model_, size_, offset_);
}

Super::Super(const char *path, int offset) : DiskPart(path, offset, sizeof(Model)) {}

int Super::BlockNum() {
    return model_.block_num_;
}

void Super::SetBlockNum(int num) {
    model_.block_num_ = num;
}

int Super::BlockSize() {
    return model_.block_size_;
}

void Super::SetBlockSize(int size) {
    model_.block_size_ = size;
}

int Super::InodeNum() {
    return model_.inode_num_;
}

void Super::SetInodeNum(int num) {
    model_.inode_num_ = num;
}
