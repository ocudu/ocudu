// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/memory_pool/free_list_recycling_object_pool.h"
#include <gtest/gtest.h>
#include <set>
#include <vector>

using namespace ocudu;

namespace {

/// Object that counts constructions and destructions, and holds memory reserved on initialization.
class recycled_object
{
public:
  recycled_object() { ++nof_constructions; }
  ~recycled_object() { ++nof_destructions; }

  std::vector<int> data;

  static int nof_constructions;
  static int nof_destructions;
};

int recycled_object::nof_constructions = 0;
int recycled_object::nof_destructions  = 0;

} // namespace

class free_list_recycling_object_pool_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    recycled_object::nof_constructions = 0;
    recycled_object::nof_destructions  = 0;
  }
};

TEST_F(free_list_recycling_object_pool_test, all_objects_are_constructed_on_pool_creation)
{
  constexpr size_t                                 nof_objects = 4;
  free_list_recycling_object_pool<recycled_object> pool(nof_objects);

  ASSERT_EQ(recycled_object::nof_constructions, nof_objects);
  ASSERT_EQ(recycled_object::nof_destructions, 0);
  ASSERT_EQ(pool.nof_objects(), nof_objects);
  ASSERT_EQ(pool.nof_objects_available(), nof_objects);
}

TEST_F(free_list_recycling_object_pool_test, objects_are_initialized_once_on_pool_creation)
{
  constexpr size_t                                 nof_objects = 3;
  free_list_recycling_object_pool<recycled_object> pool(nof_objects,
                                                        [](recycled_object& obj) { obj.data.reserve(10); });

  auto obj = pool.get();
  ASSERT_NE(obj, nullptr);
  ASSERT_GE(obj->data.capacity(), 10) << "Object was not initialized by the pool";
}

TEST_F(free_list_recycling_object_pool_test, object_is_not_destroyed_when_returned_to_the_pool)
{
  free_list_recycling_object_pool<recycled_object> pool(1);

  auto obj = pool.get();
  ASSERT_NE(obj, nullptr);
  obj->data.reserve(20);
  const auto* addr = obj.get();

  obj.reset();
  ASSERT_EQ(recycled_object::nof_destructions, 0) << "Object was destroyed on release";
  ASSERT_EQ(pool.nof_objects_available(), 1);

  // The next holder gets the same object, with the memory it had reserved.
  auto reused = pool.get();
  ASSERT_EQ(reused.get(), addr);
  ASSERT_GE(reused->data.capacity(), 20) << "Reserved memory was not preserved";
}

TEST_F(free_list_recycling_object_pool_test, objects_are_distinct_and_stable_in_memory)
{
  constexpr size_t                                 nof_objects = 8;
  free_list_recycling_object_pool<recycled_object> pool(nof_objects);

  std::vector<free_list_recycling_object_pool<recycled_object>::ptr> objs;
  std::set<const recycled_object*>                                   addrs;
  for (size_t i = 0; i != nof_objects; ++i) {
    objs.push_back(pool.get());
    ASSERT_NE(objs.back(), nullptr);
    ASSERT_TRUE(addrs.insert(objs.back().get()).second) << "Object was handed out twice";
  }

  ASSERT_TRUE(pool.full());
  ASSERT_EQ(pool.get(), nullptr);

  const auto* first_addr = objs.front().get();
  objs[3].reset();
  objs.push_back(pool.get());
  ASSERT_EQ(objs.front().get(), first_addr) << "Object address changed";
}

TEST_F(free_list_recycling_object_pool_test, objects_are_destroyed_with_the_pool)
{
  constexpr size_t nof_objects = 5;
  {
    free_list_recycling_object_pool<recycled_object> pool(nof_objects);
  }
  ASSERT_EQ(recycled_object::nof_destructions, nof_objects);
}
