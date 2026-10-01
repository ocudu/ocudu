// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/memory_pool/unbounded_object_pool.h"
#include <gtest/gtest.h>

using namespace ocudu;

class unbounded_object_pool_test : public ::testing::Test
{
protected:
  static constexpr size_t    init_capacity = 8;
  unbounded_object_pool<int> pool{init_capacity};
};

TEST_F(unbounded_object_pool_test, pool_initiated_with_provided_capacity)
{
  ASSERT_EQ(pool.current_capacity_approx(), init_capacity);
}

TEST_F(unbounded_object_pool_test, pool_allocates_objects_within_capacity)
{
  auto obj = pool.get();
  ASSERT_NE(obj, nullptr);
  ASSERT_EQ(pool.current_capacity_approx(), init_capacity - 1);
  *obj = 42;
  obj.reset();
  ASSERT_EQ(pool.current_capacity_approx(), init_capacity);
}

TEST_F(unbounded_object_pool_test, pool_allocates_objects_beyond_initial_capacity)
{
  std::vector<unbounded_object_pool<int>::ptr> objs;
  for (unsigned i = 0; i != init_capacity; ++i) {
    objs.push_back(pool.get());
    ASSERT_NE(objs.back(), nullptr);
  }
  auto obj = pool.get();
  ASSERT_NE(obj, nullptr);

  // pool capacity grew.
  objs.clear();
  obj.reset();
  ASSERT_GT(pool.current_capacity_approx(), init_capacity);
}
