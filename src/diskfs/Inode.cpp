//
// Created by quals on 2018/12/11.
//

#include "Inode.h"
#include "src/file_operation_helper/FileOperationHelper.h"

void Inode::ReadFromDisk() {
    oread(path_, &model_, size_, offset_);
}

void Inode::WriteToDisk() {
    owrite(path_, &model_, size_, offset_);
}

Inode::Inode(const char *path, int offset) : DiskPart(path, offset, sizeof(Model)) {}
