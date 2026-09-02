// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/expected.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

// test trivially_destructible
static_assert(std::is_trivially_destructible_v<expected<int, int>>, "expected should be trivially destructible");
static_assert(not std::is_trivially_destructible_v<expected<int, moveonly_test_object>>,
              "expected should not be trivially destructible");
static_assert(not std::is_trivially_destructible_v<expected<moveonly_test_object, int>>,
              "expected should not be trivially destructible");

TEST(expected_test, expected_trivial)
{
  expected<int> exp;
  ASSERT_TRUE(exp.has_value());
  ASSERT_TRUE(exp);

  exp = 5;
  ASSERT_TRUE(exp.has_value());
  ASSERT_TRUE(exp.value() == 5);
  ASSERT_TRUE(exp);

  exp = make_unexpected(default_error_t{});
  ASSERT_TRUE(not exp.has_value());
  ASSERT_TRUE(not exp);

  int i = 2;
  exp   = i;
  ASSERT_TRUE(exp);
  ASSERT_TRUE(exp.value() == 2);

  exp = make_unexpected(default_error_t{});
  ASSERT_TRUE(not exp);

  exp = 3;
  {
    expected<int> exp2 = exp;
    ASSERT_TRUE(exp2 and exp2.value() == 3);
    expected<int> exp3;
    exp3 = exp2;
    ASSERT_TRUE(exp3 and exp3.value() == 3);
  }
  ASSERT_TRUE(exp and exp.value() == 3);

  exp = make_unexpected(default_error_t{});
  {
    expected<int> exp2{exp};
    ASSERT_TRUE(not exp2);
    expected<int> exp3;
    exp3 = exp;
    ASSERT_TRUE(not exp3);
  }
}

struct C {
  C() : val(0) { count++; }
  C(int v) : val(v) { count++; }
  C(const C& other)
  {
    val = other.val;
    count++;
  }
  C(C&& other)
  {
    val       = other.val;
    other.val = 0;
    count++;
  }
  ~C() { count--; }
  C& operator=(const C& other)
  {
    val = other.val;
    return *this;
  }
  C& operator=(C&& other)
  {
    val       = other.val;
    other.val = 0;
    return *this;
  }
  int             val;
  static uint32_t count;
};
uint32_t C::count = 0;

TEST(expected_test, expected_struct)
{
  expected<C, int> exp;
  exp = C{5};
  ASSERT_TRUE(exp and exp.value().val == 5);
  ASSERT_TRUE(C::count == 1);

  {
    auto exp2 = exp;
    ASSERT_TRUE(exp2 and exp2.value().val == 5);
    ASSERT_TRUE(C::count == 2);
  }
  ASSERT_TRUE(exp and exp.value().val == 5);
  ASSERT_TRUE(C::count == 1);

  {
    auto exp2 = std::move(exp);
    ASSERT_TRUE(exp2 and exp2.value().val == 5);
    ASSERT_TRUE(exp and exp.value().val == 0);
  }

  exp = make_unexpected(2);
  ASSERT_TRUE(not exp and exp.error() == 2);
}

TEST(expected_test, unique_ptr)
{
  expected<std::unique_ptr<C>> exp;
  ASSERT_TRUE(exp);
  exp.value().reset(new C{2});
  ASSERT_TRUE(exp.value()->val == 2);

  {
    auto exp2 = std::move(exp);
    ASSERT_TRUE(exp.value() == nullptr);
    ASSERT_TRUE(exp2 and exp2.value()->val == 2);
  }
}
