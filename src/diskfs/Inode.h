//
// Created by quals on 2018/12/11.
//

#ifndef SIMPLEFILESYSTEM_INODE_H
#define SIMPLEFILESYSTEM_INODE_H

#include <string>
#include "DiskPart.h"

class Inode : public DiskPart {
public:
    enum FileType {
        kDirectory,
        kNormal
    };

    struct Model {
        char name_[255];
        FileType type_;
        int size_;
        bool flag_;
        int direct_block_[12];
        int indirect_block_2_;
        int indirect_block_3_;
    };

    void ReadFromDisk() override;

    void WriteToDisk() override;

    Inode(const char *path, int offset);

    Model model_;
};


#endif //SIMPLEFILESYSTEM_INODE_H
