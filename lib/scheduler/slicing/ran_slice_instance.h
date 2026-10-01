// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../config/cell_configuration.h"
#include "../ue_context/ue_repository.h"
#include "ran_slice_id.h"
#include "slice_ue_repository.h"
#include "ocudu/scheduler/config/slice_rrm_policy_config.h"

namespace ocudu {

/// This class stores all the internal information relative to a RAN slice instantiation.
class ran_slice_instance
{
  // Const for the max difference in number of slots between the current slot and the last allocation slot.
  // This limit ensures lack of ambiguity in the slot point comparison
  static constexpr unsigned MAX_SLOTS_SINCE_LAST_PXSCH = 256;

public:
  ran_slice_instance(ran_slice_id_t                 id_,
                     const cell_configuration&      cell_cfg_,
                     const slice_rrm_policy_config& cfg_,
                     ue_repository&                 ues_);

  void slot_indication(slot_point slot_tx);

  bool active() const { return not slice_ues.empty(); }

  /// Save PDSCH grant.
  /// \param[in] crbs Number of RBs allocated.
  /// \param[in] pdsch_slot Slot where the PDSCH is scheduled.
  void store_pdsch_grant(unsigned crbs, slot_point pdsch_slot)
  {
    pdsch_rb_count_per_slot[pdsch_slot.count() % pdsch_rb_count_per_slot.size()] += crbs;
    last_pdsch_alloc_slot = pdsch_slot;
  }

  /// Save PUSCH grant.
  void store_pusch_grant(unsigned crbs, slot_point pusch_slot)
  {
    auto& pusch_count = pusch_rb_count_per_slot[pusch_slot.count() % pusch_rb_count_per_slot.size()];
    pusch_count += crbs;
    last_pusch_alloc_slot = pusch_slot;
  }

  /// Returns UEs belonging to this slice.
  const slice_ue_repository& get_ues() const { return slice_ues; }
  slice_ue_repository&       get_ues() { return slice_ues; }

  unsigned nof_pusch_rbs_allocated(slot_point pusch_slot) const
  {
    return pusch_rb_count_per_slot[pusch_slot.count() % pusch_rb_count_per_slot.size()];
  }

  /// \brief Number of PDSCH RBs already allocated to this slice in the given slot.
  /// \remark Unlike PUSCH, most PDSCH grants target the current slot (k0=0); however, PDSCH repetition occasions can
  /// land in future slots, so this is tracked per-slot rather than as a single running counter.
  unsigned nof_pdsch_rbs_allocated(slot_point pdsch_slot) const
  {
    return pdsch_rb_count_per_slot[pdsch_slot.count() % pdsch_rb_count_per_slot.size()];
  }

  /// Number of slots elapsed between the provided PDSCH slot and the last time this slice was scheduled.
  unsigned nof_slots_since_last_pdsch(slot_point pdsch_slot) const
  {
    return last_pdsch_alloc_slot.valid()
               ? (pdsch_slot >= last_pdsch_alloc_slot ? pdsch_slot - last_pdsch_alloc_slot : 0)
               : MAX_SLOTS_SINCE_LAST_PXSCH;
  }
  unsigned nof_slots_since_last_pusch(slot_point pusch_slot) const
  {
    return last_pusch_alloc_slot.valid()
               ? (pusch_slot >= last_pusch_alloc_slot ? pusch_slot - last_pusch_alloc_slot : 0)
               : MAX_SLOTS_SINCE_LAST_PXSCH;
  }

  float average_pdsch_rbs_per_slot() const { return avg_pdsch_rbs_per_slot; }
  float average_pusch_rbs_per_slot() const { return avg_pusch_rbs_per_slot; }

  ran_slice_id_t            id;
  const cell_configuration* cell_cfg;
  slice_rrm_policy_config   cfg;
  const unsigned            min_k2;

  /// Ring of counters of how many RBs have been scheduled for PDSCH in a particular slot for this slice.
  std::vector<unsigned> pdsch_rb_count_per_slot;
  /// Ring of counters of how many RBs have been scheduled for PUSCH in a particular slot for this slice.
  std::vector<unsigned> pusch_rb_count_per_slot;

  /// Nof. of previous slots in which RB count needs to be cleared in \c pusch_rb_count_per_slot upon receiving slot
  /// indication.
  /// \remark This is needed since slot_indication() is called only over DL slots.
  unsigned nof_slots_to_clear = cell_cfg->is_tdd() ? nof_slots_per_tdd_period(*cell_cfg->params.tdd_cfg) : 1;

private:
  // Last slot that the PDSCH was allocated.
  slot_point last_pdsch_alloc_slot;

  // Last slot that the PUSCH was allocated.
  slot_point last_pusch_alloc_slot;

  // Track average number of RBs scheduler per slice.
  float avg_pdsch_rbs_per_slot = 0;
  float avg_pusch_rbs_per_slot = 0;

  slice_ue_repository slice_ues;
};

} // namespace ocudu
