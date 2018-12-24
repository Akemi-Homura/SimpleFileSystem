//
// Created by quals on 2018/12/18.
//

#include "FSInterface.h"

#include "MDir.h"
#include "MFile.h"


FSInterface *FSInterface::sInstance = nullptr;

FSInterface *FSInterface::GetInstance() {
    if (sInstance == nullptr) {
        sInstance = new FSInterface;
    }
    return sInstance;
}

int FSInterface::Open(const char *filepath, int flag) {
    FileTableItem item;
    item.used_ = true;
    item.file_open_flag_ = flag;
    item.file_offset_ = 0;

    int inode_num = manager->namei(path_, filepath);
    if (inode_num != -1) {
        Inode inode(path_, inode_num);
        inode.ReadFromDisk();
        if (HasTrunc(flag)) {
            inode.Trunc();
        }
        if (HasAppend(flag)) {
            item.file_offset_ = inode.model_.size_;
        }
    } else {
        if (HasCreat(flag)) {
            inode_num = manager->MallocInode(path_);
            Inode inode(path_, inode_num);
            inode.Initialize(filepath, Inode::kNormal);

            MDir root(path_, MDir::ROOT_DIR_INODE_NUM);
            root.Insert(filepath, inode_num);
        } else {
            error_msg_ = "Open file failed, file not exist: ";
            error_msg_.append(filepath);
            return -1;
        }
    }

    item.inode_num_ = inode_num;
    int res = InsertFileTableItem(item);
    if (res == -1) {
        error_msg_ = "Open file failed, file table is full";
    }
    return res;
}

FSInterface::FSInterface() {
    manager = DiskManager::GetInstance();
}

bool FSInterface::HasRDONLY(int flag) {
    return (flag & O_RDONLY) == O_RDONLY;
}

bool FSInterface::HasWRONLY(int flag) {
    return (flag & O_WRONLY) == O_WRONLY;
}

bool FSInterface::HasRDWR(int flag) {
    return (flag & O_RDWR) == O_RDWR;
}

bool FSInterface::HasCreat(int flag) {
    return (flag & O_CREAT) == O_CREAT;
}

bool FSInterface::HasTrunc(int flag) {
    return (flag & O_TRUNC) == O_TRUNC;
}

bool FSInterface::HasAppend(int flag) {
    return (flag & O_APPEND) == O_APPEND;
}

void FSInterface::SetPath(const char *path) {
    strcpy(path_, path);
}

int FSInterface::InsertFileTableItem(const FSInterface::FileTableItem &item) {
    for (int i = 0; i < FILE_TABLE_SIZE; i++) {
        if (!filetable_[i].used_) {
            filetable_[i] = item;
            return i;
        }
    }
    return -1;
}

int FSInterface::Close(int fd) {
    if (!filetable_[fd].used_) {
        error_msg_ = "Close file failed, Invalid file descriptor: ";
        error_msg_.append(std::to_string(fd));
        return -1;
    }
    filetable_[fd].used_ = false;
    return 0;
}

int FSInterface::Read(int fd, void *buf, int size) {
    if (!filetable_[fd].used_) {
        error_msg_ = "Read file failed, Invalid file descriptor: ";
        error_msg_.append(std::to_string(fd));
        return -1;
    }
    int flag = filetable_[fd].file_open_flag_;
    if (!(HasRDONLY(flag) || HasRDWR(flag))) {
        error_msg_ = "Read failed, permission denied";
        return -1;
    }

    MFile file(path_, filetable_[fd].inode_num_);
    int res = file.Read(buf, size, filetable_[fd].file_offset_);
    filetable_[fd].file_offset_ += res;

    return res;
}

int FSInterface::Write(int fd, void *buf, int size) {
    if (!filetable_[fd].used_) {
        error_msg_ = "Write failed, Invalid file descriptor: ";
        error_msg_.append(std::to_string(fd));
        return -1;
    }

    int flag = filetable_[fd].file_open_flag_;
    if (!(HasWRONLY(flag) || HasRDWR(flag))) {
        error_msg_ = "Write failed, permission denied";
        return -1;
    }

    MFile file(path_, filetable_[fd].inode_num_);
    int res = file.Write(buf, size, filetable_[fd].file_offset_);
    filetable_[fd].file_offset_ += res;

    return res;
}

const char *FSInterface::GetErrorMsg() {
    return error_msg_.c_str();
}

int FSInterface::Unlink(const char *pathname) {
    MDir root(path_, MDir::ROOT_DIR_INODE_NUM);
    int res = root.Remove(pathname);
    if (res == -1) {
        error_msg_ = "Unlink failed, file not exist: ";
        error_msg_.append(pathname);
    }
    return res;
}

std::vector<MDir::DirItem> FSInterface::DirOpen(const char *pathname) {
    if (strcmp(MDir::ROOT_DIR_NAME, pathname) != 0) {
        error_msg_ = "DirOpen failed, directory not exist: ";
        error_msg_.append(pathname);
        return {};
    }
    MDir root(path_, MDir::ROOT_DIR_INODE_NUM);
    return root.GetDirItems();
}

FSInterface::FileTableItem::FileTableItem() {
    used_ = false;
    file_offset_ = 0;
}
