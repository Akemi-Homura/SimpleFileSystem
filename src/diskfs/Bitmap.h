//
// Created by quals on 2018/12/11.
//

#ifndef SIMPLEFILESYSTEM_BITMAP_H
#define SIMPLEFILESYSTEM_BITMAP_H

#include "DiskPart.h"

class Bitmap : public DiskPart {
public:
    void ReadFromDisk() override;

    void WriteToDisk() override;

    Bitmap(const char *path, int offset, int size);

    Bitmap(const Bitmap& rhs);

    ~Bitmap();

    void zero();

    int len();

    bool &operator[](int n);

    const bool &operator[](int n) const;

private:
    bool *data_;

    char *raw_data_;

    int len_;

    void CheckIndex(int n) const;

    void set(bool flag);
};


#endif //SIMPLEFILESYSTEM_BITMAP_H
