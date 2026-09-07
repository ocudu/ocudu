// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file Self-tests for the roundtrip harness's bottom-up coverage rule: an inner type that is independently
/// roundtrip-tested (defines roundtrip()) is swept by enclosing types only at its two extremes, while an
/// embedded-only inner type keeps the full recursive sweep.

// common.h supplies the integral test_values the dummy types' int fields sweep through.
#include "common.h" // IWYU pragma: keep
#include "roundtrip_test.h"
#include "ocudu/adt/span.h"
#include "ocudu/adt/to_array.h"
#include <gtest/gtest.h>
#include <limits>

namespace ocudu::schedtrace::roundtrip_test {

namespace {

// An inner type with its own roundtrip(), standing in for a type with exposed converters and its own TEST.
struct tested_inner {
  int a = 0;
  int b = 0;

  bool operator==(const tested_inner& other) const { return a == other.a and b == other.b; }
};

// An inner type with reflected members only, coverable solely through its enclosing type.
struct embedded_inner {
  int a = 0;
  int b = 0;

  bool operator==(const embedded_inner& other) const { return a == other.a and b == other.b; }
};

struct outer_with_tested {
  tested_inner in;
  int          c = 0;
};

struct outer_with_embedded {
  embedded_inner in;
  int            c = 0;
};

// A single-field independently-tested type: its test_values are already just the two extremes, so the member
// sweep must return them unchanged.
struct tiny_tested {
  int a = 0;

  bool operator==(const tiny_tested& other) const { return a == other.a; }
};

// Mirrors pucch_info::res: a compare-only pointer member with no test_values, looked up on decode rather than
// serialized by value.
struct outer_with_pointer {
  const tested_inner* p = nullptr;
  int                 c = 0;
};

// Mirrors pusch_information::intra_slot_freq_hopping: a field whose type does have test_values, but whose valid
// values depend on another field's, so the harness must not pick them.
struct outer_with_compare_only {
  int a = 0;
  int c = 0;
};

// Mirrors rach_indication_message::preamble: no operator==, compared member-wise through its reflected members.
struct uncomparable_inner {
  int a = 0;
  int b = 0;
};

// A type whose operator== disagrees with its wire format: b is not serialized and is left out of the reflected
// members, but the native operator== -- which knows nothing about the wire format -- still compares it.
struct partly_serialized {
  int a = 0;
  int b = 0;

  bool operator==(const partly_serialized& other) const { return a == other.a and b == other.b; }
};

struct computed {
  int a = 0;

  bool operator==(const computed& other) const { return a == other.a; }
};

} // namespace

template <>
struct roundtrip_traits<tested_inner> {
  static constexpr auto members = std::make_tuple(field("a", &tested_inner::a), field("b", &tested_inner::b));

  static tested_inner roundtrip(const tested_inner& value) { return value; }
};

template <>
struct roundtrip_traits<embedded_inner> {
  static constexpr auto members = std::make_tuple(field("a", &embedded_inner::a), field("b", &embedded_inner::b));
};

template <>
struct roundtrip_traits<outer_with_tested> {
  static constexpr auto members =
      std::make_tuple(field("in", &outer_with_tested::in), field("c", &outer_with_tested::c));
};

template <>
struct roundtrip_traits<outer_with_embedded> {
  static constexpr auto members =
      std::make_tuple(field("in", &outer_with_embedded::in), field("c", &outer_with_embedded::c));
};

template <>
struct roundtrip_traits<tiny_tested> {
  static constexpr auto members = std::make_tuple(field("a", &tiny_tested::a));

  static tiny_tested roundtrip(const tiny_tested& value) { return value; }
};

template <>
struct roundtrip_traits<outer_with_pointer> {
  static constexpr auto members =
      std::make_tuple(field("p", &outer_with_pointer::p), field("c", &outer_with_pointer::c));
};

template <>
struct roundtrip_traits<outer_with_compare_only> {
  static constexpr auto members =
      std::make_tuple(compare_only_field("a", &outer_with_compare_only::a), field("c", &outer_with_compare_only::c));
};

template <>
struct roundtrip_traits<uncomparable_inner> {
  static constexpr auto members =
      std::make_tuple(field("a", &uncomparable_inner::a), field("b", &uncomparable_inner::b));
};

template <>
struct roundtrip_traits<partly_serialized> {
  static constexpr auto members = std::make_tuple(field("a", &partly_serialized::a));
};

template <>
struct roundtrip_traits<computed> {
  static constexpr auto members =
      std::make_tuple(computed_field<computed, int>("a", [](computed& c) -> int& { return c.a; }));
};

namespace {

constexpr int int_min = std::numeric_limits<int>::min();
constexpr int int_max = std::numeric_limits<int>::max();

constexpr auto tested_values =
    to_array({tested_inner{int_min, int_min}, tested_inner{int_max, int_min}, tested_inner{int_max, int_max}});

constexpr auto tested_swept = to_array({tested_inner{int_min, int_min}, tested_inner{int_max, int_max}});

constexpr auto embedded_values =
    to_array({embedded_inner{int_min, int_min}, embedded_inner{int_max, int_min}, embedded_inner{int_max, int_max}});

constexpr auto embedded_swept = embedded_values;

TEST(roundtrip_harness_test, tested_inner_is_swept_only_at_extremes)
{
  const std::vector<tested_inner> values = test_values<tested_inner>::get();
  EXPECT_EQ(span<const tested_inner>(values), span<const tested_inner>(tested_values));

  const std::vector<tested_inner> swept = member_sweep_values<tested_inner>();
  EXPECT_EQ(span<const tested_inner>(swept), span<const tested_inner>(tested_swept));
}

TEST(roundtrip_harness_test, embedded_inner_keeps_full_sweep)
{
  const std::vector<embedded_inner> values = test_values<embedded_inner>::get();
  EXPECT_EQ(span<const embedded_inner>(values), span<const embedded_inner>(embedded_values));

  const std::vector<embedded_inner> swept = member_sweep_values<embedded_inner>();
  EXPECT_EQ(span<const embedded_inner>(swept), span<const embedded_inner>(embedded_swept));
}

TEST(roundtrip_harness_test, enclosing_type_varies_tested_inner_once)
{
  // base, in at its all-last extreme, c at max: the inner's intermediate variation {max,min} must not appear.
  const std::vector<outer_with_tested> values = test_values<outer_with_tested>::get();
  ASSERT_EQ(values.size(), 3U);
  for (unsigned i = 0; i != tested_swept.size(); ++i) {
    EXPECT_EQ(values[i].in, tested_swept[i]);
  }
}

TEST(roundtrip_harness_test, enclosing_type_varies_embedded_inner_in_full)
{
  // base, in.a->max, in.b->max, c->max.
  const std::vector<outer_with_embedded> values = test_values<outer_with_embedded>::get();
  ASSERT_EQ(values.size(), 4U);
  for (unsigned i = 0; i != embedded_swept.size(); ++i) {
    EXPECT_EQ(values[i].in, embedded_swept[i]);
  }
}

TEST(roundtrip_harness_test, two_value_tested_type_is_swept_unchanged)
{
  constexpr auto tiny_values = to_array({tiny_tested{int_min}, tiny_tested{int_max}});

  const std::vector<tiny_tested> swept = member_sweep_values<tiny_tested>();
  EXPECT_EQ(span<const tiny_tested>(swept), span<const tiny_tested>(tiny_values));
}

TEST(roundtrip_harness_test, computed_field_sweeps_like_a_regular_field)
{
  const std::vector<computed> values = test_values<computed>::get();
  ASSERT_EQ(values.size(), 2U);
  EXPECT_EQ(values[0].a, int_min);
  EXPECT_EQ(values[1].a, int_max);

  EXPECT_TRUE(fields_eq(computed{int_max}, computed{int_max}));
  EXPECT_FALSE(fields_eq(computed{int_min}, computed{int_max}));
}

TEST(roundtrip_harness_test, fields_eq_verdicts)
{
  const tested_inner value{int_min, int_max};
  EXPECT_TRUE(fields_eq(value, tested_inner{int_min, int_max}));
  EXPECT_FALSE(fields_eq(value, tested_inner{int_min, int_min}));
  EXPECT_FALSE(fields_eq(value, tested_inner{int_max, int_max}));
}

TEST(roundtrip_harness_test, compare_only_member_is_not_varied_but_still_compared)
{
  // p has no test_values, so it contributes no sweep axis: only c is varied...
  const std::vector<outer_with_pointer> values = test_values<outer_with_pointer>::get();
  ASSERT_EQ(values.size(), 2U);
  for (const outer_with_pointer& value : values) {
    EXPECT_EQ(value.p, nullptr);
  }

  // ...but fields_eq still checks it on every instance.
  const tested_inner pointee{int_min, int_min};
  EXPECT_FALSE(fields_eq(outer_with_pointer{nullptr, int_min}, outer_with_pointer{&pointee, int_min}));
}

TEST(roundtrip_harness_test, compare_only_field_is_not_swept_but_still_compared)
{
  // a is marked compare-only, so it contributes no sweep axis even though test_values<int> exists: only c is varied,
  // and a keeps its default rather than test_values<int>'s first value...
  const std::vector<outer_with_compare_only> values = test_values<outer_with_compare_only>::get();
  ASSERT_EQ(values.size(), 2U);
  for (const outer_with_compare_only& value : values) {
    EXPECT_EQ(value.a, 0);
  }
  EXPECT_EQ(values[0].c, int_min);
  EXPECT_EQ(values[1].c, int_max);

  // ...but fields_eq still checks it on every instance.
  EXPECT_FALSE(fields_eq(outer_with_compare_only{0, int_min}, outer_with_compare_only{int_max, int_min}));
}

TEST(roundtrip_harness_test, pointer_members_compare_by_pointee)
{
  const tested_inner pointee{int_min, int_max};
  const tested_inner equal_pointee{int_min, int_max};
  const tested_inner different_pointee{int_max, int_max};

  const outer_with_pointer value{&pointee, 0};
  EXPECT_TRUE(fields_eq(value, outer_with_pointer{&equal_pointee, 0}));
  EXPECT_FALSE(fields_eq(value, outer_with_pointer{&different_pointee, 0}));
  EXPECT_FALSE(fields_eq(value, outer_with_pointer{nullptr, 0}));
  EXPECT_TRUE(fields_eq(outer_with_pointer{nullptr, 0}, outer_with_pointer{nullptr, 0}));
}

TEST(roundtrip_harness_test, optional_sweeps_nullopt_then_member_sweep_values)
{
  const std::vector<std::optional<tested_inner>> values = test_values<std::optional<tested_inner>>::get();
  ASSERT_EQ(values.size(), 1 + tested_swept.size());
  EXPECT_EQ(values[0], std::nullopt);
  for (unsigned i = 0; i != tested_swept.size(); ++i) {
    EXPECT_EQ(values[1 + i], std::optional(tested_swept[i]));
  }
}

TEST(roundtrip_harness_test, variant_alternatives_follow_the_same_rule)
{
  using var_t = std::variant<tested_inner, embedded_inner>;
  // tested_inner contributes its two extremes, embedded_inner its full 3-value sweep.
  const std::vector<var_t> values = test_values<var_t>::get();
  ASSERT_EQ(values.size(), 5U);
  for (unsigned i = 0; i != tested_swept.size(); ++i) {
    EXPECT_EQ(std::get<tested_inner>(values[i]), tested_swept[i]);
  }
  for (unsigned i = tested_swept.size(); i != values.size(); ++i) {
    EXPECT_EQ(std::get<embedded_inner>(values[i]), embedded_swept[i - tested_swept.size()]);
  }
}

TEST(roundtrip_harness_test, reflected_members_take_precedence_over_operator_equal)
{
  constexpr partly_serialized value{1, 2};
  constexpr partly_serialized differs_outside_wire_format{1, 3};
  constexpr partly_serialized differs_on_the_wire{2, 2};

  // b is outside the wire format, so it must not decide the verdict even though operator== compares it.
  EXPECT_TRUE(values_equal(value, differs_outside_wire_format));
  EXPECT_FALSE(value == differs_outside_wire_format);
  EXPECT_FALSE(values_equal(value, differs_on_the_wire));

  // Same through the containers, which must not fall back to the std:: operator== either.
  using vec_t = static_vector<partly_serialized, 2>;
  EXPECT_TRUE(values_equal(vec_t{value}, vec_t{differs_outside_wire_format}));
  using opt_t = std::optional<partly_serialized>;
  EXPECT_TRUE(values_equal(opt_t{value}, opt_t{differs_outside_wire_format}));
  using var_t = std::variant<std::monostate, partly_serialized>;
  EXPECT_TRUE(values_equal(var_t{value}, var_t{differs_outside_wire_format}));
}

TEST(roundtrip_harness_test, variant_and_optional_of_uncomparable_type_compare_member_wise)
{
  using var_t = std::variant<std::monostate, uncomparable_inner>;
  EXPECT_TRUE(values_equal(var_t{uncomparable_inner{1, 2}}, var_t{uncomparable_inner{1, 2}}));
  EXPECT_FALSE(values_equal(var_t{uncomparable_inner{1, 2}}, var_t{uncomparable_inner{1, 3}}));
  EXPECT_FALSE(values_equal(var_t{uncomparable_inner{1, 2}}, var_t{std::monostate{}}));
  EXPECT_TRUE(values_equal(var_t{std::monostate{}}, var_t{std::monostate{}}));

  using opt_t = std::optional<uncomparable_inner>;
  EXPECT_TRUE(values_equal(opt_t{uncomparable_inner{1, 2}}, opt_t{uncomparable_inner{1, 2}}));
  EXPECT_FALSE(values_equal(opt_t{uncomparable_inner{1, 2}}, opt_t{uncomparable_inner{2, 2}}));
  EXPECT_FALSE(values_equal(opt_t{uncomparable_inner{1, 2}}, opt_t{std::nullopt}));
  EXPECT_TRUE(values_equal(opt_t{std::nullopt}, opt_t{std::nullopt}));
}

} // namespace

} // namespace ocudu::schedtrace::roundtrip_test
