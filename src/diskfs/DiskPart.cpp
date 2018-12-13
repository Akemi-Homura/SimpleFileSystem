//
// Created by quals on 2018/12/12.
//
#include "DiskPart.h"

DiskPart::DiskPart(const char *filepath, int offset, int size) {
    this->path_ = filepath;
    this->offset_ = offset;
    this->size_ = size;
}

int DiskPart::size() {
    return size_;
}

int DiskPart::offset() {
    return offset_;
}