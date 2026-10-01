// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "srs_allocator.h"
#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/circular_vector.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/srs/srs_configuration.h"
#include "ocudu/ran/srs/srs_constants.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"
#include "ocudu/scheduler/resource_grid_util.h"
#include "ocudu/scheduler/sched_consts.h"

namespace ocudu {

class cell_configuration;
struct cell_resource_allocator;
class ue_cell_configuration;
struct positioning_measurement_request;

class srs_allocator_impl : public srs_allocator
{
public:
  explicit srs_allocator_impl(const cell_configuration& cell_cfg_, std::optional<srs_periodicity> prohibit_window);

  void slot_indication(slot_point slot_tx) override;

  aperiodic_srs_alloc_info allocate_aperiodic_srs(cell_resource_allocator&     res_alloc,
                                                  slot_point                   last_srs_slot,
                                                  const ue_cell_configuration& ue_cfg) override;

private:
  static constexpr size_t RES_MANAGER_RING_BUFFER_SIZE = get_allocator_ring_size_gt_min(
      SCHEDULER_MAX_K0 + srs_constants::MAX_SRS_SLOT_OFFSET + NTN_CELL_SPECIFIC_KOFFSET_MAX);

  // Checks if the SRS resource with cell ID \ref cell_res_id is currently used at slot \ref srs_slot. If not, it sets
  // the SRS resource as used (which means it can be allocated for the UE for which this function was called).
  // It returns "true" if the UE is allocated the SRS resource with cell ID \ref cell_res_id; "false" otherwise.
  bool alloc_srs_resource(slot_point srs_slot, unsigned cell_res_id);
  // Returns the index to the SRS slot bitmap \ref srs_slots corresponding to a given slot.
  unsigned get_slot_idx(slot_point sl) const;

  // Bitmap of the TDD slot indices indicating whether a given slot can be used for aperiodic SRS allocation.
  // The TDD slot index indicates the index of a slot within the TDD period; srs_slots.test(i) returns "true" if the
  // index i corresponds to a slot that can be used for aperiodic SRS allocation, "false" otherwise.
  bounded_bitset<MAX_NOF_SLOTS_IN_TDD_PERIOD> srs_slots;

  const cell_configuration& cell_cfg;
  // Defines a time interval (since the last aperiodic SRS allocation) within which it is not possible to allocate
  // another aperiodic SRS. If not set, the SRS allocator works as no-op.
  const std::optional<unsigned> srs_prohibit_time;

  // The bitset is indexed by SRS cell resource ID, whose range is the number of SRS resources generated for the cell.
  // Note: It cannot be bounded by the number of UEs of a cell, as the generated resources are tied to a slot offset and
  // a symbol interval, and dropping the excess ones would prevent the allocator from spreading the SRS.
  using slot_srs_allocation = bounded_bitset<MAX_NOF_DU_UES>;
  // Ring of SRS resource allocations indexed by slot.
  circular_vector<slot_srs_allocation> srs_allocation_ring;

  // Keeps track of the last slot_point used by the resource manager.
  slot_point last_sl_ind;
};

} // namespace ocudu
