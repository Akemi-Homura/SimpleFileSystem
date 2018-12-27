//
// Created by quals on 2018/12/11.
//

#include "Block.h"
#include <cstring>
#include "src/file_operation_helper/FileOperationHelper.h"

void Block::ReadFromDisk() {
    oread(path_, data_, size_, offset_);
}

void Block::WriteToDisk() {
    owrite(path_, data_, size_, offset_);
}

Block::Block(const char *path, int offset, int size) : DiskPart(path, offset, size) {
    data_ = new unsigned char[size];
}

Block::~Block() {
    delete[] data_;
}

Block::Block(const Block &rhs) : Block(rhs.path_, rhs.offset_, rhs.size_) {
    memcpy(data_, rhs.data_, (size_t)size_);
}

void Block::Initialize() {
    memset(data_, 0, (size_t)size_);
    WriteToDisk();
}
