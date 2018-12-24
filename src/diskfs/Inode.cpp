//
// Created by quals on 2018/12/11.
//

#include "Inode.h"
#include "src/file_operation_helper/FileOperationHelper.h"
#include <algorithm>
#include <cstring>
#include "DiskManager.h"

void Inode::ReadFromDisk() {
    oread(path_, &model_, size_, offset_);
}

void Inode::WriteToDisk() {
    owrite(path_, &model_, size_, offset_);
}

Inode::Inode(const char *path, int offset) : DiskPart(path, offset, sizeof(Model)) {
    manager = DiskManager::GetInstance();
}

int Inode::WriteData2Block(const void *buf, int size, int offset) {
    static const int block_size = manager->GetBlockSize();
    int block_num = offset / block_size;
    int block_offset = offset - block_num * block_size;
    int already_write_size = 0;
    model_.size_ = offset + size;

    while (size > 0) {
        int index = GetBlockIndex(block_num);
        int size_need_write = std::min(block_size - block_offset, size);
        Block block(path_, manager->GetBlockOffset(index), block_size);
        block.ReadFromDisk();

        std::memcpy(block.data_ + block_offset, (char *) buf + already_write_size, (size_t) size_need_write);
        block.WriteToDisk();

        already_write_size += size_need_write;
        size -= size_need_write;
        block_num++;
        block_offset = 0;
    }
    WriteToDisk();
    return already_write_size;
}

int Inode::ReadDataFromBlock(void *buf, int size, int offset) {
    static const int block_size = manager->GetBlockSize();
    int block_num = offset / block_size;
    int block_offset = offset - block_num * block_size;
    int already_read_size = 0;
    size = std::min(size, model_.size_ - offset);

    while (size > 0) {
        int index = GetBlockIndex(block_num);
        int size_need_read = std::min(block_size - block_offset, size);

        Block block(path_, manager->GetBlockOffset(index), block_size);
        block.ReadFromDisk();

        std::memcpy((char *) buf + already_read_size, block.data_ + block_offset, (size_t) size_need_read);

        already_read_size += size_need_read;
        size -= size_need_read;

        block_num++;
        block_offset = 0;
    }
    return already_read_size;
}

int Inode::GetBlockIndex(int block_num) {
    static const int block_size = manager->GetBlockSize();
    static const int sec_indirect_block_maxnum = DIRECT_BLOCK_NUM + block_size / BLOCK_INDEX_SIZE;
    static const int block_num_in_sec_indirect_block = block_size / BLOCK_INDEX_SIZE;

    if (block_num < DIRECT_BLOCK_NUM) {
        int &res = model_.direct_block_[block_num];
        if (res == -1) {
            res = manager->MallocBlock(path_);
        }
        return res;
    } else if (block_num < sec_indirect_block_maxnum) {
        bool sec_indirect_block_init_flag = false;
        if (model_.indirect_block_2_ == -1) {
            model_.indirect_block_2_ = manager->MallocBlock(path_);
            sec_indirect_block_init_flag = true;
        }

        Block sec_indirect_block(path_, manager->GetBlockOffset(model_.indirect_block_2_), block_size);

        if (sec_indirect_block_init_flag) {
            sec_indirect_block.Initialize();
        } else {
            sec_indirect_block.ReadFromDisk();
        }

        int block_index_offset = (block_num - DIRECT_BLOCK_NUM) * BLOCK_INDEX_SIZE;
        int &res = *(int *) (sec_indirect_block.data_ + block_index_offset);
        if (res == 0) {
            res = manager->MallocBlock(path_);
            sec_indirect_block.WriteToDisk();
        }
        return res;
    } else {
        bool third_indirect_block_init_flag = false;
        if (model_.indirect_block_3_ == -1) {
            third_indirect_block_init_flag = true;
            model_.indirect_block_3_ = manager->MallocBlock(path_);
        }
        Block third_indirect_block(path_, manager->GetBlockOffset(model_.indirect_block_3_), block_size);
        if (third_indirect_block_init_flag) {
            third_indirect_block.Initialize();
        } else {
            third_indirect_block.ReadFromDisk();
        }

        bool sec_indirect_block_init_flag = false;
        int sec_indirect_block_index_offset =
                (block_num - sec_indirect_block_maxnum) / (block_num_in_sec_indirect_block) * BLOCK_INDEX_SIZE;
        int &sec_indirect_block_index = *(int *) (third_indirect_block.data_ + sec_indirect_block_index_offset);

        if (sec_indirect_block_index == 0) {
            sec_indirect_block_index = manager->MallocBlock(path_);
            sec_indirect_block_init_flag = true;
            third_indirect_block.WriteToDisk();
        }

        Block sec_indirect_block(path_, manager->GetBlockOffset(sec_indirect_block_index), block_size);
        if (sec_indirect_block_init_flag) {
            sec_indirect_block.Initialize();
        } else {
            sec_indirect_block.ReadFromDisk();
        }

        int block_index_offset =
                ((block_num - sec_indirect_block_maxnum) % block_num_in_sec_indirect_block) * BLOCK_INDEX_SIZE;
        int &res = *(int *) (sec_indirect_block.data_ + block_index_offset);
        if (res == 0) {
            res = manager->MallocBlock(path_);
            sec_indirect_block.WriteToDisk();
        }
        return res;
    }
}

void Inode::Initialize(const char *name, Inode::FileType type) {
    std::strcpy(model_.name_, name);
    model_.size_ = 0;
    model_.type_ = type;
    for (int i = 0; i < DIRECT_BLOCK_NUM; i++) {
        model_.direct_block_[i] = -1;
    }
    model_.indirect_block_2_ = -1;
    model_.indirect_block_3_ = -1;
    WriteToDisk();
}

void Inode::Trunc() {
    model_.size_ = 0;
    for (int i = 0; i < DIRECT_BLOCK_NUM; i++) {
        model_.direct_block_[i] = -1;
    }
    model_.indirect_block_2_ = -1;
    model_.indirect_block_3_ = -1;
    WriteToDisk();
}