// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <type_traits>

namespace ocudu {

namespace detail {

struct default_interval_tag;

} // namespace detail

/// Representation of an interval between two numeric-types with the math representation [start, stop).
template <typename T, bool RightClosed = false, typename Tag = detail::default_interval_tag>
class interval
{
  static_assert(std::is_trivially_copyable_v<T>, "Expected to be trivially copyable");

public:
  /// Whether the interval is of real numbers.
  using is_real = std::is_floating_point<T>;
  /// Type used to represent the interval length.
  using length_type =
      std::conditional_t<is_real::value, T, std::conditional_t<(sizeof(T) > sizeof(uint32_t)), uint64_t, uint32_t>>;

  constexpr interval() : start_(T{}), stop_(T{}) {}

  template <typename U, typename V>
  constexpr interval(U start_point, V stop_point) : start_(start_point), stop_(stop_point)
  {
    static_assert(std::is_convertible_v<U, T>, "Invalid interval start point type");
    static_assert(std::is_convertible_v<V, T>, "Invalid interval stop point type");
    ocudu_assert(start_ <= stop_, "Invalid interval, start point must not exceed stop point");
  }

  template <typename U, typename V>
  static constexpr interval start_and_len(U start_point, V dur)
  {
    return interval(start_point, RightClosed ? start_point + dur - 1 : start_point + dur);
  }

  constexpr T start() const noexcept { return start_; }

  constexpr T stop() const noexcept { return stop_; }

  bool empty() const { return not RightClosed and stop_ == start_; }

  /// Interval length. e.g. [0, 1), [0, 1], [0 .. 1) have length 1. [0 .. 1] has length 2.
  constexpr length_type length() const noexcept
  {
    return static_cast<length_type>(stop_ - start_ + ((RightClosed and not is_real::value) ? 1 : 0));
  }

  void set(T start_point, T stop_point)
  {
    ocudu_assert(stop_point >= start_point, "interval::set called for invalid range points");
    start_ = start_point;
    stop_  = stop_point;
  }

  /// Increase the interval length, maintaining the same starting point.
  /// \param length_increment Length increment/decrement to apply to current interval size. In case
  /// length_increment < 0, the condition -length_increment <= length() must be met.
  void extend(T length_increment)
  {
    // Detect length overflows
    ocudu_assert(std::is_unsigned<T>::value or (length_increment >= 0 or length() >= -length_increment),
                 "Resulting interval would be invalid");
    stop_ += length_increment;
  }

  /// Set the interval length, maintaining the same starting point.
  void resize(length_type new_length) { stop_ = start_ + new_length; }

  /// Move interval by an offset.
  constexpr void displace_by(int offset)
  {
    ocudu_assert(
        std::is_signed_v<T> or static_cast<int64_t>(start_) >= -offset,
        "Cannot have negative starting_points in case interval<T> underlying type is unsigned. Start={} < offset={}",
        start_,
        -offset);
    start_ += offset;
    stop_ += offset;
  }

  /// Move interval start to provided starting point.
  void displace_to(T start_point)
  {
    stop_  = start_point + length();
    start_ = start_point;
  }

  /// If this and other intervals overlap.
  bool overlaps(interval other) const noexcept
  {
    // Note: if one or both intervals are empty, this will always return false
    return RightClosed ? start_ <= other.stop_ and other.start_ <= stop_
                       : start_ < other.stop_ and other.start_ < stop_;
  }

  /// If interval contains provided point.
  bool contains(T point) const noexcept
  {
    return start_ <= point and (point < stop_ or (RightClosed and point == stop_));
  }

  /// If interval contains provided interval.
  bool contains(interval other) const noexcept { return start_ <= other.start_ and other.stop_ <= stop_; }

  /// Assign to this interval the intersection between this and the provided interval.
  interval& intersect(const interval& other)
  {
    if (not overlaps(other)) {
      *this = interval{};
    } else {
      start_ = std::max(start(), other.start());
      stop_  = std::min(stop(), other.stop());
    }
    return *this;
  }

  /// Returns the closest integer to \c point that is within the bounds of the interval.
  T clamp(T point) const { return std::max(std::min(point, stop_), start_); }

  bool operator==(const interval& other) const noexcept { return start() == other.start() and stop() == other.stop(); }

  bool operator!=(const interval& other) const noexcept { return not(*this == other); }

  /// Compares interval starting points. If they are the same, the interval with shorter length is the lowest.
  bool operator<(const interval& other) const noexcept
  {
    return start() < other.start() or (start() == other.start() and stop() < other.stop());
  }

  /// Make union of intervals. If intervals do not overlap, the empty interval is returned.
  interval operator|(const interval& other) const noexcept
  {
    if (not overlaps(other)) {
      return interval{};
    }
    return {std::min(start(), other.start()), std::max(stop(), other.stop())};
  }

  /// Make intersection of intervals.
  interval operator&(const interval& other) const noexcept
  {
    if (not overlaps(other)) {
      return interval{};
    }
    return interval{std::max(start(), other.start()), std::min(stop(), other.stop())};
  }
  template <typename Interval>
  Interval convert_to() const
  {
    static_assert(right_closed() == Interval::right_closed(), "Conversion only supported for same RightClosed");
    // Just the tag changes.
    return {start(), stop()};
  }

  static constexpr bool right_closed() noexcept { return RightClosed; }

private:
  T start_;
  T stop_;
};

} // namespace ocudu
