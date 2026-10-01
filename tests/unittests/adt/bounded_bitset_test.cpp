// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/format.h"
#include "ocudu/adt/interval.h"
#include <bitset>
#include <gtest/gtest.h>

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

using namespace ocudu;

// ** bounded_bitset constexpr tests

static_assert(bounded_bitset<4>{}.max_size() == 4, "invalid max_size() method");
static_assert(bounded_bitset<4>{}.size() == 0, "invalid size() method");
static_assert(bounded_bitset<4>{}.empty(), "invalid empty() method");
static_assert(bounded_bitset<4>{{false, true, true}}.size() == 3, "invalid size() method");
static_assert(bounded_bitset<4>{{false, true, true}}.test(2), "invalid test() method");
static_assert(not bounded_bitset<4>{{false, true, true}}.test(0), "invalid test() method");
static_assert(std::is_trivially_copyable_v<bounded_bitset<4>>, "bounded_bitset should be trivially copyable");

// ** bounded_bitset runtime tests

template <typename BoundedBitset>
class bounded_bitset_tester : public ::testing::Test
{
protected:
  using bitset_type         = BoundedBitset;
  using reverse_bitset_type = bounded_bitset<bitset_type::max_size(), not bitset_type::bit_order()>;

  static constexpr unsigned max_size() { return bitset_type::max_size(); }

  unsigned get_random_size(unsigned min_val = 1, unsigned max_val = bitset_type::max_size()) const
  {
    ocudu_assert(max_val <= bitset_type::max_size(), "Invalid test bitset size argument");
    return test_rng::uniform_int<unsigned>(min_val, max_val);
  }

  bitset_type create_bitset_with_zeros(unsigned size) const { return bitset_type(size); }

  static bitset_type create_bitset_with_ones(unsigned size)
  {
    std::vector<bool> data(size, true);
    return bitset_type(data.begin(), data.end());
  }

  static bitset_type create_random_bitset(unsigned size)
  {
    std::vector<bool> data(size);
    for (auto it = data.begin(); it != data.end(); ++it) {
      *it = test_rng::bernoulli();
    }
    return bitset_type(data.begin(), data.end());
  }

  static std::vector<bool> create_random_vector(unsigned size)
  {
    std::vector<bool> vec(size);
    for (auto it = vec.begin(); it != vec.end(); ++it) {
      *it = test_rng::bernoulli();
    }
    return vec;
  }

  void test_all_zeros()
  {
    auto zero_mask = this->create_bitset_with_zeros(this->get_random_size());
    ASSERT_GT(zero_mask.size(), 0);
    ASSERT_EQ(0, zero_mask.count());
    ASSERT_TRUE(zero_mask.none());
    ASSERT_FALSE(zero_mask.any());
    ASSERT_FALSE(zero_mask.all());
  }

  void test_empty_bitset()
  {
    auto zero_sized_mask = this->create_bitset_with_zeros(0);
    ASSERT_EQ(this->max_size(), zero_sized_mask.max_size());
    ASSERT_EQ(0, zero_sized_mask.size());
    ASSERT_EQ(0, zero_sized_mask.count());
    ASSERT_TRUE(zero_sized_mask.none());
    ASSERT_FALSE(zero_sized_mask.any());
    ASSERT_TRUE(zero_sized_mask.all());

    zero_sized_mask.flip();
    ASSERT_TRUE(zero_sized_mask.none()) << "Flipping empty bitset should be a no-op";
    ASSERT_EQ(0, zero_sized_mask.size()) << "Flipping empty bitset should be a no-op";

    auto zero_mask = this->create_bitset_with_zeros(this->get_random_size());
    ASSERT_NE(zero_mask, zero_sized_mask);
    zero_sized_mask.resize(zero_mask.size());
    ASSERT_EQ(zero_mask, zero_sized_mask);
  }

  void test_initializer_list_constructor()
  {
    bitset_type bitmap = {true, true, true, true, false};
    ASSERT_EQ(bitmap.size(), 5);
    ASSERT_TRUE(bitmap.test(0));
    ASSERT_TRUE(bitmap.test(1));
    ASSERT_TRUE(bitmap.test(2));
    ASSERT_TRUE(bitmap.test(3));
    ASSERT_FALSE(bitmap.test(4));
  }

  void test_iterator_constructor()
  {
    std::vector<bool> data = this->create_random_vector(this->get_random_size());
    bitset_type       bitmap(data.begin(), data.end());
    ASSERT_EQ(bitmap.size(), data.size());

    for (unsigned i = 0; i != data.size(); ++i) {
      ASSERT_EQ(data[i], bitmap.test(i));
    }
  }

  void test_all_ones()
  {
    auto ones_bitmap = this->create_bitset_with_ones(this->get_random_size());

    ASSERT_TRUE(ones_bitmap.all());
    ASSERT_TRUE(ones_bitmap.any());
    ASSERT_FALSE(ones_bitmap.none());
    ASSERT_EQ(ones_bitmap.size(), ones_bitmap.count());
  }

  void test_flip()
  {
    auto bitmap          = this->create_random_bitset(this->get_random_size());
    auto original_bitmap = bitmap;
    bitmap.flip();

    ASSERT_GT(bitmap.size(), 0);
    ASSERT_EQ(bitmap.size(), original_bitmap.size());
    for (unsigned i = 0; i != bitmap.size(); ++i) {
      ASSERT_NE(bitmap.test(i), original_bitmap.test(i));
    }
  }

  void test_set_bit()
  {
    std::vector<bool> data = this->create_random_vector(this->get_random_size());
    bitset_type       bitmap(data.size());
    bitset_type       expected_bitmap(data.begin(), data.end());

    for (unsigned i = 0; i != data.size(); ++i) {
      if (data[i]) {
        bitmap.set(i);
      }
    }
    ASSERT_EQ(bitmap, expected_bitmap);
  }

  void test_bitwise_or()
  {
    bitset_type bitmap       = this->create_random_bitset(this->get_random_size());
    bitset_type zeros_bitmap = this->create_bitset_with_zeros(bitmap.size());
    bitset_type ones_bitmap  = this->create_bitset_with_ones(bitmap.size());

    ASSERT_EQ(bitmap | bitmap, bitmap);
    ASSERT_EQ(bitmap | zeros_bitmap, bitmap);
    ASSERT_EQ(bitmap | ones_bitmap, ones_bitmap);

    auto flipped_bitmap = bitmap;
    flipped_bitmap.flip();
    bitmap |= flipped_bitmap;
    ASSERT_EQ(bitmap, ones_bitmap);
  }

  void test_bitwise_and()
  {
    bitset_type bitmap       = this->create_random_bitset(this->get_random_size());
    bitset_type zeros_bitmap = this->create_bitset_with_zeros(bitmap.size());
    bitset_type ones_bitmap  = this->create_bitset_with_ones(bitmap.size());

    ASSERT_EQ(bitmap & bitmap, bitmap);
    ASSERT_EQ(bitmap & zeros_bitmap, zeros_bitmap);
    ASSERT_EQ(bitmap & ones_bitmap, bitmap);

    auto flipped_bitmap = bitmap;
    flipped_bitmap.flip();
    bitmap &= flipped_bitmap;
    ASSERT_EQ(bitmap, zeros_bitmap);
  }

  void test_any_range()
  {
    std::vector<bool> vec = this->create_random_vector(this->get_random_size());
    bitset_type       bitmap(vec.begin(), vec.end());

    for (unsigned l = 1; l < vec.size(); ++l) {
      for (unsigned i = 0; i < vec.size() - l; ++i) {
        bool expected_val = false;
        for (unsigned j = 0; j != l; ++j) {
          if (vec[i + j]) {
            expected_val = true;
            break;
          }
        }
        ASSERT_EQ(expected_val, bitmap.any(i, i + l)) << "l=" << l << " i=" << i;
      }
    }
  }

  void test_all_range()
  {
    std::vector<bool> vec = this->create_random_vector(this->get_random_size());
    bitset_type       bitmap(vec.begin(), vec.end());

    for (unsigned l = 1; l < vec.size(); ++l) {
      for (unsigned i = 0; i < vec.size() - l; ++i) {
        bool expected_val = true;
        for (unsigned j = 0; j != l; ++j) {
          if (not vec[i + j]) {
            expected_val = false;
            break;
          }
        }
        ASSERT_EQ(expected_val, bitmap.all(i, i + l)) << "l=" << l << " i=" << i;
      }
    }
  }

  void test_is_subset_of()
  {
    std::vector<bool> vec1 = this->create_random_vector(this->get_random_size());
    std::vector<bool> vec2 = this->create_random_vector(vec1.size());
    bitset_type       bitmap1(vec1.begin(), vec1.end());
    bitset_type       bitmap2(vec2.begin(), vec2.end());

    for (unsigned l = 1; l < vec1.size(); ++l) {
      for (unsigned i = 0; i < vec1.size() - l; ++i) {
        bool expected_val = true;
        for (unsigned j = 0; j != l; ++j) {
          if (vec1[i + j] and not vec2[i + j]) {
            expected_val = false;
            break;
          }
        }
        ASSERT_EQ(expected_val, bitmap1.is_subset_of(bitmap2, i, i + l)) << "l=" << l << " i=" << i;
      }
    }

    const auto zeros_bitmap = this->create_bitset_with_zeros(bitmap1.size());
    const auto ones_bitmap  = this->create_bitset_with_ones(bitmap1.size());

    ASSERT_TRUE(zeros_bitmap.is_subset_of(zeros_bitmap, 0, zeros_bitmap.size()));
    ASSERT_TRUE(zeros_bitmap.is_subset_of(bitmap1, 0, bitmap1.size())) << "The empty set is a subset of any other set";
    ASSERT_TRUE(bitmap1.is_subset_of(ones_bitmap, 0, bitmap1.size())) << "Any set is a subset of the universal set";
    ASSERT_TRUE(bitmap1.is_subset_of(bitmap1, 0, bitmap1.size())) << "A set is always a subset of itself";
    ASSERT_FALSE(ones_bitmap.is_subset_of(zeros_bitmap, 0, ones_bitmap.size()))
        << "The universal set is not a subset of the empty set";
  }

  void test_fill_ones()
  {
    unsigned   bitset_size  = this->get_random_size();
    const auto zeros_bitmap = this->create_bitset_with_zeros(bitset_size);

    for (unsigned l = 1; l < bitset_size; ++l) {
      for (unsigned i = 0; i < bitset_size - l; ++i) {
        auto bitmap = zeros_bitmap;
        bitmap.fill(i, i + l);
        ASSERT_FALSE(bitmap.any(0, i));
        ASSERT_TRUE(bitmap.all(i, i + l)) << "l=" << l << " i=" << i;
        ASSERT_FALSE(bitmap.any(i + l, bitset_size));
      }
    }
  }

  void test_fill_zeros()
  {
    unsigned   bitset_size = this->get_random_size();
    const auto ones_bitmap = this->create_bitset_with_ones(bitset_size);

    for (unsigned l = 1; l < bitset_size; ++l) {
      for (unsigned i = 0; i < bitset_size - l; ++i) {
        auto bitmap = ones_bitmap;
        bitmap.fill(i, i + l, false);
        ASSERT_TRUE(bitmap.all(0, i)) << "l=" << l << " i=" << i;
        ASSERT_FALSE(bitmap.any(i, i + l));
        ASSERT_TRUE(bitmap.all(i + l, bitset_size)) << "l=" << l << " i=" << i;
      }
    }
  }

  void test_slice()
  {
    using big_bitset_type                   = bitset_type;
    unsigned                big_bitset_size = this->get_random_size();
    const std::vector<bool> vec             = this->create_random_vector(big_bitset_size);
    big_bitset_type         big_bitmap(vec.begin(), vec.end());

    constexpr size_t N_small   = bitset_type::max_size() / 2;
    using small_bitset_type    = bounded_bitset<N_small, big_bitset_type::bit_order()>;
    unsigned small_bitset_size = test_rng::uniform_int<unsigned>(0, std::min((unsigned)N_small, big_bitset_size - 1));
    unsigned offset            = test_rng::uniform_int<unsigned>(0, small_bitset_size);
    unsigned end_offset        = test_rng::uniform_int<unsigned>(offset, small_bitset_size);

    small_bitset_type small_bitmap = big_bitmap.template slice<N_small>(offset, end_offset);
    ASSERT_EQ(end_offset - offset, small_bitmap.size());

    for (unsigned i = 0; i != small_bitmap.size(); ++i) {
      ASSERT_EQ(small_bitmap.test(i), big_bitmap.test(i + offset)) << "mismatch at position " << i;
    }
  }

  void test_resize_keeps_existing_bit_values()
  {
    const unsigned large_size   = this->get_random_size(2);
    const unsigned smaller_size = this->get_random_size(1, large_size - 1);

    auto ones_bitmap = this->create_bitset_with_ones(large_size);
    ASSERT_EQ(ones_bitmap.count(), large_size);
    ones_bitmap.resize(smaller_size);
    ASSERT_EQ(ones_bitmap.count(), smaller_size);
    ones_bitmap.resize(large_size);
    ASSERT_EQ(ones_bitmap.count(), smaller_size);
  }

  void test_copy_preserves_size_and_values()
  {
    const unsigned size1 = this->get_random_size();
    const unsigned size2 = this->get_random_size();

    bitset_type bitset1 = this->create_random_bitset(size1);
    bitset_type bitset2 = this->create_random_bitset(size2);

    bitset2 = bitset1;
    ASSERT_EQ(bitset1, bitset2);
  }

  void test_binary_format_is_mirror_of_binary_format_reverse()
  {
    static constexpr size_t         N        = bitset_type::max_size();
    static constexpr bool           BitOrder = bitset_type::bit_order();
    std::vector<bool>               vec      = this->create_random_vector(this->get_random_size());
    bounded_bitset<N, BitOrder>     bitmap(vec.begin(), vec.end());
    bounded_bitset<N, not BitOrder> bitmap_reversed(vec.begin(), vec.end());

    std::string str          = fmt::format("{:b}", bitmap);
    std::string str_reverse  = fmt::format("{:br}", bitmap);
    std::string str2         = fmt::format("{:b}", bitmap_reversed);
    std::string str2_reverse = fmt::format("{:br}", bitmap_reversed);

    ASSERT_TRUE(std::equal(str.begin(), str.end(), str_reverse.rbegin(), str_reverse.rend()));
    ASSERT_TRUE(std::equal(str2.begin(), str2.end(), str2_reverse.rbegin(), str2_reverse.rend()));
    ASSERT_EQ(str, str2_reverse);
  }

  void test_bit_positions_format_is_agnostic_to_reverse()
  {
    static constexpr size_t         N        = bitset_type::max_size();
    static constexpr bool           BitOrder = bitset_type::bit_order();
    std::vector<bool>               vec      = this->create_random_vector(this->get_random_size());
    bounded_bitset<N, BitOrder>     bitmap(vec.begin(), vec.end());
    bounded_bitset<N, not BitOrder> bitmap_reversed(vec.begin(), vec.end());

    std::string str         = fmt::format("{:n}", bitmap);
    std::string str_reverse = fmt::format("{:n}", bitmap_reversed);

    ASSERT_EQ(str, str_reverse);
  }

  void test_hex_format_is_mirror_of_hex_format_reverse()
  {
    static constexpr size_t         N        = bitset_type::max_size();
    static constexpr bool           BitOrder = bitset_type::bit_order();
    std::vector<bool>               vec      = this->create_random_vector(this->get_random_size());
    bounded_bitset<N, BitOrder>     bitmap(vec.begin(), vec.end());
    bounded_bitset<N, not BitOrder> bitmap_reversed(vec.begin(), vec.end());

    ASSERT_EQ(fmt::format("{:x}", bitmap), fmt::format("{:xr}", bitmap_reversed));
    ASSERT_EQ(fmt::format("{:xr}", bitmap), fmt::format("{:x}", bitmap_reversed));
  }

  void test_push_back()
  {
    std::vector<bool> vec = this->create_random_vector(this->get_random_size());
    bitset_type       bitmap;

    unsigned count = 0;
    for (bool v : vec) {
      bitmap.push_back(v);
      ASSERT_EQ(bitmap.size(), count + 1);
      ASSERT_EQ(bitmap.test(count), v);
      ++count;
    }

    ASSERT_EQ(bitmap.size(), vec.size());
    std::vector<bool> actual(vec.size());
    for (unsigned i = 0; i < vec.size(); ++i) {
      actual[i] = bitmap.test(i);
    }
    ASSERT_EQ(actual, vec);
  }

  void test_push_back_multiple_bits()
  {
    std::vector<bool> vec      = this->create_random_vector(this->get_random_size());
    const unsigned    bit_step = test_rng::uniform_int<unsigned>(1U, std::min(32U, (unsigned)vec.size()));
    bitset_type       bitmap;

    for (unsigned offset = 0; offset < vec.size(); offset += bit_step) {
      unsigned nof_packed = std::min(bit_step, (unsigned)vec.size() - offset);
      uint32_t to_pack    = 0;
      for (unsigned i = 0; i != nof_packed; ++i) {
        to_pack <<= 1;
        to_pack |= vec[offset + i];
      }
      bitmap.push_back(to_pack, nof_packed);
    }

    ASSERT_EQ(bitmap.size(), vec.size());
    std::vector<bool> actual(vec.size());
    for (unsigned i = 0; i < vec.size(); ++i) {
      actual[i] = bitmap.test(i);
    }
    ASSERT_EQ(actual, vec);
  }
};
using bitset_types = ::testing::Types<bounded_bitset<25>,
                                      bounded_bitset<25, true>,
                                      bounded_bitset<32>,
                                      bounded_bitset<32, true>,
                                      bounded_bitset<63>,
                                      bounded_bitset<63, true>,
                                      bounded_bitset<64>,
                                      bounded_bitset<64, true>,
                                      bounded_bitset<120>,
                                      bounded_bitset<120, true>,
                                      bounded_bitset<127>,
                                      bounded_bitset<127, true>,
                                      bounded_bitset<512>,
                                      bounded_bitset<512, true>>;
TYPED_TEST_SUITE(bounded_bitset_tester, bitset_types);

// A single TYPED_TEST calling out-of-line test_* methods, rather than one TYPED_TEST per case, avoids
// per-declaration gtest type-registration overhead and keeps the compiler from inlining every case's
// body into one function, both large -O2 compile-memory costs across the 14 instantiated types.
TYPED_TEST(bounded_bitset_tester, all_tests_combined)
{
  this->test_all_zeros();
  this->test_empty_bitset();
  this->test_initializer_list_constructor();
  this->test_iterator_constructor();
  this->test_all_ones();
  this->test_flip();
  this->test_set_bit();
  this->test_bitwise_or();
  this->test_bitwise_and();
  this->test_any_range();
  this->test_all_range();
  this->test_is_subset_of();
  this->test_fill_ones();
  this->test_fill_zeros();
  this->test_slice();
  this->test_resize_keeps_existing_bit_values();
  this->test_copy_preserves_size_and_values();
  this->test_binary_format_is_mirror_of_binary_format_reverse();
  this->test_bit_positions_format_is_agnostic_to_reverse();
  this->test_hex_format_is_mirror_of_hex_format_reverse();
  this->test_push_back();
  this->test_push_back_multiple_bits();
}

TEST(bounded_bitset_test, bitset_integer_conversion_consistent_with_std_bitset)
{
  unsigned bitset_size = 23;
  unsigned nof_ones    = test_rng::uniform_int<unsigned>(1, bitset_size);
  unsigned integermask = mask_lsb_ones<unsigned>(nof_ones);

  bounded_bitset<25> mask(bitset_size);
  mask.from_uint64(integermask);
  std::bitset<25> std_mask(integermask);

  ASSERT_EQ(bitset_size, mask.size());
  ASSERT_EQ(25, std_mask.size());
  ASSERT_EQ(nof_ones, mask.count());
  ASSERT_EQ(nof_ones, std_mask.count());
  ASSERT_TRUE(mask.any());
  ASSERT_TRUE(std_mask.any());
  ASSERT_FALSE(mask.none());
  ASSERT_FALSE(std_mask.none());
  ASSERT_EQ(integermask, mask.to_uint64());
  ASSERT_EQ(integermask, std_mask.to_ulong());
}

TEST(bounded_bitset_test, bitset_integer_conversion_with_template_arg)
{
  unsigned bitset_size = 23;

  bounded_bitset<25>       mask(bitset_size);
  bounded_bitset<25, true> mask_reversed(bitset_size);
  std::bitset<25>          std_mask;

  mask.set(3);
  mask_reversed.set(3);
  std_mask.set(3);

  ASSERT_EQ(mask.to_uint64(), 0b1000);
  ASSERT_EQ(mask_reversed.to_uint64(), 1U << (bitset_size - 4));
  ASSERT_EQ(std_mask.to_ulong(), 0b1000);
}

TEST(bounded_bitset_test, bitset_integer_conversion_with_large_integer)
{
  bounded_bitset<48> mask(48);
  mask.from_uint64(278099133963U);

  ASSERT_EQ(mask.to_uint64(), 278099133963U);
}

TEST(bounded_bitset_test, to_uint64_and_from_uint64_are_inverse_lsb)
{
  bounded_bitset<18> mask(18);
  mask.set(0);
  mask.set(2);
  mask.set(7);
  mask.set(12);

  const uint64_t     packed = mask.to_uint64();
  bounded_bitset<18> restored(18);
  restored.from_uint64(packed);

  ASSERT_EQ(mask, restored);
}

TEST(bounded_bitset_test, to_uint64_and_from_uint64_are_inverse_msb)
{
  bounded_bitset<18, true> mask(18);
  mask.set(0);
  mask.set(2);
  mask.set(7);
  mask.set(12);

  const uint64_t           packed = mask.to_uint64();
  bounded_bitset<18, true> restored(18);
  restored.from_uint64(packed);

  ASSERT_EQ(mask, restored);
}

TEST(bounded_bitset_test, one_word_bitset_format)
{
  bounded_bitset<25> bitset(23);
  bitset.set(0);
  bitset.set(5);
  bounded_bitset<25, true> bitset_reversed(23);
  bitset_reversed.set(0);
  bitset_reversed.set(5);

  ASSERT_EQ(fmt::format("{:b}", bitset), "00000000000000000100001");
  ASSERT_EQ(fmt::format("{:br}", bitset), "10000100000000000000000");
  ASSERT_EQ(fmt::format("{:x}", bitset), "000021");
  ASSERT_EQ(fmt::format("{:xr}", bitset), "420000");
  ASSERT_EQ(fmt::format("{:n}", bitset), "[0, 5]");
  ASSERT_EQ(fmt::format("{:n}", bitset_reversed), "[0, 5]");
  ASSERT_EQ(fmt::format("{:b}", bitset), fmt::format("{:br}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:br}", bitset), fmt::format("{:b}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:x}", bitset), fmt::format("{:xr}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:xr}", bitset), fmt::format("{:x}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:n}", bitset), fmt::format("{:nr}", bitset));

  bitset.set(22);
  bitset_reversed.set(22);
  ASSERT_EQ(fmt::format("{:b}", bitset), "10000000000000000100001");
  ASSERT_EQ(fmt::format("{:br}", bitset), "10000100000000000000001");
  ASSERT_EQ(fmt::format("{:x}", bitset), "400021");
  ASSERT_EQ(fmt::format("{:xr}", bitset), "420001");
  ASSERT_EQ(fmt::format("{:n}", bitset), "[0, 5, 22]");
  ASSERT_EQ(fmt::format("{:n}", bitset_reversed), "[0, 5, 22]");
  ASSERT_EQ(fmt::format("{:b}", bitset), fmt::format("{:br}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:br}", bitset), fmt::format("{:b}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:x}", bitset), fmt::format("{:xr}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:xr}", bitset), fmt::format("{:x}", bitset_reversed));
}

TEST(bounded_bitset_test, empty_bitset_format)
{
  bounded_bitset<25> bitset;

  ASSERT_EQ(fmt::format("{:b}", bitset), "");
  ASSERT_EQ(fmt::format("{:br}", bitset), "");
  ASSERT_EQ(fmt::format("{:x}", bitset), "");
  ASSERT_EQ(fmt::format("{:xr}", bitset), "");
  ASSERT_EQ(fmt::format("{:n}", bitset), "empty");
  ASSERT_EQ(fmt::format("{:i}", bitset), "{}");
}

TEST(bounded_bitset_test, all_false_bitset_format)
{
  bounded_bitset<25> bitset(23);

  ASSERT_EQ(fmt::format("{:b}", bitset), "00000000000000000000000");
  ASSERT_EQ(fmt::format("{:br}", bitset), "00000000000000000000000");
  ASSERT_EQ(fmt::format("{:x}", bitset), "000000");
  ASSERT_EQ(fmt::format("{:xr}", bitset), "000000");
  ASSERT_EQ(fmt::format("{:n}", bitset), "none");
  ASSERT_EQ(fmt::format("{:i}", bitset), "{}");
}

TEST(bounded_bitset_test, contiguous_bitset_format)
{
  bounded_bitset<25> bitset(9);
  bitset.set(1);
  bitset.set(2);
  bitset.set(3);
  bounded_bitset<25, true> bitset_reversed(9);
  bitset_reversed.set(1);
  bitset_reversed.set(2);
  bitset_reversed.set(3);

  ASSERT_EQ(fmt::format("{:b}", bitset), "000001110");
  ASSERT_EQ(fmt::format("{:br}", bitset), "011100000");
  ASSERT_EQ(fmt::format("{:x}", bitset), "00e");
  ASSERT_EQ(fmt::format("{:xr}", bitset), "0e0");
  ASSERT_EQ(fmt::format("{:n}", bitset), "[1, 4)");
  ASSERT_EQ(fmt::format("{:n}", bitset_reversed), "[1, 4)");
  ASSERT_EQ(fmt::format("{:i}", bitset), "{[1, 4)}");
  ASSERT_EQ(fmt::format("{:i}", bitset_reversed), "{[1, 4)}");
  ASSERT_EQ(fmt::format("{:b}", bitset), fmt::format("{:br}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:br}", bitset), fmt::format("{:b}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:x}", bitset), fmt::format("{:xr}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:xr}", bitset), fmt::format("{:x}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:n}", bitset), fmt::format("{:nr}", bitset));
  ASSERT_EQ(fmt::format("{:i}", bitset), fmt::format("{:ir}", bitset));

  bitset.reset();
  bitset_reversed.reset();
  bitset.set(1);
  bitset_reversed.set(1);
  bitset.set(4);
  bitset_reversed.set(4);
  bitset.set(5);
  bitset_reversed.set(5);
  bitset.set(7);
  bitset_reversed.set(7);
  bitset.set(8);
  bitset_reversed.set(8);

  ASSERT_EQ(fmt::format("{:b}", bitset), "110110010");
  ASSERT_EQ(fmt::format("{:br}", bitset), "010011011");
  ASSERT_EQ(fmt::format("{:x}", bitset), "1b2");
  ASSERT_EQ(fmt::format("{:xr}", bitset), "09b");
  ASSERT_EQ(fmt::format("{:n}", bitset), "[1, 4, 5, 7, 8]");
  ASSERT_EQ(fmt::format("{:n}", bitset_reversed), "[1, 4, 5, 7, 8]");
  ASSERT_EQ(fmt::format("{:i}", bitset), "{1, [4, 6), [7, 9)}");
  ASSERT_EQ(fmt::format("{:i}", bitset_reversed), "{1, [4, 6), [7, 9)}");
  ASSERT_EQ(fmt::format("{:b}", bitset), fmt::format("{:br}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:br}", bitset), fmt::format("{:b}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:x}", bitset), fmt::format("{:xr}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:xr}", bitset), fmt::format("{:x}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:n}", bitset), fmt::format("{:nr}", bitset));
  ASSERT_EQ(fmt::format("{:i}", bitset), fmt::format("{:ir}", bitset));
}

TEST(bounded_bitset_test, two_word_bitset_format)
{
  bounded_bitset<101> bitset(100);
  bitset.set(0);
  bitset.set(5);
  bounded_bitset<101, true> bitset_reversed(100);
  bitset_reversed.set(0);
  bitset_reversed.set(5);

  ASSERT_EQ(fmt::format("{:b}", bitset),
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100001");
  ASSERT_EQ(fmt::format("{:br}", bitset),
            "1000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
  ASSERT_EQ(fmt::format("{:x}", bitset), "0000000000000000000000021");
  ASSERT_EQ(fmt::format("{:xr}", bitset), "8400000000000000000000000");
  ASSERT_EQ(fmt::format("{:n}", bitset), "[0, 5]");
  ASSERT_EQ(fmt::format("{:n}", bitset_reversed), "[0, 5]");
  ASSERT_EQ(fmt::format("{:b}", bitset), fmt::format("{:br}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:br}", bitset), fmt::format("{:b}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:x}", bitset), fmt::format("{:xr}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:xr}", bitset), fmt::format("{:x}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:n}", bitset), fmt::format("{:nr}", bitset));

  bitset.set(99);
  bitset_reversed.set(99);
  ASSERT_EQ(fmt::format("{:b}", bitset),
            "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100001");
  ASSERT_EQ(fmt::format("{:br}", bitset),
            "1000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001");
  ASSERT_EQ(fmt::format("{:x}", bitset), "8000000000000000000000021");
  ASSERT_EQ(fmt::format("{:xr}", bitset), "8400000000000000000000001");
  ASSERT_EQ(fmt::format("{:n}", bitset), "[0, 5, 99]");
  ASSERT_EQ(fmt::format("{:n}", bitset_reversed), "[0, 5, 99]");
  ASSERT_EQ(fmt::format("{:b}", bitset), fmt::format("{:br}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:br}", bitset), fmt::format("{:b}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:x}", bitset), fmt::format("{:xr}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:xr}", bitset), fmt::format("{:x}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:n}", bitset), fmt::format("{:nr}", bitset));
}

TEST(BoundedBitset, bitwise_resize)
{
  {
    bounded_bitset<100> bitset;
    ASSERT_TRUE(bitset.none() and bitset.size() == 0);

    bitset.resize(100);
    ASSERT_TRUE(bitset.none() and bitset.size() == 100);
    bitset.fill(0, 100);
    ASSERT_TRUE(bitset.all() and bitset.size() == 100);

    bitset.resize(25);
    ASSERT_EQ(bitset.to_uint64(), 0x1ffffff);
    ASSERT_TRUE(bitset.all() and bitset.size() == 25); // keeps the data it had for the non-erased bits

    bitset.resize(100);
    ASSERT_TRUE(bitset.count() == 25 and bitset.size() == 100);
  }

  {
    // TEST: Reverse case
    bounded_bitset<100, true> bitset;
    ASSERT_TRUE(bitset.none() and bitset.size() == 0);

    bitset.resize(100);
    ASSERT_TRUE(bitset.none() and bitset.size() == 100);
    bitset.fill(0, 100);
    ASSERT_TRUE(bitset.all() and bitset.size() == 100);

    bitset.resize(25);
    ASSERT_EQ(bitset.to_uint64(), 0x1ffffff);
    ASSERT_TRUE(bitset.all() and bitset.size() == 25); // keeps the data it had for the non-erased bits

    bitset.resize(100);
    ASSERT_TRUE(bitset.count() == 25 and bitset.size() == 100);
  }
}

template <bool reversed>
void test_bitset_find()
{
  {
    bounded_bitset<25, reversed> bitset(6);

    // 0b000000
    ASSERT_TRUE(bitset.find_lowest(0, bitset.size(), false) == 0);
    ASSERT_TRUE(bitset.find_lowest(0, bitset.size(), true) == -1);

    // 0b000100
    bitset.set(2);
    ASSERT_TRUE(bitset.find_lowest(0, 6) == 2);
    ASSERT_TRUE(bitset.find_lowest(0, 6, false) == 0);
    ASSERT_TRUE(bitset.find_lowest(3, 6) == -1);
    ASSERT_TRUE(bitset.find_lowest(3, 6, false) == 3);

    // 0b000101
    bitset.set(0);
    ASSERT_TRUE(bitset.find_lowest(0, 6) == 0);
    ASSERT_TRUE(bitset.find_lowest(0, 6, false) == 1);
    ASSERT_TRUE(bitset.find_lowest(3, 6) == -1);
    ASSERT_TRUE(bitset.find_lowest(3, 6, false) == 3);

    // 0b100101
    bitset.set(5);
    ASSERT_TRUE(bitset.find_lowest(0, 6) == 0);
    ASSERT_TRUE(bitset.find_lowest(0, 6, false) == 1);
    ASSERT_TRUE(bitset.find_lowest(3, 6) == 5);

    // 0b111111
    bitset.fill(0, 6);
    ASSERT_TRUE(bitset.find_lowest(0, 6) == 0);
    ASSERT_TRUE(bitset.find_lowest(0, 6, false) == -1);
    ASSERT_TRUE(bitset.find_lowest(3, 6, false) == -1);
  }
  {
    bounded_bitset<100, reversed> bitset(95);

    // 0b0...0
    ASSERT_TRUE(bitset.find_lowest(0, bitset.size()) == -1);

    // 0b1000...
    bitset.set(94);
    ASSERT_TRUE(bitset.find_lowest(0, 93) == -1);
    ASSERT_TRUE(bitset.find_lowest(0, bitset.size()) == 94);

    // 0b1000...010
    bitset.set(1);
    ASSERT_TRUE(bitset.find_lowest(0, bitset.size()) == 1);
    ASSERT_TRUE(bitset.find_lowest(1, bitset.size()) == 1);
    ASSERT_TRUE(bitset.find_lowest(2, bitset.size()) == 94);

    // 0b11..11
    bitset.fill(0, bitset.size());
    ASSERT_TRUE(bitset.find_lowest(0, bitset.size()) == 0);
    ASSERT_TRUE(bitset.find_lowest(5, bitset.size()) == 5);
  }
  {
    bounded_bitset<100, reversed> bitset(95);

    // 0b0...0
    ASSERT_TRUE(bitset.find_lowest() == -1);

    // 0b1000...
    bitset.set(94);
    ASSERT_TRUE(bitset.find_lowest() == 94);

    // 0b1000...010
    bitset.set(1);
    ASSERT_TRUE(bitset.find_lowest() == 1);

    // 0b11..11
    bitset.fill(0, bitset.size());
    ASSERT_TRUE(bitset.find_lowest() == 0);
  }
  {
    bounded_bitset<100, reversed> bitset(95);

    // 0b0...0
    ASSERT_TRUE(bitset.find_highest() == -1);

    // 0b1000...
    bitset.set(94);
    ASSERT_EQ(bitset.find_highest(), 94);

    // 0b1000...010
    bitset.set(1);
    ASSERT_EQ(bitset.find_highest(), 94);

    // 0b11..11
    bitset.fill(0, bitset.size());
    ASSERT_EQ(bitset.find_highest(), 94);
  }
}

TEST(BoundedBitset, bitset_find)
{
  test_bitset_find<false>();
  test_bitset_find<true>();
}

TEST(BoundedBitset, is_contiguous)
{
  // Test contiguous condition 1. No bit set.
  {
    ASSERT_TRUE(bounded_bitset<10>({0, 0, 0, 0}).is_contiguous());
  }
  // Test contiguous condition 2. One bit set.
  {
    ASSERT_TRUE(bounded_bitset<10>({1, 0, 0, 0}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({0, 0, 1, 0}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({0, 1, 0, 0}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({0, 0, 0, 1}).is_contiguous());
  }
  // Test contiguous condition 3. All set bits are contiguous.
  {
    ASSERT_TRUE(bounded_bitset<10>({1, 1, 0, 0}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({1, 1, 1, 0}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({1, 1, 1, 1}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({0, 1, 1, 1}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({0, 0, 1, 1}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({0, 1, 1, 0}).is_contiguous());
  }
  // Not contiguous.
  {
    ASSERT_FALSE(bounded_bitset<10>({1, 0, 1, 1}).is_contiguous());
    ASSERT_FALSE(bounded_bitset<10>({1, 1, 0, 1}).is_contiguous());
    ASSERT_FALSE(bounded_bitset<10>({1, 0, 1, 1}).is_contiguous());
    ASSERT_FALSE(bounded_bitset<10>({1, 1, 0, 1}).is_contiguous());
    ASSERT_FALSE(bounded_bitset<10>({1, 0, 0, 1}).is_contiguous());
  }
}

TEST(BoundedBitset, push_back)
{
  {
    bounded_bitset<10> bitset;
    bitset.push_back(true);
    bitset.push_back(false);
    bitset.push_back(true);
    bitset.push_back(true);
    ASSERT_EQ(bitset.size(), 4);
    ASSERT_TRUE(bitset.test(0));
    ASSERT_FALSE(bitset.test(1));
    ASSERT_EQ(bitset, bounded_bitset<10>({1, 0, 1, 1}));
  }
  {
    bounded_bitset<10> bitset;
    bitset.push_back(0xbU, 4);
    ASSERT_EQ(bitset, bounded_bitset<10>({1, 0, 1, 1}));
  }
}

TEST(BoundedBitset, push_back_many_bits)
{
  {
    bounded_bitset<10> bitset;
    bitset.push_back(0b10U, 2);
    bitset.push_back(0b11U, 2);
    ASSERT_EQ(bitset.size(), 4);
    ASSERT_TRUE(bitset.test(0));
    ASSERT_FALSE(bitset.test(1));
    ASSERT_EQ(bitset, bounded_bitset<10>({1, 0, 1, 1}));
    static_vector<bool, 10> expected_vec = {1, 0, 1, 1};
    static_vector<bool, 10> unpacked(4);
    bitset.to_unpacked_bits(span<bool>{unpacked.data(), 4});
    ASSERT_EQ(unpacked, expected_vec);
  }
  {
    bounded_bitset<10> bitset;
    bitset.push_back(0xbU, 4);
    ASSERT_EQ(bitset, bounded_bitset<10>({1, 0, 1, 1}));
  }
}

TEST(BoundedBitset, extract)
{
  static constexpr uint64_t            nof_values = 100;
  bounded_bitset<64 * nof_values>      bitset;
  std::vector<std::array<uint64_t, 3>> data;

  for (unsigned i_value = 0, offset = 0; i_value != nof_values; ++i_value) {
    uint64_t size  = test_rng::uniform_int(1, 64);
    uint64_t value = (test_rng::uniform_int<uint64_t>() & mask_lsb_ones<uint64_t>(size));

    bitset.push_back(value, size);

    data.push_back({offset, size, value});

    offset += size;
  }

  for (const auto& entry : data) {
    uint64_t offset = entry[0];
    uint64_t size   = entry[1];
    uint64_t value  = entry[2];

    ASSERT_EQ(bitset.extract<uint64_t>(offset, size), value);

    offset += size;
  }
}

TEST(BoundedBitset, extract_inverted)
{
  static constexpr uint64_t             nof_values = 10;
  bounded_bitset<64 * nof_values, true> bitset;
  std::vector<std::array<uint64_t, 3>>  data;

  for (unsigned i_value = 0, offset = 0; i_value != nof_values; ++i_value) {
    uint64_t size  = test_rng::uniform_int(1, 64);
    uint64_t value = (test_rng::uniform_int<uint64_t>() & mask_lsb_ones<uint64_t>(size));

    bitset.push_back(value, size);

    data.push_back({offset, size, value});

    offset += size;
  }

  for (const auto& entry : data) {
    uint64_t offset = entry[0];
    uint64_t size   = entry[1];
    uint64_t value  = entry[2];

    ASSERT_EQ(bitset.extract<uint64_t>(offset, size), value);

    offset += size;
  }
}

TEST(BoundedBitset, fold_and_accumulate)
{
  size_t fold_size = 20;

  bounded_bitset<105> big_bitmap(100);
  for (size_t i = 0; i < big_bitmap.size(); i += fold_size + 1) {
    big_bitmap.set(i);
  }

  bounded_bitset<21> fold_bitset = fold_and_accumulate<21>(big_bitmap, fold_size);
  ASSERT_EQ(big_bitmap.size() / fold_size, fold_bitset.count());
  ASSERT_EQ(fold_bitset.count(), big_bitmap.count());
  ASSERT_TRUE(fold_bitset.is_contiguous());
  ASSERT_EQ(0, fold_bitset.find_lowest());
}

TEST(BoundedBitset, for_each)
{
  bounded_bitset<10>  mask   = {false, true, false, true, false, true, false, true, false, true};
  std::array<int, 10> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  std::vector<int>    output;

  mask.for_each(0, mask.size(), [&output, &values](int n) { output.emplace_back(values[n]); });
  ASSERT_EQ(output, std::vector<int>({2, 4, 6, 8, 10}));

  output.resize(0);
  mask.for_each(0, mask.size(), [&output, &values](int n) { output.emplace_back(values[n]); }, false);
  ASSERT_EQ(output, std::vector<int>({1, 3, 5, 7, 9}));

  output.resize(0);
  mask.for_each(2, mask.size(), [&output, &values](int n) { output.emplace_back(values[n]); });
  ASSERT_EQ(output, std::vector<int>({4, 6, 8, 10}));

  output.resize(0);
  mask.for_each(0, 9, [&output, &values](int n) { output.emplace_back(values[n]); });
  ASSERT_EQ(output, std::vector<int>({2, 4, 6, 8}));
}

TEST(BoundedBitset, for_each_interval)
{
  std::vector<interval<unsigned>> intervals = {{1, 5}, {7, 9}, {15, 20}};

  bounded_bitset<32> bitset(20);
  for (interval<unsigned> interv : intervals) {
    bitset.fill(interv.start(), interv.stop());
  }

  for_each_interval(bitset, [n = 0, &intervals](size_t start, size_t stop) mutable {
    ASSERT_EQ(intervals[n].start(), start);
    ASSERT_EQ(intervals[n].stop(), stop);
    ++n;
  });

  for_each_interval(bitset, 2, 19, [n = 0U, &intervals](size_t start, size_t stop) mutable {
    unsigned expected_start = intervals[n].start();
    unsigned expected_stop  = intervals[n].stop();
    if (n == 0) {
      expected_start = 2;
    }
    if (n == (intervals.size() - 1U)) {
      expected_stop = 19;
    }
    ASSERT_EQ(expected_start, start);
    ASSERT_EQ(expected_stop, stop);
    ++n;
  });
}

TEST(bounded_bitset_test, to_packed_bits_one_byte)
{
  bounded_bitset<10>       bitset{true, true, false, false, true};
  bounded_bitset<10, true> bitset_rev{true, true, false, false, true};

  std::array<uint8_t, 1> packed_bits = {};
  ASSERT_EQ(bitset.to_packed_bits(span<uint8_t>{packed_bits}), 1);
  ASSERT_EQ(packed_bits[0], 0b00010011);
  ASSERT_EQ(bitset_rev.to_packed_bits(span<uint8_t>{packed_bits}), 1);
  ASSERT_EQ(packed_bits[0], 0b11001000);
}

TEST(bounded_bitset_test, to_packed_bits_two_byte)
{
  bounded_bitset<20>       bitset(15);
  bounded_bitset<20, true> bitset_rev(15);
  bitset.set(0);
  bitset.set(1);
  bitset.set(9);
  bitset_rev.set(0);
  bitset_rev.set(1);
  bitset_rev.set(9);

  std::array<uint8_t, 3> packed_bits = {}, packed_bits2 = {};
  ASSERT_EQ(bitset.to_packed_bits(span<uint8_t>{packed_bits}), 2);
  std::array<uint8_t, 2> expected_packed_bits = {0b00000011, 0b00000010};
  ASSERT_TRUE(std::equal(expected_packed_bits.begin(), expected_packed_bits.end(), packed_bits.begin()));
  ASSERT_EQ(bitset_rev.to_packed_bits(span<uint8_t>{packed_bits2}), 2);
  std::array<uint8_t, 2> expected_packed_bits2 = {0b11000000, 0b01000000};
  ASSERT_TRUE(std::equal(expected_packed_bits2.begin(), expected_packed_bits2.end(), packed_bits2.begin()));
}

TEST(bounded_bitset_test, to_packed_bits_one_word)
{
  bounded_bitset<80>       bitset(64);
  bounded_bitset<80, true> bitset_rev(64);
  bitset.set(0);
  bitset.set(1);
  bitset.set(9);
  bitset.set(31);
  bitset.set(48);
  bitset_rev.set(0);
  bitset_rev.set(1);
  bitset_rev.set(9);
  bitset_rev.set(31);
  bitset_rev.set(48);

  std::array<uint8_t, 8> packed_bits = {}, packed_bits2 = {};
  ASSERT_EQ(bitset.to_packed_bits(span<uint8_t>{packed_bits}), 8);
  std::array<uint8_t, 8> expected_packed_bits = {
      0b00000011, 0b00000010, 0b00000000, 0b10000000, 0b00000000, 0b00000000, 0b00000001, 0b00000000};
  ASSERT_TRUE(std::equal(expected_packed_bits.begin(), expected_packed_bits.end(), packed_bits.begin()));

  ASSERT_EQ(bitset_rev.to_packed_bits(span<uint8_t>{packed_bits2}), 8);
  std::array<uint8_t, 8> expected_packed_bits2 = {
      0b11000000, 0b01000000, 0b00000000, 0b00000001, 0b00000000, 0b00000000, 0b10000000, 0b00000000};
  ASSERT_TRUE(std::equal(expected_packed_bits2.begin(), expected_packed_bits2.end(), packed_bits2.begin()));
}

TEST(bounded_bitset_test, bit_positions_to_bitset)
{
  std::vector<unsigned> positions = {1, 2, 5};
  auto                  bset      = bit_positions_to_bitset<7>(positions);
  ASSERT_EQ(bset.size(), 6);
  ASSERT_EQ(bset.count(), 3);
  ASSERT_TRUE(bset.test(1));
  ASSERT_TRUE(bset.test(2));
  ASSERT_TRUE(bset.test(5));
}
