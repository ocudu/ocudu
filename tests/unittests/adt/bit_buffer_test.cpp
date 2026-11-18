// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/bit_buffer.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

namespace {

class bit_buffer_insert_test : public ::testing::TestWithParam<unsigned>
{
protected:
  static constexpr unsigned max_offset = 16;
  static constexpr unsigned buffer_len = max_offset + 32;

  std::mt19937 rgen{0};
};

} // namespace

TEST_P(bit_buffer_insert_test, wide_insert_matches_bitwise_insert)
{
  const unsigned count = GetParam();
  for (unsigned offset = 0; offset != max_offset; ++offset) {
    uint32_t value = rgen();
    if (count < 32) {
      value &= (1U << count) - 1U;
    }

    dynamic_bit_buffer expected(buffer_len);
    dynamic_bit_buffer actual(buffer_len);
    // Fill both with the same background so the bits around the inserted range are checked too.
    for (unsigned i_bit = 0; i_bit != buffer_len; ++i_bit) {
      unsigned bit = rgen() & 1U;
      expected.insert(bit, i_bit, 1);
      actual.insert(bit, i_bit, 1);
    }

    for (unsigned i_bit = 0; i_bit != count; ++i_bit) {
      expected.insert((value >> (count - 1 - i_bit)) & 1U, offset + i_bit, 1);
    }
    actual.insert(value, offset, count);

    ASSERT_EQ(actual, expected) << fmt::format("count={} offset={} value={:#x}", count, offset, value);
  }
}

INSTANTIATE_TEST_SUITE_P(bit_buffer_insert_test,
                         bit_buffer_insert_test,
                         ::testing::Values(1, 8, 9, 12, 16, 17, 24, 31, 32));
