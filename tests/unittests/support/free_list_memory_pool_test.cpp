// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/memory_pool/free_list_memory_pool.h"
#include <gtest/gtest.h>
#include <set>
#include <vector>

using namespace ocudu;

TEST(free_list_memory_pool_test, pool_starts_with_all_chunks_available)
{
  free_list_memory_pool pool(4, 32, 8);

  ASSERT_EQ(pool.nof_memory_blocks(), 4);
  ASSERT_EQ(pool.nof_memory_blocks_available(), 4);
  ASSERT_FALSE(pool.full());
  ASSERT_EQ(pool.alignment(), 8);
}

TEST(free_list_memory_pool_test, chunk_size_is_rounded_up_to_the_alignment)
{
  free_list_memory_pool pool(2, 12, 8);

  ASSERT_EQ(pool.memory_block_size(), 16);
}

TEST(free_list_memory_pool_test, chunk_size_fits_the_free_list_link)
{
  free_list_memory_pool pool(2, 1, 1);

  ASSERT_GE(pool.memory_block_size(), sizeof(uint32_t));
  void* first  = pool.allocate();
  void* second = pool.allocate();
  ASSERT_NE(first, nullptr);
  ASSERT_NE(second, nullptr);
  ASSERT_NE(first, second);
}

TEST(free_list_memory_pool_test, chunks_are_relinked_when_deallocated_out_of_order)
{
  constexpr size_t      nof_chunks = 5;
  free_list_memory_pool pool(nof_chunks, 32, 8);

  std::vector<void*> chunks;
  for (size_t i = 0; i != nof_chunks; ++i) {
    chunks.push_back(pool.allocate());
  }
  ASSERT_TRUE(pool.full());

  // Return the chunks in an order different from the one they were allocated in.
  for (size_t i : {2U, 0U, 4U, 1U, 3U}) {
    pool.deallocate(chunks[i]);
  }
  ASSERT_EQ(pool.nof_memory_blocks_available(), nof_chunks);

  std::set<void*> reallocated;
  for (size_t i = 0; i != nof_chunks; ++i) {
    void* chunk = pool.allocate();
    ASSERT_NE(chunk, nullptr);
    ASSERT_TRUE(reallocated.insert(chunk).second) << "Chunk was handed out twice";
  }
  ASSERT_EQ(pool.allocate(), nullptr);
  ASSERT_EQ(reallocated, std::set<void*>(chunks.begin(), chunks.end()));
}

TEST(free_list_memory_pool_test, allocated_chunks_are_distinct_and_aligned)
{
  constexpr size_t      nof_chunks = 8;
  constexpr size_t      align      = 64;
  free_list_memory_pool pool(nof_chunks, 32, align);

  std::set<void*> chunks;
  for (size_t i = 0; i != nof_chunks; ++i) {
    void* chunk = pool.allocate();
    ASSERT_NE(chunk, nullptr);
    ASSERT_EQ(reinterpret_cast<uintptr_t>(chunk) % align, 0) << "Chunk is not aligned";
    ASSERT_TRUE(chunks.insert(chunk).second) << "Chunk was allocated twice";
  }
  ASSERT_TRUE(pool.full());
}

TEST(free_list_memory_pool_test, allocation_fails_when_pool_is_full)
{
  free_list_memory_pool pool(1, 32, 8);

  ASSERT_NE(pool.allocate(), nullptr);
  ASSERT_EQ(pool.allocate(), nullptr);
}

TEST(free_list_memory_pool_test, deallocated_chunk_can_be_allocated_again)
{
  free_list_memory_pool pool(1, 32, 8);

  void* chunk = pool.allocate();
  ASSERT_EQ(pool.allocate(), nullptr);

  pool.deallocate(chunk);
  ASSERT_EQ(pool.nof_memory_blocks_available(), 1);
  ASSERT_EQ(pool.allocate(), chunk);
}

namespace {

/// Object that counts the number of instances alive.
class counted_object
{
public:
  explicit counted_object(int val_) : val(val_) { ++nof_alive; }
  ~counted_object() { --nof_alive; }

  int value() const { return val; }

  static int nof_alive;

private:
  int val;
};

int counted_object::nof_alive = 0;

} // namespace

class free_list_object_pool_test : public ::testing::Test
{
protected:
  void SetUp() override { counted_object::nof_alive = 0; }
};

TEST_F(free_list_object_pool_test, created_object_is_constructed_with_forwarded_arguments)
{
  free_list_object_pool<counted_object> pool(2);

  auto obj = pool.get(5);
  ASSERT_NE(obj, nullptr);
  ASSERT_EQ(obj->value(), 5);
  ASSERT_EQ(counted_object::nof_alive, 1);
  ASSERT_EQ(pool.nof_objects_available(), 1);
}

TEST_F(free_list_object_pool_test, object_is_destroyed_and_returned_to_the_pool_when_handle_is_reset)
{
  free_list_object_pool<counted_object> pool(1);

  auto obj = pool.get(5);
  ASSERT_TRUE(pool.full());

  obj.reset();
  ASSERT_EQ(counted_object::nof_alive, 0) << "Object destructor was not called";
  ASSERT_EQ(pool.nof_objects_available(), 1);
  ASSERT_FALSE(pool.full());
}

TEST_F(free_list_object_pool_test, object_creation_fails_when_pool_is_full)
{
  free_list_object_pool<counted_object> pool(1);

  auto obj = pool.get(1);
  ASSERT_NE(obj, nullptr);
  ASSERT_EQ(pool.get(2), nullptr);
  ASSERT_EQ(counted_object::nof_alive, 1);
}

TEST_F(free_list_object_pool_test, objects_are_stable_in_memory_while_other_objects_are_created_and_destroyed)
{
  free_list_object_pool<counted_object> pool(4);

  auto        first      = pool.get(1);
  const auto* first_addr = first.get();

  auto second = pool.get(2);
  auto third  = pool.get(3);
  second.reset();
  auto fourth = pool.get(4);

  ASSERT_EQ(first.get(), first_addr) << "Object address changed";
  ASSERT_EQ(first->value(), 1);
}
