//
// Created by quals on 2018/12/11.
//

#ifndef SIMPLEFILESYSTEM_SUPER_H
#define SIMPLEFILESYSTEM_SUPER_H

#include "DiskPart.h"

class Super : public DiskPart {
public:
    struct Model {
        int block_num_;
        int inode_num_;
        int block_size_;
    };

    void ReadFromDisk() override;

    void WriteToDisk() override;

    Super(const char *path, int offset);

    int BlockNum();

    void SetBlockNum(int num);

    int BlockSize();

    void SetBlockSize(int size);

    int InodeNum();

    void SetInodeNum(int num);

private:
    Model model_;
};


#endif //SIMPLEFILESYSTEM_SUPER_H
