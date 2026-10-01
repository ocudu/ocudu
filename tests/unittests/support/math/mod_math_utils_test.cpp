// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/math/detail/type_utils.h"
#include "ocudu/support/math/mod_math_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

static_assert(std::is_same_v<smallest_signed_fit_t<(1 << 7) - 1>, int8_t>, "Incorrect smallest_signed_fit_t");
static_assert(std::is_same_v<smallest_signed_fit_t<128>, int16_t>, "Incorrect smallest_signed_fit_t");
static_assert(std::is_same_v<smallest_signed_fit_t<(1 << 15) - 1>, int16_t>, "Incorrect smallest_signed_fit_t");
static_assert(std::is_same_v<smallest_signed_fit_t<static_cast<int>((1U << 31U) - 1U)>, int32_t>,
              "Incorrect smallest_signed_fit_t");

TEST(mod_math_utils_test, signed_modular_difference_no_wrap_around)
{
  constexpr unsigned a = 4;
  constexpr unsigned b = 2;
  constexpr unsigned N = 12;

  static_assert(signed_modular_difference<N>(a, b) == 2, "Incorrect signed_modular_difference");
  static_assert(signed_modular_difference(a, b, N) == 2, "Incorrect signed_modular_difference");
  static_assert(signed_modular_difference<N>(b, a) == -2, "Incorrect signed_modular_difference");
  static_assert(signed_modular_difference(b, a, N) == -2, "Incorrect signed_modular_difference");
}

TEST(mod_math_utils_test, signed_modular_difference_wrap_around)
{
  constexpr unsigned a = 2;
  constexpr unsigned b = 10;
  constexpr unsigned N = 12;

  ASSERT_EQ(signed_modular_difference<N>(a, b), 4);
  static_assert(signed_modular_difference<N>(a, b) == 4, "Incorrect signed_modular_difference");
  static_assert(signed_modular_difference(a, b, N) == 4, "Incorrect signed_modular_difference");
  static_assert(signed_modular_difference<N>(b, a) == -4, "Incorrect signed_modular_difference");
  static_assert(signed_modular_difference(b, a, N) == -4, "Incorrect signed_modular_difference");
}

TEST(mod_math_utils_test, signed_modular_difference_near_limit)
{
  constexpr uint8_t a = 1;
  constexpr uint8_t b = 255;
  constexpr uint8_t N = std::numeric_limits<uint8_t>::max();

  static_assert(std::is_same_v<decltype(signed_modular_difference<N>(a, b)), int8_t>,
                "Incorrect return type for signed_modular_difference");
  static_assert(signed_modular_difference<N>(a, b) == 1, "Incorrect signed_modular_difference");
  static_assert(signed_modular_difference(a, b, N) == 1, "Incorrect signed_modular_difference");
  static_assert(signed_modular_difference<N>(b, a) == -1, "Incorrect signed_modular_difference");
  static_assert(signed_modular_difference(b, a, N) == -1, "Incorrect signed_modular_difference");
}

TEST(mod_math_utils_test, modular_min_max)
{
  constexpr unsigned a = 2;
  constexpr unsigned b = 10;
  constexpr unsigned N = 12;

  static_assert(modular_min<N>(a, b) == b, "Incorrect modular_min");
  static_assert(modular_min(a, b, N) == b, "Incorrect modular_min");
  static_assert(modular_min<N>(b, a) == b, "Incorrect modular_min");
  static_assert(modular_min(b, a, N) == b, "Incorrect modular_min");

  static_assert(modular_max<N>(a, b) == a, "Incorrect modular_max");
  static_assert(modular_max(a, b, N) == a, "Incorrect modular_max");
  static_assert(modular_max<N>(b, a) == a, "Incorrect modular_max");
  static_assert(modular_max(b, a, N) == a, "Incorrect modular_max");
}
