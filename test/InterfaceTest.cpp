//
// Created by quals on 2018/12/19.
//
#include <gtest/gtest.h>
#include <src/interface/MFile.h>
#include <src/interface/MDir.h>
#include <src/diskfs/Inode.h>

#include "src/interface/FSInterface.h"

class InterfaceFixture : public ::testing::Test {
protected:
    virtual void SetUp() override {
        srand(static_cast<unsigned int>(time(nullptr)));
        path = "disk";
        manager = DiskManager::GetInstance();
        interface = FSInterface::GetInstance();
        interface->SetPath(path);
        manager->Format(512);
        manager->InitializeFileSystem(path);
        block_size = manager->GetBlockSize();
    }

    virtual void TearDown() override {
    }

    FSInterface *interface;
    DiskManager *manager;
    int block_size;
    const char *path;
};

TEST_F(InterfaceFixture, InterfaceTest) {
    const char *filename1 = "/test.txt";
    const char *msg1 = "Hello World";
    const char *msg2 = "NDSL Homework";
    const int msg_size1 = (int) std::strlen(msg1) + 1;
    const int msg_size2 = (int) std::strlen(msg2) + 1;

    /* open first file */
    int fd = interface->Open(filename1, FSInterface::O_WRONLY | FSInterface::O_CREAT);
    if (fd == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }

    /* write then read */
    if (interface->Write(fd, (void *) msg1, msg_size1) == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }
    if (interface->Close(fd) == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }

    fd = interface->Open(filename1, FSInterface::O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }

    char content[40];
    if (interface->Read(fd, content, msg_size1) == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }
    EXPECT_EQ(std::string(msg1), std::string(content));
    if (interface->Close(fd) == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }

    /* open with O_APPEND */
    fd = interface->Open(filename1, FSInterface::O_WRONLY | FSInterface::O_APPEND);
    if (fd == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }
    if (interface->Write(fd, (void *) msg2, msg_size2) == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }
    if (interface->Close(fd) == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }

    /* read it */
    fd = interface->Open(filename1, FSInterface::O_RDWR);
    if (fd == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }
    if (interface->Read(fd, (void *) content, msg_size1) == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }
    EXPECT_EQ(std::string(msg1), std::string(content));

    if (interface->Read(fd, (void *) content, msg_size2) == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }
    EXPECT_EQ(std::string(msg2), std::string(content));
    if (interface->Close(fd) == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }

    /* open with trunc */
    fd = interface->Open(filename1, FSInterface::O_RDWR | FSInterface::O_TRUNC);
    if (fd == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }
    EXPECT_EQ(0, interface->Read(fd, (void *) content, msg_size1));
    if (interface->Write(fd, (void *) msg2, msg_size2) == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }
    if (interface->Write(fd, (void *) msg1, msg_size1) == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }

    /* read and check it */
    fd = interface->Open(filename1, FSInterface::O_RDWR);
    if (fd == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }
    if (interface->Read(fd, (void *) content, msg_size2) == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }
    EXPECT_EQ(std::string(msg2), std::string(content));
    if (interface->Read(fd, (void *) content, msg_size1) == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }
    EXPECT_EQ(std::string(msg1), std::string(content));

    /* check open dir */
    auto items = interface->DirOpen(MDir::ROOT_DIR_NAME);
    EXPECT_EQ(3, items.size());
    EXPECT_EQ(std::string(filename1 + 1), std::string(items[2].model_.name_));

    /* unlink */
    if (interface->Unlink(filename1) == -1) {
        throw std::runtime_error(interface->GetErrorMsg());
    }
    items = interface->DirOpen(MDir::ROOT_DIR_NAME);
    EXPECT_EQ(2, items.size());
    EXPECT_EQ(std::string("."), std::string(items[0].model_.name_));
    EXPECT_EQ(std::string(".."), std::string(items[1].model_.name_));
}

TEST_F(InterfaceFixture, InodeTest) {
    const int sec_block_num = Inode::DIRECT_BLOCK_NUM + block_size / Inode::BLOCK_INDEX_SIZE;
    const int block_num_in_sec_indirect_block = block_size / Inode::BLOCK_INDEX_SIZE;
    const int NUM = 4060;

    manager->Format(block_size);
    manager->InitializeFileSystem(path);

    int inode_num = manager->MallocInode(path);
    Inode inode(path, manager->GetInodeOffset(inode_num));
    inode.Initialize("test.txt", Inode::kNormal);


    for (int i = 0; i < Inode::DIRECT_BLOCK_NUM; i++) {
        int index = inode.GetBlockIndex(i);
        ASSERT_EQ(index, i + 2);

    }
    for (int i = Inode::DIRECT_BLOCK_NUM; i < sec_block_num; i++) {
        int index = inode.GetBlockIndex(i);
        ASSERT_EQ(index, i + 3);
    }

    ASSERT_EQ(Inode::DIRECT_BLOCK_NUM + 2, inode.model_.indirect_block_2_);

    for (int i = sec_block_num; i < NUM; i++) {
        int index = inode.GetBlockIndex(i);
        ASSERT_EQ(i + 5 + (i - sec_block_num) / block_num_in_sec_indirect_block, index);
    }
}

TEST_F(InterfaceFixture, BigMFile) {
    const char *msg = "Hello World";
    const int msg_size = (int) std::strlen(msg) + 1;
    const int block_num = Inode::DIRECT_BLOCK_NUM + block_size / Inode::BLOCK_INDEX_SIZE;
    const int write_times = 1000;

    int inode_num = manager->MallocInode(path);
    MFile f1(path, inode_num);
    f1.Initialize(MDir::ROOT_DIR_INODE_NUM, "bigtest.txt");

    for (int i = 0; i < write_times; i++) {
        f1.Write(msg, msg_size, msg_size * i);
    }

    MFile f2(path, inode_num);
    char content[20];
    for (int i = 0; i < write_times; i++) {
        f2.Read(content, msg_size, msg_size * i);
        ASSERT_EQ(std::string(msg), std::string(content));
    }
}

TEST_F(InterfaceFixture, BigDir) {
    const char *basefilename = "file";
    const int create_times = 1000;

    char filename[20];
    for (int i = 0; i < create_times; i++) {
        sprintf(filename, "%s_%d", basefilename, i);
        int inode_num = manager->MallocInode(path);
        MFile f(path, inode_num);
        f.Initialize(MDir::ROOT_DIR_INODE_NUM, filename);
    }

    MDir dir(path, MDir::ROOT_DIR_INODE_NUM);
    auto items = dir.GetDirItems();
    ASSERT_EQ(create_times + 2, items.size());


    for (int i = 2; i < create_times + 2; i++) {
        sprintf(filename, "%s_%d", basefilename, i - 2);
        ASSERT_EQ(std::string(filename), std::string(items[i].model_.name_));
        ASSERT_EQ(i - 1, items[i].model_.inode_num_);
    }
}

TEST_F(InterfaceFixture, SmallMFile) {
    int inode_num = manager->MallocInode(path);
    MFile f1(path, inode_num);
    f1.Initialize(MDir::ROOT_DIR_INODE_NUM, "test.txt");
    const char *msg = "Hello World";
    const int msg_size = (int) std::strlen(msg) + 1;
    f1.Write(msg, msg_size);

    char content[20];
    MFile f2(path, inode_num);
    f2.Read(content, msg_size);
    EXPECT_EQ(std::string(msg), std::string(content));
}

TEST_F(InterfaceFixture, DiskInitializeTest) {
    Bitmap inode_bitmap(path, manager->GetInodeBitmapOffset(), block_size);
    inode_bitmap.ReadFromDisk();
    ASSERT_TRUE(inode_bitmap[MDir::ROOT_DIR_INODE_NUM]);

    MDir root(path, MDir::ROOT_DIR_INODE_NUM);
    auto items = root.GetDirItems();
    ASSERT_EQ(std::string(items[0].model_.name_), std::string("."));
    ASSERT_EQ(std::string(items[1].model_.name_), std::string(".."));

    ASSERT_EQ(MDir::ROOT_DIR_INODE_NUM, items[0].model_.inode_num_);
    ASSERT_EQ(MDir::ROOT_DIR_INODE_NUM, items[1].model_.inode_num_);
}