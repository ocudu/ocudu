// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/strong_type.h"
#include "ocudu/support/ocudu_assert.h"
#include <type_traits>

namespace ocudu {

namespace detail {

/// Tag struct to uniquely identify bounded integers by template types and values.
template <typename Integer>
struct bounded_integer_tag {};

template <typename Integer>
using bounded_integer_base = strong_type<Integer,
                                         detail::bounded_integer_tag<Integer>,
                                         strong_equality,
                                         strong_equality_with<Integer>,
                                         strong_comparison,
                                         strong_comparison_with<Integer>>;

} // namespace detail

/// Tag used to initialize a bounded_integer in invalid state.
struct bounded_integer_invalid_tag {};

/// This class represents an integer whose value is within the set of possible values: {MIN_VALUE, ..., MAX_VALUE}.
template <typename Integer, Integer MIN_VALUE, Integer MAX_VALUE>
class bounded_integer : public detail::bounded_integer_base<Integer>
{
  static_assert(std::is_integral_v<Integer>, "Template argument must be an integer");
  static_assert(MIN_VALUE <= MAX_VALUE, "Provided bounds for bounded_integer are not valid");

  using base_class = detail::bounded_integer_base<Integer>;

public:
  using base_class::base_class;
  using difference_type = std::conditional_t<sizeof(Integer) == sizeof(int64_t), int64_t, int32_t>;

  constexpr bounded_integer() noexcept = default;
  constexpr explicit bounded_integer(bounded_integer_invalid_tag /**/) noexcept : base_class(MAX_VALUE + 1) {}
  constexpr bounded_integer(Integer v) noexcept : base_class(v) { assert_bounds(v); }
  constexpr bounded_integer(const bounded_integer& other) noexcept            = default;
  constexpr bounded_integer(bounded_integer&& other) noexcept                 = default;
  constexpr bounded_integer& operator=(const bounded_integer& other) noexcept = default;
  constexpr bounded_integer& operator=(bounded_integer&& other) noexcept      = default;

  bounded_integer& operator=(Integer v)
  {
    assert_bounds(v);
    base_class::value() = v;
    return *this;
  }

  /// Get minimum possible value.
  static constexpr Integer min() { return MIN_VALUE; }

  /// Get maximum possible value.
  static constexpr Integer max() { return MAX_VALUE; }

  /// Checks whether the value is within the defined boundaries.
  constexpr bool valid() const { return base_class::value() >= MIN_VALUE && base_class::value() <= MAX_VALUE; }

  /// Cast operator to primitive integer type.
  explicit constexpr operator Integer() const { return base_class::value(); }

  /// Retrieves stored raw integer value.
  constexpr Integer value() const { return base_class::value(); }

  bounded_integer& operator++()
  {
    ++base_class::value();
    assert_bounds(base_class::value());
    return *this;
  }

  bounded_integer operator++(int)
  {
    auto tmp = base_class::value()++;
    assert_bounds(base_class::value());
    return tmp;
  }

  bounded_integer& operator--()
  {
    --base_class::value();
    assert_bounds(base_class::value());
    return *this;
  }

  bounded_integer operator--(int)
  {
    auto tmp = base_class::value()--;
    assert_bounds(base_class::value());
    return tmp;
  }
  bounded_integer& operator+=(Integer other)
  {
    base_class::value() += other;
    assert_bounds(base_class::value());
    return *this;
  }
  bounded_integer& operator+=(bounded_integer other) { return *this += other.value(); }

  bounded_integer operator+(Integer other) const
  {
    bounded_integer<Integer, MIN_VALUE, MAX_VALUE> ret{*this};
    return ret += other;
  }
  bounded_integer operator+(bounded_integer other) const { return *this + other.value(); }

  bounded_integer& operator-=(Integer other)
  {
    base_class::value() -= other;
    assert_bounds(base_class::value());
    return *this;
  }
  bounded_integer& operator-=(bounded_integer other) { return *this -= other.value(); }

  bounded_integer operator-(Integer other) const
  {
    bounded_integer<Integer, MIN_VALUE, MAX_VALUE> ret{*this};
    return ret -= other;
  }
  difference_type operator-(bounded_integer other) const
  {
    return static_cast<difference_type>(this->value()) - static_cast<difference_type>(other.value());
  }

protected:
  constexpr void assert_bounds(Integer v) const
  {
    ocudu_assert(v >= MIN_VALUE && v <= MAX_VALUE, "Value={} out-of-bounds [{}, {}]", v, MIN_VALUE, MAX_VALUE);
  }
};

} // namespace ocudu
