// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Tests the correct behavior of \c transform_optional.

#include "ocudu/support/transform_optional.h"
#include <gtest/gtest.h>

namespace {

/// The transformation function takes no argument other than the optional value.
TEST(TransformOptionalTest, NoArgs)
{
  std::optional<int> in = std::nullopt;
  auto               fn = [](int a) { return a + 1; };

  std::optional<int> out = ocudu::transform_optional(in, fn);
  EXPECT_FALSE(out.has_value()) << "Transforming an empty optional should return an empty optional.";

  in = 5;
  EXPECT_EQ(ocudu::transform_optional(in, fn), in.value() + 1) << "Error when transforming an optional with value.";
}

/// The transformation function takes one extra argument besides the optional value.
TEST(TransformOptionalTest, OneArg)
{
  std::optional<int> in = std::nullopt;
  auto               fn = [](int a, int b) { return a + b; };

  std::optional<int> out = ocudu::transform_optional(in, fn, 2);
  EXPECT_FALSE(out.has_value()) << "Transforming an empty optional should return an empty optional.";

  int extra = 3;
  in        = 5;
  EXPECT_EQ(ocudu::transform_optional(in, fn, extra), in.value() + extra)
      << "Error when transforming an optional with value.";
}

/// The transformation function takes two extra arguments besides the optional value.
TEST(TransformOptionalTest, TwoArgs)
{
  std::optional<int> in = std::nullopt;
  auto               fn = [](int a, int b, int c) { return a + b + c; };

  std::optional<int> out = ocudu::transform_optional(in, fn, 2, 3);
  EXPECT_FALSE(out.has_value()) << "Transforming an empty optional should return an empty optional.";

  int extra = 3;
  in        = 5;
  EXPECT_EQ(ocudu::transform_optional(in, fn, extra, extra), in.value() + 2 * extra)
      << "Error when transforming an optional with value.";
}

/// The transformation function return type is different from the base type of the original optional.
TEST(TransformOptionalTest, TypeChange)
{
  std::optional<int> in = std::nullopt;
  auto               fn = [](int a) { return static_cast<double>(a) / 3.0; };

  std::optional<int> out = ocudu::transform_optional(in, fn);
  EXPECT_FALSE(out.has_value()) << "Transforming an empty optional should return an empty optional.";

  in = 5;
  EXPECT_EQ(ocudu::transform_optional(in, fn), static_cast<double>(in.value()) / 3.0)
      << "Error when transforming an optional with value.";
}

/// Test evaluate_or with one extra argument.
TEST(TransformOptionalTest, EvaluateOr)
{
  std::optional<int> in    = std::nullopt;
  auto               fn    = [](int a, int b, int c) { return a + b + c; };
  int                d_out = 103;

  int out = ocudu::evaluate_or(in, d_out, fn, 2, 3);
  EXPECT_EQ(out, d_out) << "Evaluating an empty optional should return the default output.";

  int extra = 3;
  in        = 5;
  EXPECT_EQ(ocudu::evaluate_or(in, d_out, fn, extra, extra), in.value() + 2 * extra)
      << "Error when evaluating an optional with value.";
}
} // namespace
