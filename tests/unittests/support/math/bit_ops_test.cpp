// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/support/math/bit_ops.h"
#include <fmt/format.h>
#include <gtest/gtest.h>

using namespace ocudu;

/// Number of random values each test cross-checks against its reference implementation.
static constexpr unsigned nof_random_trials = 100;

/// Counts the bits set to one, one bit at a time.
template <typename Integer>
static unsigned ref_count_ones(Integer value)
{
  unsigned count = 0;
  for (unsigned i = 0, e = std::numeric_limits<Integer>::digits; i != e; ++i) {
    count += (value >> i) & 1U;
  }
  return count;
}

/// Reverses the bits of a 64-bit word, one bit at a time.
static uint64_t ref_bit_reverse(uint64_t value)
{
  static constexpr unsigned nof_bits = std::numeric_limits<uint64_t>::digits;

  uint64_t reversed = 0;
  for (unsigned i = 0; i != nof_bits; ++i) {
    reversed |= ((value >> i) & 1U) << (nof_bits - 1 - i);
  }
  return reversed;
}

template <typename T>
class bitmask_test : public ::testing::Test
{
  static_assert(std::is_unsigned_v<T>, "Invalid type T");

protected:
  using Integer                    = T;
  static constexpr size_t nof_bits = sizeof(Integer) * 8U;
};
using mask_integer_types = ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>;
TYPED_TEST_SUITE(bitmask_test, mask_integer_types);

TYPED_TEST(bitmask_test, lsb_ones)
{
  using IntegerType = typename TestFixture::Integer;
  // sanity checks.
  ASSERT_EQ(0, mask_lsb_ones<IntegerType>(0));
  ASSERT_EQ(static_cast<IntegerType>(-1), mask_lsb_ones<IntegerType>(this->nof_bits))
      << "for nof_bits=" << (unsigned)this->nof_bits;
  ASSERT_EQ(0b11, mask_lsb_ones<IntegerType>(2));

  // test all combinations.
  for (unsigned nof_ones = 0; nof_ones != this->nof_bits; ++nof_ones) {
    IntegerType expected = (static_cast<uint64_t>(1U) << nof_ones) - 1U;
    ASSERT_EQ(expected, mask_lsb_ones<IntegerType>(nof_ones)) << "for nof_ones=" << nof_ones;
  }
}

TYPED_TEST(bitmask_test, lsb_zeros)
{
  using IntegerType = typename TestFixture::Integer;
  // sanity checks.
  ASSERT_EQ((IntegerType)-1, mask_lsb_zeros<IntegerType>(0));
  ASSERT_EQ(0, mask_lsb_zeros<IntegerType>(this->nof_bits));

  // test all combinations.
  for (unsigned nof_zeros = 0; nof_zeros != this->nof_bits; ++nof_zeros) {
    IntegerType expected = (static_cast<uint64_t>(1U) << nof_zeros) - 1U;
    expected             = ~expected;
    ASSERT_EQ(expected, mask_lsb_zeros<IntegerType>(nof_zeros)) << "for nof_zeros=" << nof_zeros;
    ASSERT_EQ((IntegerType)~mask_lsb_ones<IntegerType>(nof_zeros), mask_lsb_zeros<IntegerType>(nof_zeros));
  }
}

TYPED_TEST(bitmask_test, msb_ones)
{
  using IntegerType = typename TestFixture::Integer;
  // sanity checks.
  ASSERT_EQ(0, mask_msb_ones<IntegerType>(0));
  ASSERT_EQ(static_cast<IntegerType>(-1), mask_msb_ones<IntegerType>(this->nof_bits));

  // test all combinations.
  for (unsigned nof_ones = 0; nof_ones != this->nof_bits; ++nof_ones) {
    IntegerType expected = 0;
    if (nof_ones > 0) {
      unsigned nof_lsb_zeros = this->nof_bits - nof_ones;
      expected               = ~((static_cast<IntegerType>(1U) << (nof_lsb_zeros)) - 1U);
    }
    ASSERT_EQ(expected, mask_msb_ones<IntegerType>(nof_ones)) << "for nof_ones=" << nof_ones;
  }
}

TYPED_TEST(bitmask_test, msb_zeros)
{
  using IntegerType = typename TestFixture::Integer;
  // sanity checks.
  ASSERT_EQ((IntegerType)-1, mask_msb_zeros<IntegerType>(0));
  ASSERT_EQ(0, mask_msb_zeros<IntegerType>(this->nof_bits));

  // test all combinations.
  for (unsigned nof_zeros = 0; nof_zeros != this->nof_bits; ++nof_zeros) {
    IntegerType expected = 0;
    if (nof_zeros > 0) {
      unsigned nof_lsb_ones = this->nof_bits - nof_zeros;
      expected              = ~((static_cast<IntegerType>(1U) << (nof_lsb_ones)) - 1U);
    }
    expected = ~expected;
    ASSERT_EQ(expected, mask_msb_zeros<IntegerType>(nof_zeros)) << "for nof_zeros=" << nof_zeros;
    ASSERT_EQ((IntegerType)~mask_lsb_ones<IntegerType>(nof_zeros), mask_lsb_zeros<IntegerType>(nof_zeros));
  }
}

TYPED_TEST(bitmask_test, first_lsb_one)
{
  using IntegerType = typename TestFixture::Integer;
  std::uniform_int_distribution<IntegerType> rd_int{0, std::numeric_limits<IntegerType>::max()};

  // sanity checks.
  ASSERT_EQ(std::numeric_limits<IntegerType>::digits, find_first_lsb_one<IntegerType>(0));
  ASSERT_EQ(0, find_first_lsb_one<IntegerType>(-1));
  ASSERT_EQ(0, find_first_lsb_one<IntegerType>(0b1));
  ASSERT_EQ(1, find_first_lsb_one<IntegerType>(0b10));
  ASSERT_EQ(0, find_first_lsb_one<IntegerType>(0b11));

  // test all combinations.
  for (unsigned one_idx = 0; one_idx != this->nof_bits; ++one_idx) {
    IntegerType single_bit = static_cast<IntegerType>(IntegerType{1} << one_idx);
    ASSERT_EQ(one_idx, find_first_lsb_one(single_bit));
    ASSERT_EQ(one_idx, find_first_lsb_one(mask_lsb_zeros<IntegerType>(one_idx)));

    // Bits above the lowest set one must not shift the result.
    IntegerType higher_bits = test_rng::uniform_int<IntegerType>() & mask_lsb_zeros<IntegerType>(one_idx + 1);
    IntegerType value       = static_cast<IntegerType>(single_bit | higher_bits);
    ASSERT_EQ(one_idx, find_first_lsb_one(value)) << fmt::format("for value {:#b}", value);
  }
}

TYPED_TEST(bitmask_test, first_msb_one)
{
  using IntegerType = typename TestFixture::Integer;

  // sanity checks.
  ASSERT_EQ(std::numeric_limits<IntegerType>::digits, find_first_msb_one<IntegerType>(0));
  ASSERT_EQ(this->nof_bits - 1, find_first_msb_one<IntegerType>(-1));
  ASSERT_EQ(0, find_first_msb_one<IntegerType>(0b1));
  ASSERT_EQ(1, find_first_msb_one<IntegerType>(0b10));
  ASSERT_EQ(1, find_first_msb_one<IntegerType>(0b11));

  // test all combinations.
  for (unsigned one_idx = 0; one_idx != this->nof_bits; ++one_idx) {
    IntegerType single_bit = static_cast<IntegerType>(IntegerType{1} << one_idx);
    ASSERT_EQ(one_idx, find_first_msb_one(single_bit));
    ASSERT_EQ(one_idx, find_first_msb_one(mask_lsb_ones<IntegerType>(one_idx + 1)));

    // Bits below the highest set one must not shift the result.
    IntegerType lower_bits = test_rng::uniform_int<IntegerType>() & mask_lsb_ones<IntegerType>(one_idx);
    IntegerType value      = static_cast<IntegerType>(single_bit | lower_bits);
    ASSERT_EQ(one_idx, find_first_msb_one(value)) << fmt::format("for value {:#b}", value);
  }
}

TYPED_TEST(bitmask_test, zero_msb_count)
{
  using IntegerType = typename TestFixture::Integer;

  // sanity checks.
  ASSERT_EQ(this->nof_bits, zero_msb_count<IntegerType>(0));
  ASSERT_EQ(0, zero_msb_count(std::numeric_limits<IntegerType>::max()));

  // test all combinations.
  for (unsigned one_idx = 0; one_idx != this->nof_bits; ++one_idx) {
    IntegerType single_bit     = static_cast<IntegerType>(IntegerType{1} << one_idx);
    unsigned    expected_zeros = this->nof_bits - 1 - one_idx;
    ASSERT_EQ(expected_zeros, zero_msb_count(single_bit));

    // Bits below the highest set one must not change the count.
    IntegerType lower_bits = test_rng::uniform_int<IntegerType>() & mask_lsb_ones<IntegerType>(one_idx);
    IntegerType value      = static_cast<IntegerType>(single_bit | lower_bits);
    ASSERT_EQ(expected_zeros, zero_msb_count(value)) << fmt::format("for value {:#b}", value);
  }
}

TYPED_TEST(bitmask_test, count_ones)
{
  using IntegerType = typename TestFixture::Integer;

  // sanity checks.
  ASSERT_EQ(0, ocudu::count_ones<IntegerType>(0));
  ASSERT_EQ(this->nof_bits, static_cast<unsigned>(ocudu::count_ones(std::numeric_limits<IntegerType>::max())));

  // test all combinations.
  for (unsigned nof_ones = 0; nof_ones != this->nof_bits + 1; ++nof_ones) {
    ASSERT_EQ(nof_ones, static_cast<unsigned>(ocudu::count_ones(mask_lsb_ones<IntegerType>(nof_ones))))
        << "for nof_ones=" << nof_ones;
    ASSERT_EQ(nof_ones, static_cast<unsigned>(ocudu::count_ones(mask_msb_ones<IntegerType>(nof_ones))))
        << "for nof_ones=" << nof_ones;
  }

  for (unsigned i = 0; i != nof_random_trials; ++i) {
    IntegerType value = test_rng::uniform_int<IntegerType>();
    ASSERT_EQ(ref_count_ones(value), static_cast<unsigned>(ocudu::count_ones(value)))
        << fmt::format("for value {:#b}", value);
  }
}

TEST(bit_reverse_test, single_bit_moves_to_the_mirrored_position)
{
  static constexpr unsigned nof_bits = std::numeric_limits<uint64_t>::digits;

  for (unsigned one_idx = 0; one_idx != nof_bits; ++one_idx) {
    ASSERT_EQ(uint64_t{1} << (nof_bits - 1 - one_idx), bit_reverse(uint64_t{1} << one_idx));
  }
}

TEST(bit_reverse_test, matches_reference)
{
  // sanity checks.
  ASSERT_EQ(0, bit_reverse(0));
  ASSERT_EQ(std::numeric_limits<uint64_t>::max(), bit_reverse(std::numeric_limits<uint64_t>::max()));

  for (unsigned i = 0; i != nof_random_trials; ++i) {
    uint64_t value = test_rng::uniform_int<uint64_t>();
    ASSERT_EQ(ref_bit_reverse(value), bit_reverse(value)) << fmt::format("for value {:#x}", value);
    ASSERT_EQ(value, bit_reverse(bit_reverse(value))) << fmt::format("for value {:#x}", value);
  }
}

TEST(reverse_byte_test, matches_reference)
{
  static constexpr unsigned nof_byte_values = 256;

  for (unsigned i = 0; i != nof_byte_values; ++i) {
    uint8_t byte = static_cast<uint8_t>(i);

    // The reversed byte is the top half of the reversed 64-bit word holding it.
    uint8_t expected = static_cast<uint8_t>(ref_bit_reverse(byte) >> 56U);

    ASSERT_EQ(expected, reverse_byte(byte)) << fmt::format("for byte {:#b}", byte);
    ASSERT_EQ(byte, reverse_byte(reverse_byte(byte))) << fmt::format("for byte {:#b}", byte);
  }
}
