// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/fixed_bitset.h"
#include "ocudu/adt/format.h"
#include <algorithm>
#include <gtest/gtest.h>

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

using namespace ocudu;

// ** Compile-time checks for fixed_size_bitset

static_assert(fixed_size_bitset<4>{}.max_size() == 4, "invalid max_size()");
static_assert(fixed_size_bitset<4>{}.size() == 4, "invalid size()");
static_assert(!fixed_size_bitset<4>{}.empty(), "size-4 bitset should not be empty");
static_assert(fixed_size_bitset<1>{}.size() == 1, "invalid size()");
static_assert(fixed_size_bitset<64>{}.size() == 64, "invalid size()");
static_assert(fixed_size_bitset<128>{}.size() == 128, "invalid size()");
static_assert(!fixed_size_bitset<4>{}.bit_order(), "invalid default bit_order()");
static_assert(fixed_size_bitset<4, true>{}.bit_order(), "invalid bit_order()");

// ** Typed test suite for fixed_size_bitset

template <typename FixedBitset>
class fixed_bitset_tester : public ::testing::Test
{
protected:
  using bitset_type = FixedBitset;

  static constexpr size_t N = bitset_type::max_size();

  static bitset_type create_all_zeros() { return bitset_type{}; }

  static bitset_type create_all_ones()
  {
    bitset_type b{};
    b.fill(true);
    return b;
  }

  static bitset_type create_random()
  {
    bitset_type b{};
    for (size_t i = 0; i != N; ++i) {
      b.set(i, test_rng::bernoulli());
    }
    return b;
  }

  static std::vector<bool> create_random_vector()
  {
    std::vector<bool> vec(N);
    for (size_t i = 0; i != N; ++i) {
      vec[i] = test_rng::bernoulli();
    }
    return vec;
  }
};

using fixed_bitset_types = ::testing::Types<fixed_size_bitset<1>,
                                            fixed_size_bitset<1, true>,
                                            fixed_size_bitset<25>,
                                            fixed_size_bitset<25, true>,
                                            fixed_size_bitset<32>,
                                            fixed_size_bitset<32, true>,
                                            fixed_size_bitset<63>,
                                            fixed_size_bitset<63, true>,
                                            fixed_size_bitset<64>,
                                            fixed_size_bitset<64, true>,
                                            fixed_size_bitset<120>,
                                            fixed_size_bitset<120, true>,
                                            fixed_size_bitset<127>,
                                            fixed_size_bitset<127, true>>;
TYPED_TEST_SUITE(fixed_bitset_tester, fixed_bitset_types);

// ** Default construction: all bits are 0

TYPED_TEST(fixed_bitset_tester, default_construction_all_zeros)
{
  auto b = this->create_all_zeros();
  ASSERT_EQ(b.count(), 0);
  ASSERT_TRUE(b.none());
  ASSERT_FALSE(b.any());
  ASSERT_EQ(b.size(), this->N);
  for (size_t i = 0; i < this->N; ++i) {
    ASSERT_FALSE(b.test(i));
  }
}

// ** Iterator constructor

TYPED_TEST(fixed_bitset_tester, iterator_constructor)
{
  std::vector<bool>                 data = this->create_random_vector();
  typename TestFixture::bitset_type bitmap(data.begin(), data.end());
  ASSERT_EQ(bitmap.size(), data.size());

  for (size_t i = 0; i != data.size(); ++i) {
    ASSERT_EQ(data[i], bitmap.test(i)) << "Mismatch at bit " << i;
  }
}

// ** Initializer list constructor

TYPED_TEST(fixed_bitset_tester, initializer_list_constructor)
{
  // Build a bitset of size N from a full initializer list.
  // We can only have a fixed-size initializer list in code, so test with N=25 outside the typed test.
  // For the typed test, we just verify that the iterator constructor gives the same result as fill+reset.
  auto                              data = this->create_random_vector();
  typename TestFixture::bitset_type b1(data.begin(), data.end());
  typename TestFixture::bitset_type b2{};
  for (size_t i = 0; i != this->N; ++i) {
    if (data[i]) {
      b2.set(i);
    }
  }
  ASSERT_EQ(b1, b2);
}

// ** set / reset / test / flip (individual bits)

TYPED_TEST(fixed_bitset_tester, set_reset_test)
{
  typename TestFixture::bitset_type b{};

  for (size_t i = 0; i < this->N; ++i) {
    b.set(i);
    ASSERT_TRUE(b.test(i));
    b.reset(i);
    ASSERT_FALSE(b.test(i));
  }
}

TYPED_TEST(fixed_bitset_tester, set_with_value)
{
  typename TestFixture::bitset_type b{};

  for (size_t i = 0; i < this->N; ++i) {
    b.set(i, true);
    ASSERT_TRUE(b.test(i));
    b.set(i, false);
    ASSERT_FALSE(b.test(i));
  }
}

TYPED_TEST(fixed_bitset_tester, flip_single_bit)
{
  typename TestFixture::bitset_type b{};

  for (size_t i = 0; i < this->N; ++i) {
    ASSERT_FALSE(b.test(i));
    b.flip(i);
    ASSERT_TRUE(b.test(i));
    b.flip(i);
    ASSERT_FALSE(b.test(i));
  }
}

// ** fill(bool) and reset()

TYPED_TEST(fixed_bitset_tester, fill_all_ones)
{
  auto b = this->create_all_ones();
  ASSERT_TRUE(b.all());
  ASSERT_EQ(b.count(), this->N);
}

TYPED_TEST(fixed_bitset_tester, fill_false_clears_all)
{
  auto b = this->create_all_ones();
  b.fill(false);
  ASSERT_TRUE(b.none());
  ASSERT_EQ(b.count(), 0);
}

TYPED_TEST(fixed_bitset_tester, reset_clears_all)
{
  auto b = this->create_all_ones();
  b.reset();
  ASSERT_TRUE(b.none());
  ASSERT_EQ(b.count(), 0);
}

// ** fill(start, end, value)

TYPED_TEST(fixed_bitset_tester, fill_range_ones)
{
  if (this->N < 2) {
    GTEST_SKIP();
  }
  typename TestFixture::bitset_type b{};
  size_t                            start = 0;
  size_t                            end   = this->N;
  b.fill(start, end, true);
  ASSERT_TRUE(b.all(start, end));
  ASSERT_EQ(b.count(), this->N);
}

TYPED_TEST(fixed_bitset_tester, fill_range_zeros)
{
  if (this->N < 2) {
    GTEST_SKIP();
  }
  auto   b     = this->create_all_ones();
  size_t start = 0;
  size_t end   = this->N;
  b.fill(start, end, false);
  ASSERT_FALSE(b.any(start, end));
  ASSERT_EQ(b.count(), 0);
}

TYPED_TEST(fixed_bitset_tester, fill_partial_range)
{
  if (this->N < 3) {
    GTEST_SKIP();
  }
  typename TestFixture::bitset_type b{};
  size_t                            mid = this->N / 2;
  b.fill(1, mid);
  ASSERT_FALSE(b.test(0));
  ASSERT_TRUE(b.all(1, mid));
  if (mid < this->N) {
    ASSERT_FALSE(b.any(mid, this->N));
  }
}

// ** flip()

TYPED_TEST(fixed_bitset_tester, flip_all)
{
  auto b      = this->create_random();
  auto b_orig = b;
  b.flip();
  ASSERT_EQ(b.size(), b_orig.size());
  for (size_t i = 0; i < this->N; ++i) {
    ASSERT_NE(b.test(i), b_orig.test(i));
  }
  // Double flip restores original.
  b.flip();
  ASSERT_EQ(b, b_orig);
}

// ** any / all / none

TYPED_TEST(fixed_bitset_tester, all_zeros_predicates)
{
  auto b = this->create_all_zeros();
  ASSERT_TRUE(b.none());
  ASSERT_FALSE(b.any());
  ASSERT_EQ(b.all(), this->N == 0); // all() on empty-ish bitset — N>0 so false
}

TYPED_TEST(fixed_bitset_tester, all_ones_predicates)
{
  auto b = this->create_all_ones();
  ASSERT_FALSE(b.none());
  ASSERT_TRUE(b.any());
  ASSERT_TRUE(b.all());
}

TYPED_TEST(fixed_bitset_tester, any_all_none_range)
{
  if (this->N < 4) {
    GTEST_SKIP();
  }
  typename TestFixture::bitset_type b{};
  size_t                            half = this->N / 2;
  b.fill(0, half, true);

  ASSERT_TRUE(b.any(0, half));
  ASSERT_TRUE(b.all(0, half));
  ASSERT_TRUE(b.none(half, this->N));
  ASSERT_FALSE(b.any(half, this->N));
}

// ** count

TYPED_TEST(fixed_bitset_tester, count)
{
  auto                              data = this->create_random_vector();
  size_t                            ones = std::count(data.begin(), data.end(), true);
  typename TestFixture::bitset_type b(data.begin(), data.end());
  ASSERT_EQ(b.count(), ones);
}

// ** find_lowest / find_highest

TYPED_TEST(fixed_bitset_tester, find_lowest_all_zeros)
{
  auto b = this->create_all_zeros();
  ASSERT_EQ(b.find_lowest(true), -1);
  ASSERT_EQ(b.find_lowest(false), 0);
}

TYPED_TEST(fixed_bitset_tester, find_highest_all_zeros)
{
  auto b = this->create_all_zeros();
  ASSERT_EQ(b.find_highest(true), -1);
  ASSERT_EQ(b.find_highest(false), static_cast<int>(this->N) - 1);
}

TYPED_TEST(fixed_bitset_tester, find_lowest_all_ones)
{
  auto b = this->create_all_ones();
  ASSERT_EQ(b.find_lowest(true), 0);
  ASSERT_EQ(b.find_lowest(false), -1);
}

TYPED_TEST(fixed_bitset_tester, find_highest_all_ones)
{
  auto b = this->create_all_ones();
  ASSERT_EQ(b.find_highest(true), static_cast<int>(this->N) - 1);
  ASSERT_EQ(b.find_highest(false), -1);
}

TYPED_TEST(fixed_bitset_tester, find_lowest_single_set_bit)
{
  for (size_t pos = 0; pos < this->N; ++pos) {
    typename TestFixture::bitset_type b{};
    b.set(pos);
    ASSERT_EQ(b.find_lowest(true), static_cast<int>(pos));
    ASSERT_EQ(b.find_highest(true), static_cast<int>(pos));
  }
}

// ** for_each (only for LowestInfoBitIsMSB=false)

TYPED_TEST(fixed_bitset_tester, for_each_ones)
{
  if constexpr (!TestFixture::bitset_type::bit_order()) {
    auto                              data = this->create_random_vector();
    typename TestFixture::bitset_type b(data.begin(), data.end());

    std::vector<size_t> positions;
    b.for_each(0, this->N, [&positions](size_t pos) { positions.push_back(pos); });

    std::vector<size_t> expected;
    for (size_t i = 0; i < this->N; ++i) {
      if (data[i]) {
        expected.push_back(i);
      }
    }
    ASSERT_EQ(positions, expected);
  }
}

TYPED_TEST(fixed_bitset_tester, for_each_zeros)
{
  if constexpr (!TestFixture::bitset_type::bit_order()) {
    auto                              data = this->create_random_vector();
    typename TestFixture::bitset_type b(data.begin(), data.end());

    std::vector<size_t> positions;
    b.for_each(0, this->N, [&positions](size_t pos) { positions.push_back(pos); }, false);

    std::vector<size_t> expected;
    for (size_t i = 0; i < this->N; ++i) {
      if (!data[i]) {
        expected.push_back(i);
      }
    }
    ASSERT_EQ(positions, expected);
  }
}

// ** is_contiguous

TYPED_TEST(fixed_bitset_tester, is_contiguous_all_zeros)
{
  auto b = this->create_all_zeros();
  ASSERT_TRUE(b.is_contiguous(true));
}

TYPED_TEST(fixed_bitset_tester, is_contiguous_all_ones)
{
  auto b = this->create_all_ones();
  ASSERT_TRUE(b.is_contiguous(true));
}

TYPED_TEST(fixed_bitset_tester, is_contiguous_single_bit)
{
  for (size_t pos = 0; pos < this->N; ++pos) {
    typename TestFixture::bitset_type b{};
    b.set(pos);
    ASSERT_TRUE(b.is_contiguous(true));
  }
}

TYPED_TEST(fixed_bitset_tester, is_contiguous_range)
{
  if (this->N < 4) {
    GTEST_SKIP();
  }
  typename TestFixture::bitset_type b{};
  b.fill(1, this->N / 2);
  ASSERT_TRUE(b.is_contiguous(true));
}

TYPED_TEST(fixed_bitset_tester, not_contiguous_two_separated_bits)
{
  if (this->N < 3) {
    GTEST_SKIP();
  }
  typename TestFixture::bitset_type b{};
  b.set(0);
  b.set(this->N - 1);
  if (this->N > 2) {
    ASSERT_FALSE(b.is_contiguous(true));
  }
}

// ** operator== / operator!=

TYPED_TEST(fixed_bitset_tester, equality)
{
  auto b1 = this->create_random();
  auto b2 = b1;
  ASSERT_EQ(b1, b2);
  ASSERT_FALSE(b1 != b2);
}

TYPED_TEST(fixed_bitset_tester, inequality)
{
  auto b1 = this->create_all_zeros();
  auto b2 = this->create_all_ones();
  ASSERT_NE(b1, b2);
  ASSERT_FALSE(b1 == b2);
}

// ** operator|= / operator&=

TYPED_TEST(fixed_bitset_tester, bitwise_or)
{
  auto zeros = this->create_all_zeros();
  auto ones  = this->create_all_ones();
  auto b     = this->create_random();

  ASSERT_EQ(b | zeros, b);
  ASSERT_EQ(zeros | b, b);
  ASSERT_EQ(b | ones, ones);
  ASSERT_EQ(ones | b, ones);
  ASSERT_EQ(b | b, b);

  auto flipped = ~b;
  ASSERT_EQ(b | flipped, ones);
}

TYPED_TEST(fixed_bitset_tester, bitwise_and)
{
  auto zeros = this->create_all_zeros();
  auto ones  = this->create_all_ones();
  auto b     = this->create_random();

  ASSERT_EQ(b & zeros, zeros);
  ASSERT_EQ(zeros & b, zeros);
  ASSERT_EQ(b & ones, b);
  ASSERT_EQ(ones & b, b);
  ASSERT_EQ(b & b, b);

  auto flipped = ~b;
  ASSERT_EQ(b & flipped, zeros);
}

TYPED_TEST(fixed_bitset_tester, bitwise_not)
{
  auto b     = this->create_random();
  auto b_not = ~b;
  ASSERT_EQ(~b_not, b);
  ASSERT_EQ(b & b_not, this->create_all_zeros());
  ASSERT_EQ(b | b_not, this->create_all_ones());
}

// ** to_uint64 / from_uint64 (only for bitsets that fit in 64 bits)

TYPED_TEST(fixed_bitset_tester, to_uint64_from_uint64_roundtrip)
{
  if (this->N > 64) {
    GTEST_SKIP();
  }
  auto                              b = this->create_random();
  auto                              v = b.to_uint64();
  typename TestFixture::bitset_type restored{};
  restored.from_uint64(v);
  ASSERT_EQ(b, restored);
}

// ** get_bit_positions

TYPED_TEST(fixed_bitset_tester, get_bit_positions)
{
  auto                              data = this->create_random_vector();
  typename TestFixture::bitset_type b(data.begin(), data.end());

  auto                positions = b.get_bit_positions(true);
  std::vector<size_t> expected;
  for (size_t i = 0; i < this->N; ++i) {
    if (data[i]) {
      expected.push_back(i);
    }
  }

  ASSERT_EQ(positions.size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    ASSERT_EQ(positions[i], expected[i]);
  }
}

// ** Non-typed tests for specific sizes and formatting

TEST(fixed_bitset_test, initializer_list_constructor_25)
{
  fixed_size_bitset<5> b = {true, false, true, true, false};
  ASSERT_TRUE(b.test(0));
  ASSERT_FALSE(b.test(1));
  ASSERT_TRUE(b.test(2));
  ASSERT_TRUE(b.test(3));
  ASSERT_FALSE(b.test(4));
  ASSERT_EQ(b.count(), 3);
}

TEST(fixed_bitset_test, binary_format_lsb_first)
{
  fixed_size_bitset<5> b{};
  b.set(0);
  b.set(2);
  // bits are printed from highest index to lowest: pos 4,3,2,1,0 -> "00101"
  ASSERT_EQ(fmt::format("{:b}", b), "00101");
  ASSERT_EQ(fmt::format("{:br}", b), "10100");
}

TEST(fixed_bitset_test, binary_format_msb_first)
{
  fixed_size_bitset<5, true> b{};
  b.set(0);
  b.set(2);
  // LowestInfoBitIsMSB: printed from lowest index to highest -> "10100"
  ASSERT_EQ(fmt::format("{:b}", b), "10100");
  ASSERT_EQ(fmt::format("{:br}", b), "00101");
}

TEST(fixed_bitset_test, hex_format)
{
  fixed_size_bitset<8> b{};
  b.set(0);
  b.set(1);
  ASSERT_EQ(fmt::format("{:x}", b), "03");
}

TEST(fixed_bitset_test, bit_positions_format_none)
{
  fixed_size_bitset<5> b{};
  ASSERT_EQ(fmt::format("{:n}", b), "none");
}

TEST(fixed_bitset_test, bit_positions_format_single)
{
  fixed_size_bitset<5> b{};
  b.set(3);
  ASSERT_EQ(fmt::format("{:n}", b), "3");
}

TEST(fixed_bitset_test, bit_positions_format_range)
{
  fixed_size_bitset<10> b{};
  b.set(2);
  b.set(3);
  b.set(4);
  ASSERT_EQ(fmt::format("{:n}", b), "[2, 5)");
}

TEST(fixed_bitset_test, bit_positions_format_scattered)
{
  fixed_size_bitset<10> b{};
  b.set(1);
  b.set(3);
  b.set(7);
  ASSERT_EQ(fmt::format("{:n}", b), "[1, 3, 7]");
}

TEST(fixed_bitset_test, to_uint64_msb_order)
{
  fixed_size_bitset<23, true> mask{};
  mask.set(3);
  // With MSB mode, bit 3 is shifted left.
  ASSERT_EQ(mask.to_uint64(), 1U << (23 - 4));
}

TEST(fixed_bitset_test, to_uint64_lsb_order)
{
  fixed_size_bitset<23> mask{};
  mask.set(3);
  ASSERT_EQ(mask.to_uint64(), 0b1000U);
}

TEST(fixed_bitset_test, large_bitset_count)
{
  fixed_size_bitset<127> b{};
  for (size_t i = 0; i < 127; i += 2) {
    b.set(i);
  }
  ASSERT_EQ(b.count(), 64);
}

TEST(fixed_bitset_test, find_lowest_range)
{
  fixed_size_bitset<20> b{};
  b.set(5);
  b.set(10);
  b.set(15);

  ASSERT_EQ(b.find_lowest(0, 20, true), 5);
  ASSERT_EQ(b.find_lowest(6, 20, true), 10);
  ASSERT_EQ(b.find_lowest(11, 20, true), 15);
  ASSERT_EQ(b.find_lowest(16, 20, true), -1);
}

TEST(fixed_bitset_test, find_highest_range)
{
  fixed_size_bitset<20> b{};
  b.set(5);
  b.set(10);
  b.set(15);

  ASSERT_EQ(b.find_highest(0, 20, true), 15);
  ASSERT_EQ(b.find_highest(0, 15, true), 10);
  ASSERT_EQ(b.find_highest(0, 10, true), 5);
  ASSERT_EQ(b.find_highest(0, 5, true), -1);
}

TEST(fixed_bitset_test, lsb_msb_format_are_mirrors)
{
  std::vector<bool> data(25);
  for (size_t i = 0; i != data.size(); ++i) {
    data[i] = test_rng::bernoulli();
  }

  fixed_size_bitset<25>       bitmap(data.begin(), data.end());
  fixed_size_bitset<25, true> bitmap_reversed(data.begin(), data.end());

  std::string str          = fmt::format("{:b}", bitmap);
  std::string str_reverse  = fmt::format("{:br}", bitmap);
  std::string str2         = fmt::format("{:b}", bitmap_reversed);
  std::string str2_reverse = fmt::format("{:br}", bitmap_reversed);

  ASSERT_TRUE(std::equal(str.begin(), str.end(), str_reverse.rbegin(), str_reverse.rend()));
  ASSERT_TRUE(std::equal(str2.begin(), str2.end(), str2_reverse.rbegin(), str2_reverse.rend()));
  ASSERT_EQ(str, str2_reverse);
}
