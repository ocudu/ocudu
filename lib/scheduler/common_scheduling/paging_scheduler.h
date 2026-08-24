// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../config/cell_configuration.h"
#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "../support/paging_helpers.h"
#include "ocudu/adt/flat_map.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/slot_point_extended.h"
#include "ocudu/scheduler/config/scheduler_expert_config.h"
#include "ocudu/scheduler/result/pdsch_info.h"
#include "ocudu/scheduler/scheduler_paging_handler.h"

namespace ocudu {

struct sched_paging_information;

/// Defines Paging scheduler that is used to allocate resources to send paging information to UE in a given slot.
class paging_scheduler
{
public:
  // 48 bits (6 Bytes) 5G-S-TMSI + 1 Byte for Paging record header size estimate.
  static constexpr unsigned RRC_CN_PAGING_ID_RECORD_SIZE = 7U;
  // 40 bits (5 Bytes) Full-I-RNTI + 1 Byte for Paging record header size estimate.
  static constexpr unsigned RRC_RAN_PAGING_ID_RECORD_SIZE = 6U;
  // [Implementation defined] Maximum number of pending pagings allowed.
  static constexpr unsigned MAX_NOF_PENDING_PAGINGS = MAX_NOF_DU_UES;
  // [Implementation defined] Number of slots that the paging is scheduled ahead of the current slot.
  static constexpr unsigned DEFAULT_NOF_SLOTS_AHEAD_SCHED = 4U;

  /// NG-5G-S-TMSI (48 bits) or I-RNTI-Value (40 bits).
  using ue_paging_id              = uint64_t;
  using paging_retries_count_type = unsigned;

  /// \brief Constructor.
  /// \param[in] cell_cfg_ Cell configuration.
  /// \param[in] pdcch_sch_ PDCCH resource allocator.
  /// \param[in] nof_slots_ahead_sched_ Number of slots ahead to schedule paging. The higher the value, the higher
  /// the priority of paging scheduling compared to other scheduling tasks. It will make the paging scheduler more
  /// likely to find resources available in the resource grid.
  explicit paging_scheduler(const cell_configuration& cell_cfg_,
                            pdcch_resource_allocator& pdcch_sch_,
                            unsigned                  nof_slots_ahead_sched_ = DEFAULT_NOF_SLOTS_AHEAD_SCHED);

  /// \brief Performs paging (if any) scheduling for the current slot.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] hyper_sfn_tx HyperSFN for the slot provided in the current slot indication.
  void run_slot(cell_resource_allocator& res_grid, uint32_t hyper_sfn_tx);

  /// \brief Handles Paging information reported by upper layers.
  /// \note Must be called from the cell scheduler executor.
  /// \param[in] paging_info Per UE paging information to be scheduled.
  void handle_paging_information(const sched_paging_information& paging_info);

  /// Called when cell is deactivated.
  void stop();

private:
  struct ue_paging_info {
    sched_paging_information  request;
    paging_retries_count_type retry_count;
  };

  std::optional<unsigned> find_pdsch_time_resource(const cell_resource_allocator&  res_grid,
                                                   const sched_paging_information& pg_req,
                                                   slot_point_extended             pdcch_slot);

  /// \brief Helper function to get sum of paging payload size of each UE scheduled at a particular slot.
  /// \param[in] ue_paging_ids List of UE scheduled at a particular slot for Paging.
  /// \return Sum of payload size.
  unsigned get_accumulated_paging_msg_size(const std::vector<ue_paging_id>& ue_paging_ids) const;

  /// \brief Checks whether there is space for PDSCH to allocate Paging grant.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] pdsch_time_res Slot at which PDSCH needs to be scheduled.
  /// \param[in] msg_size Paging message size.
  /// \return True if there is space to allocate Paging grant, false otherwise.
  bool is_there_space_available_for_paging(const cell_resource_allocator& res_grid,
                                           unsigned                       pdsch_time_res,
                                           unsigned                       msg_size,
                                           slot_point                     pdcch_slot) const;

  /// \brief Allocates PDSCH and PDCCH for Paging.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] pdsch_time_res Slot at which PDSCH needs to be scheduled.
  /// \param[in] ues_paging_ids List of UE scheduled at a particular slot for Paging.
  /// \param[in] ss_id Search Space Id used in scheduling paging message.
  /// \return True if paging allocation is successful, false otherwise.
  bool allocate_paging(cell_resource_allocator&         res_grid,
                       slot_point                       pdcch_slot,
                       unsigned                         pdsch_time_res,
                       const std::vector<ue_paging_id>& ues_paging_ids,
                       search_space_id                  ss_id);

  /// \brief Fills the Paging grant.
  ///
  /// \param[out,in] pg_grant Paging grant to be filled.
  /// \param[out,in] pdcch Allocated PDCCH for Paging.
  /// \param[in] crbs_grant Paging grant in CRBs.
  /// \param[in] time_resource Slot at which PDSCH needs to be scheduled.
  /// \param[in] ues_paging_ids List of UE scheduled at a particular slot for Paging.
  /// \param[in] dmrs_info DMRS information related to the scheduled grant.
  /// \param[in] tbs TBS information of the Paging grant.
  void fill_paging_grant(dl_paging_allocation&            pg_grant,
                         pdcch_dl_information&            pdcch,
                         crb_interval                     crbs_grant,
                         unsigned                         time_resource,
                         const std::vector<ue_paging_id>& ues_paging_ids,
                         const dmrs_information&          dmrs_info,
                         units::bytes                     tbs);

  /// Determine whether the current slot is a paging opportunity for the given Paging information.
  /// \param[in] pdcch_slot PDCCH slot point.
  /// \param[in] UE paging information context
  bool is_paging_opportunity(slot_point_extended pdcch_slot, const sched_paging_information& req) const;

  // Args.
  const scheduler_expert_config& expert_cfg;
  const cell_configuration&      cell_cfg;
  pdcch_resource_allocator&      pdcch_sch;
  const unsigned                 nof_slots_ahead_sched;

  // Derived args.

  /// Default Paging cycle (DRX cycle) configured in SIB1, expressed in nof. radio frames. This value is used if F1AP
  /// does not provide Paging DRX IE in Paging message.
  unsigned default_paging_cycle;
  /// Number of paging frames per DRX cycle. Value of N in the equation in clause 7.1 of TS 38.304.
  unsigned nof_pf_per_drx_cycle;
  /// Paging frame offset. Value of PF_offset in the equation in clause 7.1 of TS 38.304.
  unsigned paging_frame_offset{};
  /// Number of paging occasions per paging frame. Value of Ns in the equation in clause 7.1 of TS 38.304.
  uint8_t nof_po_per_pf;

  paging_slot_helper slot_helper;

  /// Search Space configuration when pagingSearchSpace > 0, if configured.
  const search_space_configuration* ss_cfg = nullptr;

  /// BWP configuration of CORESET used for Paging (applies for both pagingSearchSpace = 0 and pagingSearchSpace > 0).
  /// It's used for CRB-to-PRB conversion.
  bwp_configuration bwp_cfg;
  /// PDSCH time domain resource allocation list.
  span<const pdsch_time_domain_resource_allocation> pdsch_td_alloc_list;

  /// Contains paging information of UEs yet to be scheduled.
  flat_map<ue_paging_id, ue_paging_info> paging_pending_ues;
  /// Lookup to keep track of scheduled paging UEs at a particular PDSCH time resource index. Index of \c
  /// pdsch_time_res_idx_to_scheduled_ues_lookup corresponds to PDSCH Time Domain Resource Index.
  std::vector<std::vector<ue_paging_id>> pdsch_time_res_idx_to_scheduled_ues_lookup;

  ocudulog::basic_logger& logger;
};

} // namespace ocudu
