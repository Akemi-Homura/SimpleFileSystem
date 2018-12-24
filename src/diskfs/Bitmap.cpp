//
// Created by quals on 2018/12/11.
//

#include <cstdio>
#include <cstring>
#include <stdexcept>
#include "Bitmap.h"
#include "src/file_operation_helper/FileOperationHelper.h"

static const int BITLEN = 8;

void Bitmap::ReadFromDisk() {
    oread(path_, raw_data_, size_, offset_);
    for (int i = 0; i < size_; i++) {
        int st = i * BITLEN;
        for (int j = 0; j < BITLEN; j++) {
            int tmp = (1 << (BITLEN - 1 - j));
            data_[st + j] = (raw_data_[i] & tmp) == tmp;
        }
    }
}

void Bitmap::WriteToDisk() {
    memset(raw_data_, 0, size_);
    for (int i = 0; i < size_; i++) {
        int st = i * BITLEN;
        for (int j = 0; j < BITLEN; j++) {
            if (data_[st + j]) {
                raw_data_[i] |= 1 << (BITLEN - 1 - j);
            }
        }
    }
    owrite(path_, raw_data_, size_, offset_);
}

Bitmap::Bitmap(const char *path, int offset, int size) : DiskPart(path, offset, size) {
    len_ = size << 3;
    data_ = new bool[len_];
    raw_data_ = new char[size];
}

bool &Bitmap::operator[](int n) {
    CheckIndex(n);
    return data_[n];
}

const bool &Bitmap::operator[](int n) const {
    CheckIndex(n);
    return data_[n];
}

Bitmap::~Bitmap() {
    delete[] data_;
    delete[] raw_data_;
}

void Bitmap::CheckIndex(int n) const {
    static char msg[50];
    if (n > len_) {
        sprintf(msg, "%d over range of %d", n, len_);
        throw std::runtime_error(msg);
    }
}

void Bitmap::zero() {
    set(false);
}

void Bitmap::set(bool flag) {
    for (int i = 0; i < len_; i++) {
        data_[i] = flag;
    }
}

Bitmap::Bitmap(const Bitmap &rhs) : Bitmap(rhs.path_, rhs.offset_, rhs.size_) {
    memcpy(data_, rhs.data_, len_);
    memcpy(raw_data_, rhs.raw_data_, size_);
}

int Bitmap::len() {
    return len_;
}

