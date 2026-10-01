// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "scheduler_time_rr.h"
#include "../slicing/slice_ue_repository.h"

using namespace ocudu;

void scheduler_time_rr::compute_ue_dl_priorities(slot_point               pdcch_slot,
                                                 slot_point               pdsch_slot,
                                                 span<ue_newtx_candidate> ue_candidates)
{
  // We perform round-robin by assigning priorities based on the difference between the current slot and the last slot
  // the UE has been allocated.
  for (ue_newtx_candidate& candidate : ue_candidates) {
    candidate.priority = dl_alloc_count - ue_last_dl_alloc_count[candidate.ue->ue_index()];
  }
}

void scheduler_time_rr::compute_ue_ul_priorities(slot_point               pdcch_slot,
                                                 slot_point               pusch_slot,
                                                 span<ue_newtx_candidate> ue_candidates)
{
  // \ref compute_ue_dl_priorities
  for (ue_newtx_candidate& candidate : ue_candidates) {
    candidate.priority = ul_alloc_count - ue_last_ul_alloc_count[candidate.ue->ue_index()];
  }
}

void scheduler_time_rr::save_dl_newtx_grants(span<const dl_msg_alloc> dl_grants)
{
  if (dl_grants.empty()) {
    return;
  }

  // Mark the count for the allocated UEs.
  for (const auto& grant : dl_grants) {
    ue_last_dl_alloc_count[grant.context.ue_index] = dl_alloc_count;
  }
  ++dl_alloc_count;
}

void scheduler_time_rr::save_ul_newtx_grants(span<const ul_sched_info> ul_grants)
{
  if (ul_grants.empty()) {
    return;
  }

  // Mark the count for the allocated UEs.
  for (const auto& grant : ul_grants) {
    ue_last_ul_alloc_count[grant.context.ue_index] = ul_alloc_count;
  }
  ++ul_alloc_count;
}
