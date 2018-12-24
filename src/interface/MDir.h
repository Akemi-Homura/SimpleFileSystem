//
// Created by quals on 2018/12/18.
//

#ifndef SIMPLEFILESYSTEM_MDIR_H
#define SIMPLEFILESYSTEM_MDIR_H


#include <src/diskfs/DiskManager.h>
#include <vector>

class MDir {
public:
    static const int ROOT_DIR_INODE_NUM;
    static const char *ROOT_DIR_NAME;

    class DirItem {
    public:
        struct Model {
            char name_[255];
            int inode_num_;
        };

        DirItem() = default;

        DirItem(const char *name, int inode_num);

        Model model_;
    };

    void Initialize(int fa_inode_num, const char *dir_name);

    std::vector<DirItem> GetDirItems();

    void Insert(const DirItem &item);

    void Insert(const char *path, int inodenum);

    MDir(const char *path, int inode_num_);

    MDir() = delete;

    int Remove(const char *name);

private:
    DiskManager *manager;
    const char *path;
    int inode_num_;
};


#endif //SIMPLEFILESYSTEM_MDIR_H
