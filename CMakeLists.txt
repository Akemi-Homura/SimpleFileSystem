cmake_minimum_required(VERSION 3.12)
project(SimpleFileSystem C)

set(CMAKE_C_STANDARD 11)

include_directories(".")

add_executable(SimpleFileSystem src/main.c)