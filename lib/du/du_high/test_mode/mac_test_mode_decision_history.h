// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/spmc_slot_ring.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/mac/cell_configuration.h"
#include "ocudu/ran/slot_pdu_capacity_constants.h"
#include "ocudu/scheduler/resource_grid_util.h"
#include "ocudu/scheduler/result/pucch_info.h"
#include "ocudu/scheduler/result/pusch_info.h"

namespace ocudu {

/// \brief Stores the decision history of the MAC cell when in test mode.
///
/// The scheduler-result handler is the single writer; the CRC/UCI auto-generation paths read the history concurrently.
class mac_test_mode_cell_decision_history
{
public:
  struct slot_decision_history {
    static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>    pucchs;
    static_vector<ul_sched_info, MAX_PUSCH_PDUS_PER_SLOT> puschs;
  };

  using ring   = spmc_slot_ring<slot_decision_history>;
  using reader = ring::reader;
  using writer = ring::writer;

  mac_test_mode_cell_decision_history(const mac_cell_creation_request& cell_cfg) :
    sched_decision_history(get_ring_size(cell_cfg))
  {
  }

  reader read(slot_point sl) { return sched_decision_history.read(sl.count()); }

  writer write(slot_point sl)
  {
    auto w = sched_decision_history.write(sl.count());
    if (w != nullptr) {
      w->pucchs.clear();
      w->puschs.clear();
    }
    return w;
  }

  void clear(slot_point sl) { sched_decision_history.clear(sl.count()); }

private:
  static size_t get_ring_size(const mac_cell_creation_request& cell_cfg)
  {
    const unsigned ntn_cs_koffset =
        cell_cfg.sched_req.ran.ntn_params.has_value() &&
                cell_cfg.sched_req.ran.ntn_params->ntn_cfg.cell_specific_koffset.has_value()
            ? cell_cfg.sched_req.ran.ntn_params->ntn_cfg.cell_specific_koffset->count() *
                  get_nof_slots_per_subframe(cell_cfg.sched_req.ran.dl_cfg_common.init_dl_bwp.generic_params.scs)
            : 0;

    // Estimation of the time it takes the UL lower-layers to process and forward CRC/UCI indications.
    static constexpr unsigned MAX_UL_PHY_DELAY = 80;
    // Note: The history ring size has to be a multiple of the TDD frame size in slots.
    // Number of slots managed by this container.
    return get_allocator_ring_size_gt_min(get_max_slot_ul_alloc_delay(ntn_cs_koffset) + MAX_UL_PHY_DELAY);
  }

  /// Ring buffer of slot decision history.
  ring sched_decision_history;
};

} // namespace ocudu
