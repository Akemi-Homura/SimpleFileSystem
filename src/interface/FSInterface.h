//
// Created by quals on 2018/12/18.
//

#ifndef SIMPLEFILESYSTEM_FSINTERFACE_H
#define SIMPLEFILESYSTEM_FSINTERFACE_H


#include <array>
#include "MDir.h"

class FSInterface {
public:
    static const int O_RDONLY = 01;
    static const int O_WRONLY = 02;
    static const int O_RDWR = 04;
    static const int O_CREAT = 8;
    static const int O_TRUNC = 16;
    static const int O_APPEND = 32;

    static const int FILE_TABLE_SIZE = 255;

    static FSInterface *GetInstance();

    static bool HasRDONLY(int flag);

    static bool HasWRONLY(int flag);

    static bool HasRDWR(int flag);

    static bool HasCreat(int flag);

    static bool HasTrunc(int flag);

    static bool HasAppend(int flag);

    int Open(const char *filepath, int flag);

    int Close(int fd);

    int Read(int fd, void*buf, int size);

    int Write(int fd, void* buf, int size);

    int Unlink(const char* pathname);

    std::vector<MDir::DirItem> DirOpen(const char* pathname);

    void SetPath(const char *path);

    const char* GetErrorMsg();

private:
    class FileTableItem {
    public:
        FileTableItem();

        bool used_;
        int file_open_flag_;
        int file_offset_;
        int inode_num_;
    };

    FSInterface();

    FSInterface(const FSInterface &);

    FSInterface &operator=(const FSInterface &);

    std::array<FileTableItem, FILE_TABLE_SIZE> filetable_;
    static FSInterface *sInstance;
    char path_[255];
    DiskManager* manager;

    std::string error_msg_;
    int InsertFileTableItem(const FileTableItem &item);
};


#endif //SIMPLEFILESYSTEM_FSINTERFACE_H
