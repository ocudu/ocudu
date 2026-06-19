// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/ue_repository.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/slot_pdu_capacity_constants.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/scheduler/config/logical_channel_group.h"
#include "ocudu/scheduler/result/sched_result.h"
#include "ocudu/scheduler/sched_consts.h"
#include "ocudu/support/units.h"

namespace ocudu {

struct cell_resource_allocator;

/// \brief Sub-scheduler that detects DL grants for bearers with triggered UL grant config and proactively injects
/// a UL BSR for those UEs, so the regular intra-slice scheduler can service the grant without waiting for an SR.
///
/// Integration:
///   - Call process_dl_results() after intra_slice_scheduler::post_process_results() to record which UEs need a grant.
///   - Call run_slot() before run_sched_strategy() to inject BSR bytes into those UEs.
class triggered_ul_grant_scheduler
{
public:
  triggered_ul_grant_scheduler(ue_repository& ues_, du_cell_index_t cell_index_, subcarrier_spacing scs_);

  /// Scans finalized DL grants and enqueues triggered UL grants for UEs whose DL grant included a LCID configured
  /// with triggered_ul_grant. \p pdsch_slot must be valid.
  void process_dl_results(slot_point sl_tx, const sched_result& sched_result);

  /// Fires triggered UL grants due by \p pdcch_slot by injecting a synthetic BSR into the affected UEs.
  void run_slot(slot_point pdcch_slot);

private:
  struct pending_grant {
    pending_grant(slot_point target_pdcch_slot_, du_ue_index_t ue_index_, lcg_id_t lcg_id_, units::bytes bytes_) :
      target_pdcch_slot(target_pdcch_slot_), ue_index(ue_index_), lcg_id(lcg_id_), bytes(bytes_)
    {
    }

    slot_point    target_pdcch_slot;
    du_ue_index_t ue_index;
    lcg_id_t      lcg_id;
    units::bytes  bytes;
  };

  ocudulog::basic_logger&    logger;
  ue_repository&             ues;
  const du_cell_index_t      cell_index;
  const subcarrier_spacing   scs;
  std::vector<pending_grant> pending_grants;
  // Reserve enough entries for all the PDUs schedulable across the maximum triggered-grant delay window.
  const unsigned pending_grants_size =
      MAX_UE_PDUS_PER_SLOT * get_nof_slots_per_subframe(scs) * SCHEDULER_MAX_TRIG_UL_DELAY.count();

  void clean_pending_grants(slot_point pdcch_slot);
};

} // namespace ocudu
