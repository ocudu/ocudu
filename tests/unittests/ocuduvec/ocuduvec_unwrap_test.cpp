// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Unwrap test.
///
/// The test takes an array of complex numbers and checks that the output of the \c unwrap_arguments function returns
/// the unwrapped phase angles.

#include "ocudu/adt/complex.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocuduvec/sc_prod.h"
#include "ocudu/ocuduvec/unwrap.h"
#include "fmt/ostream.h"
#include <algorithm>
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocuduvec;

static constexpr float tolerance = 1.0e-6;

namespace ocudu {

bool operator==(span<const float> lhs, span<const float> rhs)
{
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](float lhs_val, float rhs_val) {
    return (std::abs(lhs_val - rhs_val) < tolerance);
  });
}

std::ostream& operator<<(std::ostream& os, span<const float> data)
{
  fmt::print(os, "{}", data);
  return os;
}

} // namespace ocudu

TEST(UnwrapTest, RampUp)
{
  constexpr unsigned        length = 10;
  std::array<float, length> phases = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  ocuduvec::sc_prod(phases, phases, M_PI / 4);

  std::array<cf_t, length> c_numbers;

  std::transform(phases.cbegin(), phases.cend(), c_numbers.begin(), [](float a) { return std::polar(1.0F, a); });

  std::array<float, length> out;

  unwrap_arguments(out, c_numbers);

  ASSERT_EQ(span<const float>(phases), span<const float>(out));
}

TEST(UnwrapTest, RampDown)
{
  constexpr unsigned        length = 10;
  std::array<float, length> phases = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  ocuduvec::sc_prod(phases, phases, -M_PI / 4);

  std::array<cf_t, length> c_numbers;

  std::transform(phases.cbegin(), phases.cend(), c_numbers.begin(), [](float a) { return std::polar(1.0F, a); });

  std::array<float, length> out;

  unwrap_arguments(out, c_numbers);

  ASSERT_EQ(span<const float>(phases), span<const float>(out));
}
