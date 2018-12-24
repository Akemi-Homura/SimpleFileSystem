//
// Created by quals on 2018/12/18.
//

#ifndef SIMPLEFILESYSTEM_MFILE_H
#define SIMPLEFILESYSTEM_MFILE_H

#include "src/diskfs/DiskManager.h"

class MFile {
public:
    MFile(const char *path, int inode_num_);

    MFile() = delete;

    int Read(void *buf, int size, int offset = 0);

    int Write(const void *buf, int size, int offset = 0);

    void Initialize(int fa_inode_num, const char *filename);
private:
    DiskManager * manager;
    const char* path;
    int inode_num_;
};


#endif //SIMPLEFILESYSTEM_MFILE_H
