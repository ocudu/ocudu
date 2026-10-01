// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/support/memory_pool/memory_block_list.h"
#include <algorithm>
#include <gtest/gtest.h>

using namespace ocudu;

class memory_block_list_tester : public ::testing::Test
{
protected:
  void create_and_push_blocks(unsigned n)
  {
    this->blocks.resize(n);
    for (unsigned i = 0; i != this->blocks.size(); ++i) {
      list.push(&blocks[i]);
    }
  }

  std::vector<std::array<uint8_t, 128>> blocks;
  free_memory_block_list                list;
};

TEST_F(memory_block_list_tester, free_list_starts_empty)
{
  ASSERT_TRUE(list.empty());
  ASSERT_EQ(list.size(), 0);
  ASSERT_EQ(list.try_pop(), nullptr);

  list.clear(); // no-op
  ASSERT_TRUE(list.empty());
}

TEST_F(memory_block_list_tester, push_blocks)
{
  create_and_push_blocks(test_rng::uniform_int<unsigned>(1, 10));

  ASSERT_FALSE(list.empty());
  ASSERT_EQ(list.size(), this->blocks.size());
}

TEST_F(memory_block_list_tester, pop_blocks)
{
  create_and_push_blocks(test_rng::uniform_int<unsigned>(1, 10));

  std::vector<void*> popped_blocks;
  for (unsigned i = 0; i != this->blocks.size(); ++i) {
    void* node = list.try_pop();
    ASSERT_NE(node, nullptr);
    popped_blocks.push_back(node);
  }
  std::sort(popped_blocks.begin(), popped_blocks.end());
  for (unsigned i = 0; i != this->blocks.size(); ++i) {
    ASSERT_EQ(&this->blocks[i], popped_blocks[i]);
  }
  ASSERT_TRUE(list.empty());
  ASSERT_EQ(list.try_pop(), nullptr);
}

TEST_F(memory_block_list_tester, clear)
{
  create_and_push_blocks(test_rng::uniform_int<unsigned>(1, 10));

  ASSERT_FALSE(list.empty());
  list.clear();
  ASSERT_TRUE(list.empty());
  ASSERT_EQ(list.try_pop(), nullptr);
}

TEST_F(memory_block_list_tester, try_pop_batch)
{
  create_and_push_blocks(test_rng::uniform_int<unsigned>(1, 10));

  unsigned               n     = test_rng::uniform_int<unsigned>(0, blocks.size());
  free_memory_block_list list2 = list.try_pop_batch(n);

  ASSERT_EQ(list2.size(), n);
  ASSERT_EQ(list.size(), blocks.size() - n);
  for (unsigned i = 0; i != n; ++i) {
    ASSERT_NE(list2.try_pop(), nullptr);
  }
  ASSERT_EQ(list2.try_pop(), nullptr);
  for (unsigned i = 0; i != blocks.size() - n; ++i) {
    ASSERT_NE(list.try_pop(), nullptr);
  }
  ASSERT_EQ(list.try_pop(), nullptr);
}

TEST_F(memory_block_list_tester, steal_blocks)
{
  create_and_push_blocks(test_rng::uniform_int<unsigned>(1, 10));
  std::vector<std::array<uint8_t, 128>> blocks2(test_rng::uniform_int<unsigned>(0, blocks.size()));
  free_memory_block_list                list2;
  for (unsigned i = 0; i != blocks2.size(); ++i) {
    list2.push(&blocks2[i]);
  }

  list2.steal_blocks(list);
  ASSERT_EQ(list2.size(), blocks2.size() + blocks.size());
  ASSERT_EQ(list.size(), 0);

  for (unsigned i = 0; i != blocks2.size() + blocks.size(); ++i) {
    ASSERT_NE(list2.try_pop(), nullptr);
  }
  ASSERT_EQ(list2.try_pop(), nullptr);
}
