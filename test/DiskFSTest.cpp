//
// Created by quals on 2018/12/12.
//

#include <gtest/gtest.h>
#include "src/diskfs/DiskManager.h"
#include <unistd.h>
#include <fcntl.h>
#include <bitset>

class DiskManagerFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
        srand(static_cast<unsigned int>(time(nullptr)));
        path = "disk";
    }

    virtual void TearDown() {
    }

    const char *path;
};

TEST_F(DiskManagerFixture, DiskManagerTest) {
    DiskManager *manager = DiskManager::GetInstance();
    const int block_size = 4096;
    manager->Format(block_size);
    manager->InitializeFileSystem(path);

    /** Inode Bitmap */
    Bitmap ib1(path, manager->GetInodeBitmapOffset(), manager->GetBlockSize());
    for (int i = 0; i < ib1.len(); i++) {
        ib1[i] = (rand() & 1) == 1;
    }
    ib1.WriteToDisk();

    /** Block Bitmap */
    Bitmap bb1(path, manager->GetBlockBitmapOffset(), manager->GetBlockSize());
    for (int i = 0; i < bb1.size(); i++) {
        bb1[i] = (rand() & 1) == 1;
    }
    bb1.WriteToDisk();

    /** One Inode */
    const int inode_index = rand() % (block_size * 8);
    Inode i1(path, manager->GetInodeOffset(inode_index));
    strcpy(i1.model_.name_, "res.txt");
    i1.model_.size_ = rand();
    i1.model_.type_ = Inode::kNormal;
    for (int i = 0; i < 12; i++) {
        i1.model_.direct_block_[i] = rand();
    }
    i1.model_.indirect_block_2_ = rand();
    i1.model_.indirect_block_3_ = rand();
    i1.WriteToDisk();

    /** One block */
    const int block_index = rand() % (block_size * 8);
    Block b1(path, manager->GetBlockOffset(block_index), manager->GetBlockSize());
    for (int i = 0; i < b1.size(); i++) {
        b1.data_[i] = (unsigned char) (rand() % 255);
    }
    b1.WriteToDisk();

    Super sp1(path, manager->GetSuperOffset());
    sp1.ReadFromDisk();
    /** Super */
    DiskManager *manager2 = DiskManager::GetInstance();
    manager2->ReadFileSystemFromDisk(path);

    Super sp2(path, manager2->GetSuperOffset());
    sp2.ReadFromDisk();
    EXPECT_EQ(sp1.BlockNum(), sp2.BlockNum());
    EXPECT_EQ(sp1.BlockSize(), sp2.BlockSize());
    EXPECT_EQ(sp1.InodeNum(), sp2.InodeNum());

    /** Inode Bitmap */
    Bitmap ib2(path, manager2->GetInodeBitmapOffset(), manager2->GetBlockSize());
    ASSERT_EQ(ib1.offset(), ib2.offset());

    ib2.ReadFromDisk();
    for (int i = 0; i < ib2.len(); i++) {
        EXPECT_EQ(ib1[i], ib2[i]);
    }

    /** Block bitmap*/
    Bitmap bb2(path, manager2->GetBlockBitmapOffset(), manager2->GetBlockSize());
    ASSERT_EQ(bb1.offset(), bb2.offset());

    bb2.ReadFromDisk();
    for (int i = 0; i < bb2.len(); i++) {
        EXPECT_EQ(bb1[i], bb2[i]);
    }

    /** One inode */
    Inode i2(path, manager2->GetInodeOffset(inode_index));
    ASSERT_EQ(i1.offset(), i2.offset());
    i2.ReadFromDisk();

    EXPECT_EQ(0, strcmp(i1.model_.name_, i2.model_.name_));
    EXPECT_EQ(i1.model_.type_, i2.model_.type_);
    EXPECT_EQ(i1.model_.size_, i2.model_.size_);
    for (int i = 0; i < 12; i++) {
        EXPECT_EQ(i1.model_.direct_block_[i], i2.model_.direct_block_[i]);
    }
    EXPECT_EQ(i1.model_.indirect_block_2_, i2.model_.indirect_block_2_);
    EXPECT_EQ(i1.model_.indirect_block_3_, i2.model_.indirect_block_3_);

    /** One block */
    Block b2(path, manager2->GetBlockOffset(block_index), manager2->GetBlockSize());
    ASSERT_EQ(b1.offset(), b2.offset());
    b2.ReadFromDisk();
    for (int i = 0; i < b2.size(); i++) {
        EXPECT_EQ(b1.data_[i], b2.data_[i]);
    }
}

void ReadFilePerBit(int fd, int offset, int bytes);

TEST_F(DiskManagerFixture, InodeTest) {
    const int offset = 0;

    Inode i1(path, offset);
    strcpy(i1.model_.name_, "res.txt");
    i1.model_.size_ = rand();
    i1.model_.type_ = Inode::kNormal;
    for (int i = 0; i < 12; i++) {
        i1.model_.direct_block_[i] = rand();
    }
    i1.model_.indirect_block_2_ = rand();
    i1.model_.indirect_block_3_ = rand();
    i1.WriteToDisk();

    Inode i2(path, offset);
    i2.ReadFromDisk();

    EXPECT_EQ(0, strcmp(i1.model_.name_, i2.model_.name_));
    EXPECT_EQ(i1.model_.type_, i2.model_.type_);
    EXPECT_EQ(i1.model_.size_, i2.model_.size_);
    for (int i = 0; i < 12; i++) {
        EXPECT_EQ(i1.model_.direct_block_[i], i2.model_.direct_block_[i]);
    }
    EXPECT_EQ(i1.model_.indirect_block_2_, i2.model_.indirect_block_2_);
    EXPECT_EQ(i1.model_.indirect_block_3_, i2.model_.indirect_block_3_);

    /** copy constructor */
    Inode i3(i1);
    EXPECT_EQ(0, strcmp(i1.model_.name_, i3.model_.name_));
    EXPECT_EQ(i1.model_.type_, i3.model_.type_);
    EXPECT_EQ(i1.model_.size_, i3.model_.size_);
    for (int i = 0; i < 12; i++) {
        EXPECT_EQ(i1.model_.direct_block_[i], i3.model_.direct_block_[i]);
    }
    EXPECT_EQ(i1.model_.indirect_block_2_, i3.model_.indirect_block_2_);
    EXPECT_EQ(i1.model_.indirect_block_3_, i3.model_.indirect_block_3_);

}

TEST_F(DiskManagerFixture, BitmapTest) {
    const int size = 4096 * 8;
    const int offset = 0;

    Bitmap bitmap = Bitmap(path, offset, size / 8);
    for (int i = 0; i < size; i++) {
        bitmap[i] = (rand() & 1) == 1;
    }
    bitmap.WriteToDisk();

    Bitmap b2 = Bitmap(path, offset, size / 8);
    b2.ReadFromDisk();

    for (int i = 0; i < size; i++) {
        EXPECT_EQ(b2[i], bitmap[i]);
    }

    /** copy constuctor */
    Bitmap b3(bitmap);
    for (int i = 0; i < size; i++) {
        EXPECT_EQ(bitmap[i], b3[i]);
    }

    /** zero method **/
    b3.zero();
    b3.WriteToDisk();

    b2.ReadFromDisk();
    for(int i=0;i<size;i++){
        EXPECT_FALSE(b2[i]);
    }
}

TEST_F(DiskManagerFixture, BlockTest) {
    const int size = 4096;
    const int offset = 0;

    Block b1(path, offset, size);
    for (int i = 0; i < size; i++) {
        b1.data_[i] = (unsigned char) (rand() % 255);
    }
    b1.WriteToDisk();

    Block b2(path, offset, size);
    b2.ReadFromDisk();
    for (int i = 0; i < size; i++) {
        EXPECT_EQ(b1.data_[i], b2.data_[i]);
    }

    /** copy constructor */
    Block b3(b1);
    for (int i = 0; i < size; i++) {
        EXPECT_EQ(b1.data_[i], b3.data_[i]);
    }
}

TEST_F(DiskManagerFixture, SuperTest) {
    const int offset = rand();
    const int block_num = rand();
    const int inode_num = rand();
    const int block_size = rand();

    Super s1 = Super(path, offset);
    s1.SetBlockNum(block_num);
    s1.SetInodeNum(inode_num);
    s1.SetBlockSize(block_size);
    s1.WriteToDisk();

    Super s2 = Super(path, offset);
    s2.ReadFromDisk();

    EXPECT_EQ(block_num, s2.BlockNum());
    EXPECT_EQ(inode_num, s2.InodeNum());
    EXPECT_EQ(block_size, s2.BlockSize());

    /** copy constructor */
    Super s3(s1);
    EXPECT_EQ(s1.BlockNum(), s3.BlockNum());
    EXPECT_EQ(s1.InodeNum(), s3.InodeNum());
    EXPECT_EQ(s1.BlockSize(), s3.BlockSize());
}

TEST(DiskSync, SimpleTest) {
    const char *path = "sync_test";
    const int size = 10000;

    int *arr = new int[size];
    int fd;
    fd = open(path, O_RDWR | O_CREAT);
    for (int i = 0; i < size; i++) {
        arr[i] = rand();
        write(fd, &arr[i], 4);
    }
    fsync(fd);
    close(fd);


    if ((fd = open(path, O_RDWR | O_CREAT)) == -1) {
        perror("open failed");
        exit(1);
    }
    int x;
    for (int i = 0; i < size; i++) {
        if (read(fd, &x, 4) == -1) {
            perror("read failed: ");
            exit(1);
        }
        EXPECT_EQ(arr[i], x);
    }
    close(fd);
    delete[] arr;
}

void ReadFilePerBit(int fd, int offset, int bytes) {
    char c;
    lseek(fd, offset, SEEK_SET);
    for (int i = 0; i < bytes; i++) {
        read(fd, &c, 1);
        for (int j = 0; j < 8; j++) {
            int tmp = 1 << (7 - j);
            printf("%d%c", (c & tmp) == tmp, " \n"[j == 7]);
        }
    }
}