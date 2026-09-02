// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/unique_function.h"
#include <gtest/gtest.h>

using namespace ocudu;

struct C {
  std::unique_ptr<int> val{new int{5}};
};
struct D {
  std::array<int, 64> big_val;
  D() { big_val[0] = 6; }
};

TEST(unique_function_test, unique_function)
{
  int v = 0;

  auto l0 = [&v]() { v = 1; };

  unique_function<void()> t{l0};
  unique_function<void()> t2{[v]() mutable { v = 2; }};
  // sanity static checks
  static_assert(task_details::is_unique_function<std::decay_t<decltype(t)>>::value, "failed check\n");
  static_assert(std::is_base_of<std::false_type, task_details::is_unique_function<std::decay_t<decltype(l0)>>>::value,
                "failed check\n");

  t();
  t2();
  ASSERT_EQ(1, v);
  v              = 2;
  decltype(t) t3 = std::move(t);
  t3();
  ASSERT_EQ(1, v);

  C                       c;
  unique_function<void()> t4 = [&v, c = std::move(c)]() { v = *c.val; };
  {
    decltype(t4) t5;
    t5 = std::move(t4);
    t5();
    ASSERT_TRUE(v == 5);
  }

  D                       d;
  unique_function<void()> t6 = [&v, d]() { v = d.big_val[0]; };
  {
    unique_function<void()> t7;
    t6();
    ASSERT_EQ(6, v);
    v  = 0;
    t7 = std::move(t6);
    t7();
    ASSERT_EQ(6, v);
  }

  auto l1 = [&v, c = C{}]() { v = *c.val; };
  auto l2 = [&v, d]() { v = d.big_val[0]; };
  t       = std::move(l1);
  t2      = l2;
  v       = 0;
  t();
  ASSERT_EQ(5, v);
  t2();
  ASSERT_EQ(6, v);
  ASSERT_TRUE(t.is_in_small_buffer() and not t2.is_in_small_buffer());
  std::swap(t, t2);
  ASSERT_TRUE(t2.is_in_small_buffer() and not t.is_in_small_buffer());
  v = 0;
  t();
  ASSERT_EQ(6, v);
  t2();
  ASSERT_EQ(5, v);

  // TEST: task works in const contexts
  t       = l2;
  auto l3 = [](const unique_function<void()>& task) { task(); };
  v       = 0;
  l3(t);
  ASSERT_EQ(6, v);
}
