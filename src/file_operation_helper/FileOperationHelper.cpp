//
// Created by quals on 2018/12/13.
//

#include <fcntl.h>
#include <unistd.h>
#include "FileOperationHelper.h"
#include "src/exception/ExceptionHelper.h"

void oread(const char *path, void *buf, int size, int offset) {
    int fd;
    if ((fd = open(path, O_RDWR)) == -1) {
        open_failed(path);
    }
    if (pread(fd, buf, size, offset) != size) {
        pread_failed(path);
    }
    close(fd);
}

void owrite(const char *path, void *buf, int size, int offset) {
    int fd;
    if ((fd = open(path, O_RDWR)) == -1) {
        open_failed(path);
    }
    if (pwrite(fd, buf, size, offset) != size) {
        pwrite_failed(path);
    }
    fsync(fd);
    close(fd);
}
