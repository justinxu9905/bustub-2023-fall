//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_htable_test.cpp
//
// Identification: test/container/disk/hash/extendible_htable_test.cpp
//
// Copyright (c) 2015-2023, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <thread>  // NOLINT
#include <vector>

#include "buffer/buffer_pool_manager.h"
#include "common/logger.h"
#include "container/disk/hash/disk_extendible_hash_table.h"
#include "gtest/gtest.h"
#include "murmur3/MurmurHash3.h"
#include "storage/disk/disk_manager_memory.h"

namespace bustub {

// NOLINTNEXTLINE
TEST(ExtendibleHTableTest, InsertTest1) {
  auto disk_mgr = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(50, disk_mgr.get());

  DiskExtendibleHashTable<int, int, IntComparator> ht("blah", bpm.get(), IntComparator(), HashFunction<int>(), 0, 2, 2);

  int num_keys = 8;

  // insert some values
  for (int i = 0; i < num_keys; i++) {
    bool inserted = ht.Insert(i, i);
    ASSERT_TRUE(inserted);
    std::vector<int> res;
    ht.GetValue(i, &res);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(i, res[0]);
  }

  ht.VerifyIntegrity();

  // attempt another insert, this should fail because table is full
  ASSERT_FALSE(ht.Insert(num_keys, num_keys));
}

// NOLINTNEXTLINE
TEST(ExtendibleHTableTest, InsertTest2) {
  auto disk_mgr = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(50, disk_mgr.get());

  DiskExtendibleHashTable<int, int, IntComparator> ht("blah", bpm.get(), IntComparator(), HashFunction<int>(), 2, 3, 2);

  int num_keys = 5;

  // insert some values
  for (int i = 0; i < num_keys; i++) {
    bool inserted = ht.Insert(i, i);
    ASSERT_TRUE(inserted);
    std::vector<int> res;
    ht.GetValue(i, &res);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(i, res[0]);
  }

  ht.VerifyIntegrity();

  // check that they were actually inserted
  for (int i = 0; i < num_keys; i++) {
    std::vector<int> res;
    bool got_value = ht.GetValue(i, &res);
    ASSERT_TRUE(got_value);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(i, res[0]);
  }

  ht.VerifyIntegrity();

  // try to get some keys that don't exist/were not inserted
  for (int i = num_keys; i < 2 * num_keys; i++) {
    std::vector<int> res;
    bool got_value = ht.GetValue(i, &res);
    ASSERT_FALSE(got_value);
    ASSERT_EQ(0, res.size());
  }

  ht.VerifyIntegrity();
}

// NOLINTNEXTLINE
TEST(ExtendibleHTableTest, RemoveTest1) {
  auto disk_mgr = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(50, disk_mgr.get());

  DiskExtendibleHashTable<int, int, IntComparator> ht("blah", bpm.get(), IntComparator(), HashFunction<int>(), 2, 3, 2);

  int num_keys = 5;

  // insert some values
  for (int i = 0; i < num_keys; i++) {
    bool inserted = ht.Insert(i, i);
    ASSERT_TRUE(inserted);
    std::vector<int> res;
    ht.GetValue(i, &res);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(i, res[0]);
  }

  ht.VerifyIntegrity();

  // check that they were actually inserted
  for (int i = 0; i < num_keys; i++) {
    std::vector<int> res;
    bool got_value = ht.GetValue(i, &res);
    ASSERT_TRUE(got_value);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(i, res[0]);
  }

  ht.VerifyIntegrity();

  // try to get some keys that don't exist/were not inserted
  for (int i = num_keys; i < 2 * num_keys; i++) {
    std::vector<int> res;
    bool got_value = ht.GetValue(i, &res);
    ASSERT_FALSE(got_value);
    ASSERT_EQ(0, res.size());
  }

  ht.VerifyIntegrity();
  ht.PrintHT();

  // remove the keys we inserted
  for (int i = 0; i < num_keys; i++) {
    bool removed = ht.Remove(i);
    ASSERT_TRUE(removed);
    std::vector<int> res;
    ht.GetValue(i, &res);
    ASSERT_EQ(0, res.size());
  }

  ht.VerifyIntegrity();

  // try to remove some keys that don't exist/were not inserted
  for (int i = num_keys; i < 2 * num_keys; i++) {
    bool removed = ht.Remove(i);
    ASSERT_FALSE(removed);
    std::vector<int> res;
    bool got_value = ht.GetValue(i, &res);
    ASSERT_FALSE(got_value);
    ASSERT_EQ(0, res.size());
  }

  ht.VerifyIntegrity();
}

// NOLINTNEXTLINE
TEST(ExtendibleHTableTest, RecursiveMergeTest) {
  auto disk_mgr = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(50, disk_mgr.get());

  DiskExtendibleHashTable<int, int, IntComparator> ht("blah", bpm.get(), IntComparator(), HashFunction<int>(), 1, 2, 2);

  // insert some values
  for (int i = 4; i <= 6; i++) {
    bool inserted = ht.Insert(i, 0);
    ASSERT_TRUE(inserted);
    std::vector<int> res;
    ht.GetValue(i, &res);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(0, res[0]);
  }

  ht.VerifyIntegrity();

  for (int i = 4; i <= 6; i++) {
    bool inserted = ht.Insert(i, 0);
    ASSERT_FALSE(inserted);
    std::vector<int> res;
    ht.GetValue(i, &res);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(0, res[0]);
  }

  ht.VerifyIntegrity();

  ht.Insert(14, 0);
  ht.Insert(4, 0);
  ht.Insert(6, 0);
  ht.Insert(14, 0);

  ht.VerifyIntegrity();

  // remove the keys we inserted
  bool removed = ht.Remove(5);
  ASSERT_TRUE(removed);
  std::vector<int> res;
  ht.GetValue(5, &res);
  ASSERT_EQ(0, res.size());

  ht.VerifyIntegrity();

  removed = ht.Remove(14);
  ASSERT_TRUE(removed);
  res.clear();
  ht.GetValue(14, &res);
  ASSERT_EQ(0, res.size());

  ht.VerifyIntegrity();

  removed = ht.Remove(4);
  ASSERT_TRUE(removed);
  res.clear();
  ht.GetValue(4, &res);
  ASSERT_EQ(0, res.size());

  ht.VerifyIntegrity();
}

class Key : public HashFunction<int> {
  auto GetHash(int key) const -> uint64_t override { return key; }
};

// NOLINTNEXTLINE
TEST(ExtendibleHTableTest, GrowShrinkTest) {
  auto disk_mgr = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(3, disk_mgr.get());

  DiskExtendibleHashTable<int, int, IntComparator> ht("blah", bpm.get(), IntComparator(), Key(), 9, 9, 11);

  // insert some values
  for (int i = 0; i < 1000; i++) {
    bool inserted = ht.Insert(i, i);
    ASSERT_TRUE(inserted);
    std::vector<int> res;
    ht.GetValue(i, &res);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(i, res[0]);
  }
  ht.VerifyIntegrity();

  // remove some values
  for (int i = 0; i < 500; i++) {
    bool removed = ht.Remove(i);
    ASSERT_TRUE(removed);
    std::vector<int> res;
    ht.GetValue(i, &res);
    ASSERT_EQ(0, res.size());
  }
  ht.VerifyIntegrity();

  // insert some values
  for (int i = 1000; i < 1500; i++) {
    bool inserted = ht.Insert(i, i);
    ASSERT_TRUE(inserted);
    std::vector<int> res;
    ht.GetValue(i, &res);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(i, res[0]);
  }
  ht.VerifyIntegrity();

  // look up some values
  for (int i = 500; i < 1500; i++) {
    std::vector<int> res;
    ht.GetValue(i, &res);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(i, res[0]);
  }
  ht.VerifyIntegrity();

  // insert some values
  for (int i = 0; i < 500; i++) {
    bool inserted = ht.Insert(i, i);
    ASSERT_TRUE(inserted);
    std::vector<int> res;
    ht.GetValue(i, &res);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(i, res[0]);
  }
  ht.VerifyIntegrity();

  // remove some values
  for (int i = 0; i < 500; i++) {
    bool removed = ht.Remove(i);
    ASSERT_TRUE(removed);
    std::vector<int> res;
    ht.GetValue(i, &res);
    ASSERT_EQ(0, res.size());
  }
  ht.VerifyIntegrity();

  // remove some values
  for (int i = 0; i < 500; i++) {
    bool removed = ht.Remove(i);
    ASSERT_FALSE(removed);
    std::vector<int> res;
    ht.GetValue(i, &res);
    ASSERT_EQ(0, res.size());
  }
  ht.VerifyIntegrity();

  for (int i = 500; i < 1500; i++) {
    bool removed = ht.Remove(i);
    ASSERT_TRUE(removed);
    std::vector<int> res;
    ht.GetValue(i, &res);
    ASSERT_EQ(0, res.size());
  }
  ht.VerifyIntegrity();

  // remove some values
  for (int i = 0; i < 500; i++) {
    std::vector<int> res;
    ht.GetValue(i, &res);
    ASSERT_EQ(0, res.size());
  }
  ht.VerifyIntegrity();
}
}  // namespace bustub
