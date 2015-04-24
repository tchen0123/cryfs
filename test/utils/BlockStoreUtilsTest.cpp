#include "../../implementations/testfake/FakeBlockStore.h"
#include "../testutils/DataBlockFixture.h"
#include "../../utils/BlockStoreUtils.h"
#include "google/gtest/gtest.h"

#include <memory>

using ::testing::Test;
using ::testing::WithParamInterface;
using ::testing::Values;

using std::make_unique;
using std::unique_ptr;

using namespace blockstore;
using namespace blockstore::utils;

using blockstore::testfake::FakeBlockStore;

class BlockStoreUtilsTest: public Test {
public:
  unsigned int SIZE = 1024 * 1024;
  BlockStoreUtilsTest():
    ZEROES(SIZE),
    dataFixture(SIZE),
    blockStore(make_unique<FakeBlockStore>()) {
    ZEROES.FillWithZeroes();
  }

  Data ZEROES;
  DataBlockFixture dataFixture;
  unique_ptr<BlockStore> blockStore;
};

TEST_F(BlockStoreUtilsTest, FillWithZeroes) {
  auto block = blockStore->create(Data(SIZE));
  block->write(dataFixture.data(), 0, SIZE);
  EXPECT_NE(0, std::memcmp(ZEROES.data(), block->data(), SIZE));
  fillWithZeroes(block.get());
  EXPECT_EQ(0, std::memcmp(ZEROES.data(), block->data(), SIZE));
}

class BlockStoreUtilsTest_CopyToNewBlock: public BlockStoreUtilsTest {};

TEST_F(BlockStoreUtilsTest_CopyToNewBlock, CopyEmptyBlock) {
  auto block = blockStore->create(Data(0));
  auto block2 = copyToNewBlock(blockStore.get(), *block);

  EXPECT_EQ(0u, block2->size());
}

TEST_F(BlockStoreUtilsTest_CopyToNewBlock, CopyZeroBlock) {
  auto block = blockStore->create(ZEROES);
  auto block2 = copyToNewBlock(blockStore.get(), *block);

  EXPECT_EQ(SIZE, block2->size());
  EXPECT_EQ(0, std::memcmp(ZEROES.data(), block2->data(), SIZE));
}

TEST_F(BlockStoreUtilsTest_CopyToNewBlock, CopyDataBlock) {
  auto block = blockStore->create(Data(SIZE));
  block->write(dataFixture.data(), 0, SIZE);
  auto block2 = copyToNewBlock(blockStore.get(), *block);

  EXPECT_EQ(SIZE, block2->size());
  EXPECT_EQ(0, std::memcmp(dataFixture.data(), block2->data(), SIZE));
}

TEST_F(BlockStoreUtilsTest_CopyToNewBlock, OriginalBlockUnchanged) {
  auto block = blockStore->create(Data(SIZE));
  block->write(dataFixture.data(), 0, SIZE);
  auto block2 = copyToNewBlock(blockStore.get(), *block);

  EXPECT_EQ(SIZE, block->size());
  EXPECT_EQ(0, std::memcmp(dataFixture.data(), block->data(), SIZE));
}

class BlockStoreUtilsTest_CopyToExistingBlock: public BlockStoreUtilsTest {};

TEST_F(BlockStoreUtilsTest_CopyToExistingBlock, CopyEmptyBlock) {
  auto block = blockStore->create(Data(0));
  auto block2 = blockStore->create(Data(0));
  copyTo(block2.get(), *block);
}

TEST_F(BlockStoreUtilsTest_CopyToExistingBlock, CopyZeroBlock) {
  auto block = blockStore->create(ZEROES);
  auto block2 = blockStore->create(Data(SIZE));
  block2->write(dataFixture.data(), 0, SIZE);
  copyTo(block2.get(), *block);

  EXPECT_EQ(0, std::memcmp(ZEROES.data(), block2->data(), SIZE));
}

TEST_F(BlockStoreUtilsTest_CopyToExistingBlock, CopyDataBlock) {
  auto block = blockStore->create(Data(SIZE));
  block->write(dataFixture.data(), 0, SIZE);
  auto block2 = blockStore->create(Data(SIZE));
  copyTo(block2.get(), *block);

  EXPECT_EQ(0, std::memcmp(dataFixture.data(), block2->data(), SIZE));
}

TEST_F(BlockStoreUtilsTest_CopyToExistingBlock, OriginalBlockUnchanged) {
  auto block = blockStore->create(Data(SIZE));
  block->write(dataFixture.data(), 0, SIZE);
  auto block2 = blockStore->create(Data(SIZE));
  copyTo(block2.get(), *block);

  EXPECT_EQ(0, std::memcmp(dataFixture.data(), block->data(), SIZE));
}