//
// Created by quals on 2018/12/13.
//

#ifndef SIMPLEFILESYSTEM_FILEOPERATIONHELPER_H
#define SIMPLEFILESYSTEM_FILEOPERATIONHELPER_H

#include <src/diskfs/DiskManager.h>

void oread(const char *path, void *buf, int size, int offset);

void owrite(const char *path, void *buf, int size, int offset);

#endif //SIMPLEFILESYSTEM_FILEOPERATIONHELPER_H
