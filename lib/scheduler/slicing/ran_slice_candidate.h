// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ran_slice_instance.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {
namespace detail {

/// \brief RAN slice that is the next candidate for allocation in a given slot and cell.
template <bool IsDl>
class common_ran_slice_candidate
{
public:
  common_ran_slice_candidate(ran_slice_instance& instance_, slot_point slot_tx_, unsigned max_rbs_ = 0) :
    inst(&instance_), max_rbs(max_rbs_ == 0 ? inst->cfg.rbs.max() : max_rbs_), slot_tx(slot_tx_)
  {
  }

  ran_slice_id_t                               id() const { return inst->id; }
  [[nodiscard]] const slice_rrm_policy_config& cfg() const { return inst->cfg; }

  bool is_candidate(du_ue_index_t ue_idx) const { return inst->get_ues().contains(ue_idx); }
  bool is_candidate(du_ue_index_t ue_idx, lcid_t lcid) const { return inst->get_ues().contains(ue_idx, lcid); }

  /// Get UEs belonging to a slice.
  const slice_ue_repository& get_slice_ues() const { return inst->get_ues(); }

  /// Register that a new grant was allocated for a given UE.
  void store_grant(unsigned nof_rbs)
  {
    rb_count += nof_rbs;
    if constexpr (IsDl) {
      inst->store_pdsch_grant(nof_rbs, slot_tx);
    } else {
      inst->store_pusch_grant(nof_rbs, slot_tx);
    }
  }

  /// \brief Register RBs used by a PDSCH/PUSCH repetition occasion landing in a slot other than this candidate's own
  /// \c slot_tx. Unlike the single-argument \ref store_grant, this does not count against this candidate's own \ref
  /// remaining_rbs budget, which is scoped to \c slot_tx; it only updates the slice's per-slot RB usage for
  /// admission control purposes.
  void store_grant(unsigned nof_rbs, slot_point occasion_slot)
  {
    if constexpr (IsDl) {
      inst->store_pdsch_grant(nof_rbs, occasion_slot);
    } else {
      inst->store_pusch_grant(nof_rbs, occasion_slot);
    }
  }

  /// Remaining RBs available for allocation for the given slice.
  [[nodiscard]] unsigned remaining_rbs() const
  {
    ocudu_sanity_check(max_rbs >= rb_count, "RBs assigned to a slice cannot be more than its max");
    return max_rbs - rb_count;
  }

  /// Returns slot at which PUSCH/PDSCH needs to be scheduled for this slice candidate.
  slot_point get_slot_tx() const { return slot_tx; }

protected:
  ran_slice_instance* inst    = nullptr;
  unsigned            max_rbs = 0;
  /// Counts how many of this slice candidate's RBs have been used for UE allocation.
  unsigned rb_count = 0;
  /// Slot at which PUSCH/PDSCH needs to be scheduled for this slice candidate.
  slot_point slot_tx;
};

} // namespace detail

/// \brief Handle to fetch and update the state of a RAN slice in a given DL slot.
///
/// On destruction, the slice is marked as completed for the current slot and won't be considered as a candidate again.
using dl_ran_slice_candidate = detail::common_ran_slice_candidate<true>;

/// Interface to fetch and update the state of a RAN slice in a given UL slot.
///
/// On destruction, the slice is marked as completed for the current slot and won't be considered as a candidate again.
using ul_ran_slice_candidate = detail::common_ran_slice_candidate<false>;

} // namespace ocudu
