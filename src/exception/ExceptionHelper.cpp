//
// Created by quals on 2018/12/13.
//

#include <cstdio>
#include <errno.h>
#include <cstring>
#include <stdexcept>
#include "ExceptionHelper.h"

void open_failed(const char *path) {
    static char res[100];
    sprintf(res, "Open %s failed: %s", path, strerror(errno));
    throw std::runtime_error(res);
}

void pread_failed(const char *path) {
    static char res[100];
    sprintf(res, "pread %s failed: %s", path, strerror(errno));
    throw std::runtime_error(res);
}

void pwrite_failed(const char *path) {
    static char res[100];
    sprintf(res, "pwrite %s failed: %s", path, strerror(errno));
    throw std::runtime_error(res);
}
