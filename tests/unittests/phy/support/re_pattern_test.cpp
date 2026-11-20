// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#if defined(__GNUC__) && (__GNUC__ > 10) && (__GNUC__ < 14) && defined(__OPTIMIZE__)
#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma message "GCC versions greater than 10 give a likely false array-bounds alarm."
#endif // defined(__GNUC__) && defined(__GNUC_MINOR__)

#include "ocudu/adt/format.h"
#include "ocudu/phy/support/re_pattern.h"
#include "ocudu/ran/cyclic_prefix.h"
#include <gtest/gtest.h>

using namespace ocudu;

// Tests that two patterns with subcarrier mask in common are merged into one.
TEST(re_pattern_test, merge_even)
{
  unsigned rb_begin  = 1;
  unsigned rb_end    = 50;
  unsigned rb_stride = 50;

  re_pattern_list list;

  // Reset list.
  list.clear();

  // Create first pattern containing:
  // - even subcarrier indexes, and
  // - even symbol indexes.
  re_pattern pattern_1 = {};
  pattern_1.crb_mask   = crb_bitmap(rb_end);
  for (unsigned i_prb = rb_begin; i_prb < rb_end; i_prb += rb_stride) {
    pattern_1.crb_mask.set(i_prb);
  }
  for (unsigned k = 0; k != NOF_SUBCARRIERS_PER_RB; ++k) {
    pattern_1.re_mask.set(k, k % 2 == 0);
  }
  for (unsigned l = 0; l != MAX_NSYMB_PER_SLOT; ++l) {
    pattern_1.symbols.set(l, l % 2 == 0);
  }
  list.merge(pattern_1);
  ASSERT_EQ(list.get_nof_entries(), 1);

  // Create second pattern:
  // - Even subcarrier indexes, and
  // - odd symbol indexes.
  re_pattern pattern_2 = pattern_1;
  pattern_2.symbols    = symbol_slot_mask();
  for (unsigned l = 0; l != MAX_NSYMB_PER_SLOT; ++l) {
    pattern_2.symbols.set(l, l % 2 == 1); // Only odd symbols
  }
  list.merge(pattern_2);
  ASSERT_EQ(list.get_nof_entries(), 1);

  // The pattern should be repeated for each symbol.
  for (unsigned l = 0; l != MAX_NSYMB_PER_SLOT; ++l) {
    // Create mask with all entries to false
    bounded_bitset<MAX_NOF_SUBCARRIERS> mask(MAX_NOF_SUBCARRIERS);

    // Set include mask.
    list.get_inclusion_mask(mask, l);

    // For each subcarrier it checks the mask.
    for (unsigned k = 0; k != MAX_NOF_SUBCARRIERS; ++k) {
      bool gold = false;

      // if it is in the RB range, then check the subcarrier index.
      if ((k >= rb_begin * NOF_SUBCARRIERS_PER_RB) && (k < rb_end * NOF_SUBCARRIERS_PER_RB) &&
          ((k / NOF_SUBCARRIERS_PER_RB - rb_begin) % rb_stride == 0)) {
        // Even subcarrier
        gold = (k % 2 == 0);
      }
      ASSERT_EQ(mask.test(k), gold);
    }

    // Set exclude.
    list.get_exclusion_mask(mask, l);

    // All the subcarriers shall be false.
    for (unsigned k = 0; k != MAX_NOF_SUBCARRIERS; ++k) {
      ASSERT_TRUE(!mask.test(k));
    }
  }
}

// Tests that two patterns with symbol mask in common are merged into one.
TEST(re_pattern_test, merge_odd)
{
  unsigned rb_begin  = 1;
  unsigned rb_end    = 50;
  unsigned rb_stride = 2;

  re_pattern_list list;

  // Reset list.
  list.clear();

  // Create first pattern containing:
  // - even subcarrier indexes, and
  // - even symbol indexes.
  re_pattern pattern_1 = {};
  pattern_1.crb_mask   = crb_bitmap(rb_end);
  for (unsigned i_prb = rb_begin; i_prb < rb_end; i_prb += rb_stride) {
    pattern_1.crb_mask.set(i_prb);
  }
  for (unsigned k = 0; k != NOF_SUBCARRIERS_PER_RB; ++k) {
    pattern_1.re_mask.set(k, k % 2 == 0);
  }
  for (unsigned l = 0; l != MAX_NSYMB_PER_SLOT; ++l) {
    pattern_1.symbols.set(l, l % 2 == 0);
  }
  list.merge(pattern_1);
  ASSERT_TRUE(list.get_nof_entries() == 1);

  // Create second pattern:
  // - odd subcarrier indexes, and
  // - even symbol indexes.
  re_pattern pattern_2 = pattern_1;
  for (unsigned k = 0; k != NOF_SUBCARRIERS_PER_RB; ++k) {
    pattern_2.re_mask.set(k, k % 2 == 1);
  }
  list.merge(pattern_2);
  ASSERT_TRUE(list.get_nof_entries() == 1);

  // The pattern should be repeated for each symbol.
  for (unsigned l = 0; l != MAX_NSYMB_PER_SLOT; ++l) {
    // Create mask with all entries to false
    bounded_bitset<MAX_NOF_SUBCARRIERS> mask(MAX_NOF_SUBCARRIERS);

    // Set include mask.
    list.get_inclusion_mask(mask, l);

    // For each subcarrier it checks the mask.
    for (unsigned k = 0; k != MAX_NOF_SUBCARRIERS; ++k) {
      bool gold = false;

      // if it is in the RB range, then check the subcarrier index.
      if (k >= rb_begin * NOF_SUBCARRIERS_PER_RB && k < rb_end * NOF_SUBCARRIERS_PER_RB &&
          (k / NOF_SUBCARRIERS_PER_RB - rb_begin) % rb_stride == 0) {
        // Even symbol
        gold = (l % 2 == 0);
      }
      ASSERT_EQ(mask.test(k), gold);
    }

    // Set exclude.
    list.get_exclusion_mask(mask, l);

    // All the subcarriers shall be false.
    for (unsigned k = 0; k != MAX_NOF_SUBCARRIERS; ++k) {
      ASSERT_TRUE(!mask.test(k));
    }
  }
}

// Tests a merge between two patterns are the same.
TEST(re_pattern_test, merge_same)
{
  // Create a pattern.
  re_pattern pattern;
  pattern.crb_mask = crb_bitmap(MAX_NOF_PRBS);
  pattern.crb_mask.set(0);
  pattern.re_mask.set(0);
  pattern.symbols.set(0);

  // Add the pattern to a list twice.
  re_pattern_list list;
  list.merge(pattern);
  list.merge(pattern);

  // The number of entries shall be 1.
  ASSERT_EQ(list.get_nof_entries(), 1);
}

// Tests a merge between two patterns with different RB mapping.
TEST(re_pattern_test, merge_diff_rb)
{
  // Create pattern 1.
  re_pattern pattern1;
  pattern1.crb_mask = crb_bitmap(MAX_NOF_PRBS);
  pattern1.crb_mask.set(0);
  pattern1.re_mask.set(0);
  pattern1.symbols.set(0);

  // Create a pattern 2.
  re_pattern pattern2;
  pattern1.crb_mask = crb_bitmap(MAX_NOF_PRBS);
  pattern1.crb_mask.set(1);
  pattern2.re_mask.set(0);
  pattern2.symbols = {};
  pattern2.symbols.set(0);

  // Add the pattern to the list.
  re_pattern_list list;
  list.merge(pattern1);
  list.merge(pattern2);

  // The number of entries shall be 2.
  ASSERT_EQ(list.get_nof_entries(), 1);
}

TEST(re_pattern_test, inclusion_count)
{
  // Create a pattern.
  re_pattern pattern;
  pattern.crb_mask = crb_bitmap(MAX_NOF_PRBS);
  pattern.crb_mask.set(0);
  pattern.re_mask.set(0);
  pattern.symbols.set(0);

  // Add the pattern to a list.
  re_pattern_list list;
  list.merge(pattern);

  // Validate the inclusion count with an RB mask that matches.
  crb_bitmap rb_mask_match(2);
  rb_mask_match.set(0, true);
  ASSERT_EQ(list.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, rb_mask_match), 1);

  // Validate the inclusion count with an RB mask that does not match.
  crb_bitmap rb_mask_unmatch(2);
  rb_mask_unmatch.set(1, true);
  ASSERT_EQ(list.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, rb_mask_unmatch), 0);
}

// Tests that the inclusion count only considers the symbols within the requested symbol range.
TEST(re_pattern_test, inclusion_count_symbol_range)
{
  // Create a pattern that reserves all the subcarriers of one PRB in symbols 2 and 3.
  re_pattern pattern;
  pattern.crb_mask = crb_bitmap(MAX_NOF_PRBS);
  pattern.crb_mask.set(0);
  pattern.re_mask.fill(0, NOF_SUBCARRIERS_PER_RB);
  pattern.symbols.set(2);
  pattern.symbols.set(3);

  // A single-pattern list takes the direct calculation path.
  re_pattern_list list;
  list.merge(pattern);
  ASSERT_EQ(list.get_nof_entries(), 1);

  crb_bitmap rb_mask(1);
  rb_mask.set(0);

  // Symbol range excludes the pattern symbols.
  ASSERT_EQ(list.get_inclusion_count(0, 2, rb_mask), 0);
  ASSERT_EQ(list.get_inclusion_count(4, 10, rb_mask), 0);

  // Symbol range partially overlaps the pattern symbols.
  ASSERT_EQ(list.get_inclusion_count(3, 2, rb_mask), NOF_SUBCARRIERS_PER_RB);

  // Symbol range includes all the pattern symbols.
  ASSERT_EQ(list.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, rb_mask), 2 * NOF_SUBCARRIERS_PER_RB);

  // Add a second pattern outside the RB mask. A different RE mask prevents the merge, so the list takes the
  // multiple-pattern path. The counts match the single-pattern path.
  re_pattern pattern_other = pattern;
  pattern_other.crb_mask.reset();
  pattern_other.crb_mask.set(1);
  pattern_other.re_mask.reset();
  pattern_other.re_mask.set(0);
  list.merge(pattern_other);
  ASSERT_EQ(list.get_nof_entries(), 2);
  ASSERT_EQ(list.get_inclusion_count(0, 2, rb_mask), 0);
  ASSERT_EQ(list.get_inclusion_count(4, 10, rb_mask), 0);
  ASSERT_EQ(list.get_inclusion_count(3, 2, rb_mask), NOF_SUBCARRIERS_PER_RB);
  ASSERT_EQ(list.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, rb_mask), 2 * NOF_SUBCARRIERS_PER_RB);
}

// Test that a pattern is equal to itself and different to others.
TEST(re_pattern_test, equal)
{
  // Create a pattern.
  re_pattern pattern;
  pattern.crb_mask = crb_bitmap(MAX_NOF_PRBS);
  pattern.crb_mask.set(0);
  pattern.re_mask.set(0);
  pattern.symbols.set(0);

  // Add the pattern to a first list.
  re_pattern_list list1;
  list1.merge(pattern);

  // Modify pattern.
  pattern.symbols.set(1);

  // Add the pattern to a list.
  re_pattern_list list2;
  list2.merge(pattern);

  // Validate the equal operator.
  ASSERT_TRUE(list1 == list1);
  ASSERT_TRUE(list2 == list2);

  // Validate the not equal operator.
  ASSERT_TRUE(list1 != list2);
}

// Test a pattern creation from an initializer list.
TEST(re_pattern_test, bracket_initializer)
{
  // Create pattern list from full initializer list.
  re_pattern_list list1 = {
      {0, 52, 1, {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}},
      {0, 52, 1, {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}}};

  // Create same pattern using parameters.
  re_pattern pattern;
  pattern.crb_mask = crb_bitmap(MAX_NOF_PRBS);
  pattern.crb_mask.fill(0, 52);
  pattern.re_mask = re_prb_mask();
  pattern.re_mask.set(0);
  pattern.re_mask.set(4);
  pattern.re_mask.set(8);
  pattern.symbols.set(6);
  pattern.symbols.set(10);

  // Create list using initializer list of pattern.
  re_pattern_list list2 = {pattern};

  // Create list using default constructor and merge pattern.
  re_pattern_list list3;
  list3.merge(pattern);

  // Validate that all lists are equal.
  ASSERT_TRUE(list1 == list2);
  ASSERT_TRUE(list1 == list3);
}

// Tests that an empty list has no included resource elements.
TEST(re_pattern_test, inclusion_count_empty)
{
  re_pattern_list list;

  crb_bitmap rb_mask(2);
  rb_mask.fill(0, 2);
  ASSERT_EQ(list.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, rb_mask), 0);
}

// Tests that merging a pattern with a larger CRB mask into a pattern with a smaller one combines both CRB masks.
TEST(re_pattern_test, merge_larger_crb_mask)
{
  // Create a pattern with a CRB mask of 10 CRBs.
  re_pattern pattern_small;
  pattern_small.crb_mask = crb_bitmap(10);
  pattern_small.crb_mask.set(0);
  pattern_small.re_mask.set(0);
  pattern_small.symbols.set(0);

  // Create a pattern with the same subcarriers and symbols and a CRB mask of 20 CRBs.
  re_pattern pattern_large = pattern_small;
  pattern_large.crb_mask   = crb_bitmap(20);
  pattern_large.crb_mask.set(15);

  // Merge the larger pattern into the smaller one.
  re_pattern_list list;
  list.merge(pattern_small);
  list.merge(pattern_large);
  ASSERT_EQ(list.get_nof_entries(), 1);

  // The merged pattern contains CRB 0 and CRB 15.
  crb_bitmap rb_mask(20);
  rb_mask.fill(0, 20);
  ASSERT_EQ(list.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, rb_mask), 2);
}

// Tests the exclusion mask of a pattern with contiguous CRBs and all subcarriers within each CRB.
TEST(re_pattern_test, exclusion_mask_contiguous)
{
  // Create a pattern that reserves all the subcarriers of CRB 2 to 4 in symbol 1.
  re_pattern pattern;
  pattern.crb_mask = crb_bitmap(MAX_NOF_PRBS);
  pattern.crb_mask.fill(2, 5);
  pattern.re_mask.fill(0, NOF_SUBCARRIERS_PER_RB);
  pattern.symbols.set(1);

  re_pattern_list list;
  list.merge(pattern);

  // Mask of 10 CRBs: the subcarriers of CRB 2 to 4 are excluded in symbol 1.
  bounded_bitset<MAX_NOF_SUBCARRIERS> expected(10 * NOF_SUBCARRIERS_PER_RB);
  expected.fill(0, expected.size());
  expected.fill(2 * NOF_SUBCARRIERS_PER_RB, 5 * NOF_SUBCARRIERS_PER_RB, false);

  bounded_bitset<MAX_NOF_SUBCARRIERS> mask(10 * NOF_SUBCARRIERS_PER_RB);
  mask.fill(0, mask.size());
  list.get_exclusion_mask(mask, 1);
  ASSERT_EQ(mask, expected);

  // Mask of 4 CRBs: CRB 4 is outside the mask, so only CRBs 2 and 3 are excluded.
  expected.resize(4 * NOF_SUBCARRIERS_PER_RB);
  mask.resize(4 * NOF_SUBCARRIERS_PER_RB);
  mask.fill(0, mask.size());
  list.get_exclusion_mask(mask, 1);
  ASSERT_EQ(mask, expected);

  // Mask of 2 CRBs: the pattern lies outside the mask and excludes nothing.
  mask.resize(2 * NOF_SUBCARRIERS_PER_RB);
  mask.fill(0, mask.size());
  list.get_exclusion_mask(mask, 1);
  ASSERT_TRUE(mask.all());

  // The pattern does not use symbol 0 and excludes nothing.
  mask.resize(10 * NOF_SUBCARRIERS_PER_RB);
  mask.fill(0, mask.size());
  list.get_exclusion_mask(mask, 0);
  ASSERT_TRUE(mask.all());
}
