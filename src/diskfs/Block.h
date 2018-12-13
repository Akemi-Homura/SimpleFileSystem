//
// Created by quals on 2018/12/11.
//

#ifndef SIMPLEFILESYSTEM_BLOCK_H
#define SIMPLEFILESYSTEM_BLOCK_H

#include "DiskPart.h"

class Block : public DiskPart {
public:
    Block(const char *path, int offset, int size);

    Block(Block& rhs);

    ~Block();

    void ReadFromDisk() override;

    void WriteToDisk() override;

    unsigned char *data_;
};


#endif //SIMPLEFILESYSTEM_BLOCK_H