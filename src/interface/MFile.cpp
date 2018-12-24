//
// Created by quals on 2018/12/18.
//

#include <src/diskfs/Inode.h>
#include <src/diskfs/DiskManager.h>
#include <cstring>
#include "MFile.h"
#include "MDir.h"

int MFile::Read(void *buf, int size, int offset) {
    DiskManager *manager = DiskManager::GetInstance();
    Inode inode(path, manager->GetInodeOffset(inode_num_));
    inode.ReadFromDisk();
    return inode.ReadDataFromBlock(buf, size, offset);
}

MFile::MFile(const char *path, int inode_num_) : path(path), inode_num_(inode_num_) {
    manager = DiskManager::GetInstance();
}

int MFile::Write(const void *buf, int size, int offset) {
    DiskManager *manager = DiskManager::GetInstance();
    Inode inode(path, manager->GetInodeOffset(inode_num_));
    inode.ReadFromDisk();
    return inode.WriteData2Block(buf, size, offset);
}

/**
 * 创建文件，步骤如下：
 * 1. 初始化inode节点，写入磁盘
 * 2. 父目录增加一个目录项
 * @param fa_inode_num 父目录的inode节点编号
 */
void MFile::Initialize(int fa_inode_num, const char *filename) {
    // 初始化inode节点
    Inode inode(path, manager->GetInodeOffset(inode_num_));
    inode.Initialize(filename, Inode::kNormal);

    // 父目录增加目录项
    MDir fadir(path, fa_inode_num);
    fadir.Insert(filename, inode_num_);
}
