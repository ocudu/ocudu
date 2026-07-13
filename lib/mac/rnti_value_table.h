// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/rnti.h"
#include "ocudu/support/ocudu_assert.h"
#include <atomic>
#include <limits>
#include <memory>
#include <vector>

namespace ocudu {

/// Table used by the MAC layer to convert from RNTI to a value in a thread-safe manner.
/// \remark We assume (i) each C-RNTI maps to one and only one UE and (ii) each CS-RNTI maps to a unique UE. Therefore,
/// a UE is associated with 1 RNTI and can be optionally associated with 1 CS-RNTI. We also assume that, for each
/// CS-RNTI that is added to the table, the corresponding UE must have been already added to the table with its C-RNTI.
template <typename T, T SentinelValue>
class rnti_value_table
{
  static constexpr rnti_t MIN_CRNTI  = rnti_t::MIN_CRNTI;
  static constexpr rnti_t MAX_CRNTI  = rnti_t::MAX_CRNTI;
  static constexpr size_t RNTI_RANGE = to_value(rnti_t::MAX_CRNTI) + 1 - to_value(rnti_t::MIN_CRNTI);
  using array_type                   = std::array<std::atomic<T>, RNTI_RANGE>;

public:
  rnti_value_table() : rnti_to_ue_index_map(std::make_unique<array_type>())
  {
    for (unsigned i = 0, e = rnti_to_ue_index_map->size(); i != e; ++i) {
      (*rnti_to_ue_index_map)[i].store(SentinelValue, std::memory_order_relaxed);
    }
  }

  rnti_value_table(const rnti_value_table&)            = delete;
  rnti_value_table(rnti_value_table&&)                 = delete;
  rnti_value_table& operator=(const rnti_value_table&) = delete;
  rnti_value_table& operator=(rnti_value_table&&)      = delete;

  /// \brief Associates the given RNTI to a value.
  ///
  /// \param crnti RNTI value.
  /// \param value Value to associate with an RNTI.
  /// \param is_cs_rnti Whether this request is for CS-RNTI.
  /// \return Returns true if the RNTI does not yet exist, otherwise false.
  bool add_ue(rnti_t crnti, T value, bool is_cs_rnti = false)
  {
    ocudu_assert(is_crnti(crnti), "Invalid c-rnti={}", crnti);
    ocudu_assert(value != SentinelValue, "Invalid rnti_value_table value={}", fmt::underlying(value));

    std::atomic<T>& ue_pos      = get(crnti);
    T               prev_ue_idx = ue_pos.exchange(value, std::memory_order_relaxed);
    if (prev_ue_idx == SentinelValue) {
      // With CS-RNTI, do not increase the counter; as the corresponding C-RNTI has been counted already.
      if (not is_cs_rnti) {
        nof_ues_.fetch_add(1, std::memory_order_relaxed);
      }
      return true;
    }

    return false;
  }

  /// Removes the given RNTI from the table.
  void rem_ue(rnti_t crnti, bool is_cs_rnti = false)
  {
    ocudu_assert(is_crnti(crnti), "Invalid c-rnti={}", crnti);

    std::atomic<T>& ue_pos      = get(crnti);
    T               prev_ue_idx = ue_pos.exchange(SentinelValue, std::memory_order_relaxed);
    // With CS-RNTI, the counter wasn't increased; thus, no need to decrease it now.
    if (is_cs_rnti) {
      return;
    }
    if (prev_ue_idx != SentinelValue) {
      nof_ues_.fetch_sub(1, std::memory_order_relaxed);
      ocudu_assert(nof_ues_.load(std::memory_order_relaxed) <= RNTI_RANGE, "Invalid rnti_table state");
    }
  }

  /// Get an estimate of the current number of UEs present in the table.
  /// \remark Each UE is associated with one and only C-RNTI; however, a UE can be also associated with an optional
  /// CS-RNTI.
  size_t nof_ues() const { return nof_ues_.load(std::memory_order_relaxed); }

  /// Checks whether the passed RNTI is registered as a UE.
  bool has_rnti(rnti_t crnti) const { return this->operator[](crnti) != SentinelValue; }

  T operator[](rnti_t crnti) const
  {
    ocudu_sanity_check(is_crnti(crnti), "Invalid c-rnti={}", crnti);

    const std::atomic<T>& ue_pos = get(crnti);
    return ue_pos.load(std::memory_order_relaxed);
  }

private:
  std::atomic<T>& get(rnti_t rnti) { return (*rnti_to_ue_index_map)[to_value(rnti) - to_value(rnti_t::MIN_CRNTI)]; }
  const std::atomic<T>& get(rnti_t rnti) const
  {
    return (*rnti_to_ue_index_map)[to_value(rnti) - to_value(rnti_t::MIN_CRNTI)];
  }

  /// Table of RNTI -> UE index with size 65535.
  std::unique_ptr<array_type> rnti_to_ue_index_map;
  /// Number of tracked UEs in the table.
  std::atomic<size_t> nof_ues_{0};
};

} // namespace ocudu
