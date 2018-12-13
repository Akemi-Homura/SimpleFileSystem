//
// Created by quals on 2018/12/11.
//

#ifndef SIMPLEFILESYSTEM_DISKPART_H
#define SIMPLEFILESYSTEM_DISKPART_H


#include <string>

class DiskPart {
protected:
    DiskPart(const char *filepath, int offset, int size);

    const char *path_;
    int offset_;
    int size_;
public:
    virtual void ReadFromDisk() = 0;

    virtual void WriteToDisk() = 0;

    int size();

    int offset();
};


#endif //SIMPLEFILESYSTEM_DISKPART_H
