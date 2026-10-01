// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/scheduler/config/scheduler_expert_config.h"
#include "ocudu/scheduler/sched_consts.h"

namespace ocudu {

struct cell_slot_resource_allocator;
enum class ssb_pattern_case;

/// This class schedules the SIB1 and fills the grant to be passed to lower layers.
class sib1_scheduler
{
public:
  sib1_scheduler(const cell_configuration& cfg_, pdcch_resource_allocator& pdcch_sch, units::bytes sib1_payload_size);

  /// \brief Schedules SIB1 grants (if any) for the provided slot.
  /// \param[out] res_grid Slot at which the SIB1 may be scheduled.
  void run_slot(cell_slot_resource_allocator& res_grid);

  /// \brief Update the SIB1 PDU version.
  void handle_sib1_update_indication(unsigned version, units::bytes sib1_payload_size);

  /// Called when cell is deactivated.
  void stop();

private:
  /// \brief Searches in PDSCH and PDCCH for space to allocate SIB1 and SIB1's DCI, respectively.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] ssb_idx SSB candidate index which the SIB1 corresponds to.
  /// \param[in] time_resource PDSCH time domain resource.
  bool allocate_sib1(cell_slot_resource_allocator& res_grid, unsigned ssb_idx, unsigned time_resource);

  /// \brief Fills the SIB1 slots, at which each SSB candidate's SIB1 is allocated.
  ///
  /// These slots are computed and saved in the body of the constructor.
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] sib1_crbs_grant CRBs interval in the PDSCH allocated for SIB1.
  /// \param[in] time_resource PDSCH time domain resource.
  /// \param[in] dmrs_info DMRS information for SIB1.
  /// \param[in] tbs Transport block size.
  void fill_sib1_grant(cell_slot_resource_allocator& res_grid,
                       unsigned                      ssb_idx,
                       crb_interval                  sib1_crbs_grant,
                       unsigned                      time_resource,
                       const dmrs_information&       dmrs_info,
                       units::bytes                  tbs) const;

  void handle_pending_sib1_update();

  /// SIB1 Logger.
  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("SCHED");

  /// Parameters for SIB1 scheduling.
  const scheduler_si_expert_config& expert_cfg;
  const cell_configuration&         cell_cfg;
  pdcch_resource_allocator&         pdcch_sched;

  /// This is a derived parameters, that depends on the SSB periodicity, SIB1 periodicity and SIB1 re-tx periodicity.
  std::chrono::milliseconds sib1_rtx_period;
  /// The SIB1 payload is in bytes.
  units::bytes sib1_payload_size;
  /// Max number of SSB candidates per SSB period.
  const uint8_t L_max;
  /// Current SIB version.
  unsigned current_version = 0;

  /// This is a dummy BWP configuration dimensioned based on CORESET#0 RB limits. It's used for CRB-to-PRB conversion.
  bwp_configuration coreset0_bwp_cfg;

  /// Array of Type0-PDCCH CSS slots (1 per SSB candidate) used for SIB1 scheduling [TS 38.213, Section 13].
  std::array<slot_point, MAX_NOF_SSB_CANDIDATES> sib1_type0_pdcch_css_slots;

  /// Pending new SIB1 PDU to be applied.
  unsigned     pending_version = 0;
  units::bytes pending_sib1_len;
};

} // end of namespace ocudu
