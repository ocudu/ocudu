// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../config/ue_configuration.h"
#include "pdcch_resource_allocator.h"
#include "ocudu/scheduler/resource_grid_util.h"

namespace ocudu {

struct cell_slot_resource_allocator;
class pdcch_slot_allocator;

/// List of CRBs for a given PDCCH candidate.
using crb_index_list = static_vector<uint16_t, pdcch_constants::MAX_NOF_RB_PDCCH>;

class pdcch_resource_allocator_impl final : public pdcch_resource_allocator
{
public:
  explicit pdcch_resource_allocator_impl(const cell_configuration& cell_cfg_);
  ~pdcch_resource_allocator_impl() override;

  void slot_indication(slot_point sl_tx);

  pdcch_dl_information* alloc_dl_pdcch_common(cell_slot_resource_allocator&  slot_alloc,
                                              rnti_t                         rnti,
                                              search_space_id                ss_id,
                                              aggregation_level              aggr_lvl,
                                              std::optional<beam_identifier> beam) override;

  pdcch_ul_information* alloc_ul_pdcch_common(cell_slot_resource_allocator&  slot_alloc,
                                              rnti_t                         rnti,
                                              search_space_id                ss_id,
                                              aggregation_level              aggr_lvl,
                                              std::optional<beam_identifier> beam) override;

  pdcch_dl_information* alloc_dl_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl) override;

  pdcch_ul_information* alloc_ul_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl) override;

  bool cancel_last_pdcch(cell_slot_resource_allocator& slot_alloc) override;

  void stop();

private:
  /// Size of the ring buffer of pdcch_slot_allocator. This size sets a limit on how far in advance a PDCCH can be
  /// allocated.
  static constexpr size_t SLOT_ALLOCATOR_RING_SIZE = get_allocator_ring_size_gt_min(SCHEDULER_MAX_K0);

  struct pdcch_candidate_info {
    pdcch_candidate_list candidates;
  };

  pdcch_slot_allocator& get_pdcch_slot_alloc(slot_point sl);

  pdcch_dl_information* alloc_dl_pdcch_helper(cell_slot_resource_allocator&     slot_alloc,
                                              rnti_t                            rnti,
                                              const bwp_configuration&          bwp_cfg,
                                              const sched_coreset_config&       cs_cfg,
                                              const search_space_configuration& ss_cfg,
                                              aggregation_level                 aggr_lvl,
                                              span<const pdcch_candidate_type>  candidates,
                                              std::optional<beam_identifier>    beam);

  pdcch_ul_information* alloc_ul_pdcch_helper(cell_slot_resource_allocator&     slot_alloc,
                                              rnti_t                            rnti,
                                              const bwp_configuration&          bwp_cfg,
                                              const sched_coreset_config&       cs_cfg,
                                              const search_space_configuration& ss_cfg,
                                              aggregation_level                 aggr_lvl,
                                              span<const pdcch_candidate_type>  candidates,
                                              std::optional<beam_identifier>    beam);

  const cell_configuration& cell_cfg;

  slotted_id_vector<search_space_id, std::array<pdcch_candidate_info, NOF_AGGREGATION_LEVELS>> pdcch_common_candidates;

  /// Last slot for which slot_indication has been called.
  slot_point last_sl_ind;

  /// Intermediate results used by PDCCH scheduler.
  std::array<std::unique_ptr<pdcch_slot_allocator>, SLOT_ALLOCATOR_RING_SIZE> slot_records;
};

} // namespace ocudu
