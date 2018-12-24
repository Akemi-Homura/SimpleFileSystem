//
// Created by quals on 2018/12/18.
//

#include <src/diskfs/DiskManager.h>
#include <cstring>
#include "MDir.h"

/**
 * 初始化目录，步骤如下：
 * 1. 清空items_
 * 2. 初始化inode节点
 * 3. 添加"."和".."两个目录项
 * 4. 在父目录中添加当前目录（根节点不添加）
 * @param fa_inode_num 父节点inode编号
 */
void MDir::Initialize(int fa_inode_num, const char *dir_name) {
    // 清空items
    // 初始化inode节点
    Inode inode(path, manager->GetInodeOffset(inode_num_));
    inode.Initialize(dir_name, Inode::kDirectory);

    // 添加"." ".."
    Insert(".", inode_num_);
    Insert("..", fa_inode_num);

    // 更新父目录
    if (inode_num_ != ROOT_DIR_INODE_NUM) {
        MDir dir(path, fa_inode_num);
        dir.Insert(dir_name, inode_num_);
    }
}

/**
 * 插入目录项，该调用必须在Initialize或者ReadFromDisk()调用之后，步骤如下：
 * 1. 从磁盘读取inode节点
 * 2. 使用inode节点进行写入
 * @param item 目录项
 */
void MDir::Insert(const MDir::DirItem &item) {
    Inode inode(path, manager->GetInodeOffset(inode_num_));
    inode.ReadFromDisk();
    inode.WriteData2Block((void *) &item.model_, sizeof(MDir::DirItem::Model), inode.model_.size_);
}

void MDir::Insert(const char *path, int inodenum) {
    DirItem item(path, inodenum);
    Insert(item);
}


const char *MDir::ROOT_DIR_NAME = "/";
const int MDir::ROOT_DIR_INODE_NUM = 0;

MDir::MDir(const char *path, int inode_num_) : path(path), inode_num_(inode_num_) {
    manager = DiskManager::GetInstance();
}

std::vector<MDir::DirItem> MDir::GetDirItems() {
    Inode inode(path, manager->GetInodeOffset(inode_num_));
    inode.ReadFromDisk();

    int item_num = inode.model_.size_ / sizeof(MDir::DirItem::Model);
    std::vector<MDir::DirItem> items;
    auto *models = new MDir::DirItem::Model[item_num];
    inode.ReadDataFromBlock(models, item_num * inode.size());
    for (int i = 0; i < item_num; i++) {
        MDir::DirItem diritem(models[i].name_, models[i].inode_num_);
        items.emplace_back(diritem);
    }
    delete[] models;
    return items;
}

int MDir::Remove(const char *name) {
    auto items = GetDirItems();
    int res = -1;
    for (auto it = items.begin(); it != items.end(); it++) {
        if (strcmp(name, it->model_.name_) == 0) {
            items.erase(it);
            res = 0;
            break;
        }
    }

    if (res == 0) {
        Inode inode(path, manager->GetInodeOffset(inode_num_));
        inode.Initialize(items[1].model_.name_, Inode::kDirectory);

        for (int i = 0; i < items.size(); i++) {
            Insert(items[i]);
        }
    }
    return res;
}

MDir::DirItem::DirItem(const char *name, int inode_num) {
    std::strcpy(model_.name_, name);
    model_.inode_num_ = inode_num;
}

