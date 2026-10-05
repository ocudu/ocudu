// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/pdcch/coreset.h"
#include "ocudu/ran/pusch/pusch_constants.h"
#include "ocudu/ran/resource_allocation/rb_bitmap.h"
#include "ocudu/ran/slot_point.h"
#include <bitset>
#include <vector>

namespace ocudu {

class cell_configuration;
struct cell_resource_allocator;

/// \brief Tracks the CCE budget of the UE-dedicated CORESETs for the DL and UL UE grants of a PDCCH slot.
///
/// An equal share of CCEs is reserved for the PDSCH slot and for each of the PUSCH slots reachable from the PDCCH slot,
/// the latter only if there is UL demand. A direction can use any free CCE not reserved for the PUSCH slots yet
/// to be scheduled.
class pdcch_cce_budget_tracker
{
public:
  /// Distinct k2 values reachable from a PDCCH slot.
  using k2_list = static_vector<uint8_t, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>;

  explicit pdcch_cce_budget_tracker(const cell_resource_allocator& cell_alloc_);

  /// Reset context in preparation for new PDCCH slot, given whether any UE has pending UL data.
  void slot_indication(slot_point pdcch_slot, bool ul_pending);

  /// Remaining CCEs that DL UE grants can use in the current PDCCH slot.
  unsigned remaining_dl_cces() const;

  /// \brief Remaining CCEs that UL UE grants for the given PUSCH slot can use.
  /// \remark PUSCH slots must be scheduled in increasing order.
  unsigned remaining_ul_cces(slot_point pusch_slot) const;

private:
  /// Resources of a tracked CORESET.
  struct coreset_resources {
    /// CRBs spanned by the CORESET.
    crb_bitmap crbs;
    /// Duration of the CORESET in OFDM symbols.
    uint8_t duration;
  };

  /// k2 values reachable from the given PDCCH slot.
  const k2_list& k2s(slot_point sl) const;

  /// CCEs of a single share in the given PDCCH slot.
  unsigned share(slot_point sl) const;

  /// CCEs reserved for the reachable PUSCH slots after the given one, or for all of them if the slot is invalid.
  unsigned nof_reserved_ul_cces(slot_point pusch_slot) const;

  /// CCEs of the tracked CORESETs whose resources are still free in the current PDCCH slot.
  unsigned nof_free_cces() const;

  const cell_resource_allocator& cell_alloc;
  // k2 values reachable from each PDCCH slot index of the TDD period.
  const std::vector<k2_list> k2s_per_pdcch_slot;
  // Resources of the tracked CORESETs.
  static_vector<coreset_resources, MAX_NOF_CORESETS_PER_BWP> tracked_coreset_res;
  // Total number of CCEs of the tracked CORESETs.
  unsigned total_cces = 0;

  // Current PDCCH slot.
  slot_point pdcch_slot;
  // Whether any UE has pending UL data in the current PDCCH slot.
  bool ul_pending = false;
};

/// \brief Distinct k2 values of the common PUSCH TD resources applicable in each PDCCH slot index of the TDD period. In
/// FDD, a single entry with the smallest k2.
std::vector<pdcch_cce_budget_tracker::k2_list> compute_pusch_k2s_per_pdcch_slot(const cell_configuration& cell_cfg);

/// Number of PUSCH slots that can be scheduled from each PDCCH slot index of the TDD period.
std::vector<uint8_t> compute_nof_pusch_slots_per_pdcch_slot(const cell_configuration& cell_cfg);

} // namespace ocudu
