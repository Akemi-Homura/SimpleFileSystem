//
// Created by quals on 2018/12/13.
//

#ifndef SIMPLEFILESYSTEM_EXCEPTIONHELPER_H
#define SIMPLEFILESYSTEM_EXCEPTIONHELPER_H
void open_failed(const char* path);

void pread_failed(const char* path);

void pwrite_failed(const char* path);
#endif //SIMPLEFILESYSTEM_EXCEPTIONHELPER_H
