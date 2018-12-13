cmake_minimum_required(VERSION 3.2)
project(SimpleFileSystem CXX)

set(CMAKE_CXX_STANDARD 11)

configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
if (result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif ()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
if (result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif ()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
        ${CMAKE_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif ()

include_directories(".")

set(DiskFSSrc
        src/diskfs/DiskPart.cpp
        src/diskfs/Bitmap.cpp
        src/diskfs/Block.cpp
        src/diskfs/DiskManager.cpp
        src/diskfs/Inode.cpp
        src/diskfs/Super.cpp)

set(ExceptionSrc
        src/exception/ExceptionHelper.cpp)

set(FileOperationSrc
        src/file_operation_helper/FileOperationHelper.cpp)

add_executable(DiskFSTest
        ${DiskFSSrc}
        ${ExceptionSrc}
        ${FileOperationSrc}
        test/DiskFSTest.cpp)


add_executable(SimpleFileSystem
        src/main.cc
        )

set(TESTS
        DiskFSTest;)

foreach (x ${TESTS})
    target_link_libraries(${x} gtest_main)
endforeach ()

include(CTest)