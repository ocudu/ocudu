// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "roundtrip_test.h"
#include "ocudu/adt/bounded_integer.h"
#include "ocudu/adt/interval.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/support/units.h"

namespace ocudu::schedtrace::roundtrip_test {

/// Specialization of \c test_values for any integral type T.
template <typename T>
struct test_values<T, std::enable_if_t<std::is_integral_v<T>>> {
  static std::vector<T> get() { return {std::numeric_limits<T>::min(), std::numeric_limits<T>::max()}; }
};

/// Specialization of \c test_values for any bounded_integer<Integer, MIN, MAX>.
template <typename Integer, Integer MIN, Integer MAX>
struct test_values<bounded_integer<Integer, MIN, MAX>> {
  static std::vector<bounded_integer<Integer, MIN, MAX>> get()
  {
    return {bounded_integer<Integer, MIN, MAX>{MIN}, bounded_integer<Integer, MIN, MAX>{MAX}};
  }
};

/// Specialization of \c test_values for any std::optional<T> where T has its own test_values.
template <typename T>
struct test_values<std::optional<T>, std::enable_if_t<has_test_values<T>::value>> {
  static std::vector<std::optional<T>> get()
  {
    std::vector<std::optional<T>> result{std::nullopt};
    for (const T& value : member_sweep_values<T>()) {
      result.emplace_back(value);
    }
    return result;
  }
};

/// Specialization of \c test_values for any static_vector<T, N> where T has its own test_values.
///
/// Produces an empty list, plus as many lists as needed to cover every sweep value of T (filling each list up to the
/// capacity N) -- so every element value appears in some instance even when N is smaller than the number of values.
template <typename T, size_t N>
struct test_values<static_vector<T, N>, std::enable_if_t<has_test_values<T>::value>> {
  static std::vector<static_vector<T, N>> get()
  {
    std::vector<static_vector<T, N>> result{static_vector<T, N>{}};
    static_vector<T, N>              current;
    for (const T& value : member_sweep_values<T>()) {
      current.push_back(value);
      if (current.size() == N) {
        result.push_back(current);
        current.clear();
      }
    }
    if (!current.empty()) {
      result.push_back(current);
    }
    return result;
  }
};

/// Specialization of \c test_values for any interval<T, RightClosed, Tag>.
template <typename T, bool RightClosed, typename Tag>
struct test_values<interval<T, RightClosed, Tag>, std::enable_if_t<has_test_values<T>::value>> {
  static std::vector<interval<T, RightClosed, Tag>> get()
  {
    constexpr T max = std::numeric_limits<T>::max();
    return {interval<T, RightClosed, Tag>{0, 0},
            interval<T, RightClosed, Tag>{0, max},
            interval<T, RightClosed, Tag>{max, max}};
  }
};

template <>
struct test_values<rnti_t> {
  static std::vector<rnti_t> get() { return {rnti_t::INVALID_RNTI, rnti_t::SI_RNTI}; }
};

template <>
struct test_values<du_ue_index_t> {
  static std::vector<du_ue_index_t> get()
  {
    return {du_ue_index_t::MIN_DU_UE_INDEX, du_ue_index_t::INVALID_DU_UE_INDEX};
  }
};

template <>
struct test_values<du_cell_index_t> {
  static std::vector<du_cell_index_t> get()
  {
    return {du_cell_index_t::MIN_DU_CELL_INDEX, du_cell_index_t::INVALID_DU_CELL_INDEX};
  }
};

template <>
struct test_values<slot_point> {
  static std::vector<slot_point> get()
  {
    constexpr unsigned max_count = slot_point(subcarrier_spacing::kHz240, 0).nof_slots_per_hyper_system_frame() - 1;
    return {slot_point(subcarrier_spacing::kHz15, 0), slot_point(subcarrier_spacing::kHz240, max_count)};
  }
};

template <>
struct test_values<units::bytes> {
  static std::vector<units::bytes> get()
  {
    return {units::bytes(0), units::bytes(std::numeric_limits<unsigned>::max())};
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
