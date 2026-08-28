// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../config/sched_config_manager.h"
#include "../configured_grant/configured_grant_scheduler.h"
#include "../slicing/inter_slice_scheduler.h"
#include "ue_fallback_scheduler.h"
#include "ue_scheduler.h"
#include "ocudu/ran/du_types.h"

namespace ocudu {

class uci_scheduler_impl;
class srs_scheduler;
class uci_indication_selector;
class ra_ue_repository;
class ue_cell_repository;

struct cell_creation_event {
  cell_resource_allocator& cell_res_grid;
  ue_cell_repository&      ue_cell_db;
  ue_fallback_scheduler&   fallback_sched;
  uci_scheduler_impl&      uci_sched;
  inter_slice_scheduler&   slice_sched;
  srs_scheduler&           srs_sched;
  /// Configured Grant scheduler. Nullptr if CG is not configured for the cell.
  configured_grant_scheduler* cg_sched;
  ra_ue_repository&           ra_ue_repo;
};

class ue_event_manager;

/// Handler of UE events for a given cell.
class ue_cell_event_manager final : public cell_group_ue_config_handler,
                                    public cell_group_ue_indication_handler,
                                    public cell_ue_event_notifier
{
public:
  ue_cell_event_manager(ue_event_manager&          parent_,
                        const cell_creation_event& cell_ev,
                        ue_repository&             ue_db,
                        ocudulog::basic_logger&    logger);
  ~ue_cell_event_manager() override;

  // cell_group_ue_config_handler methods. Handled synchronously, as the cell already deferred them.
  bool handle_ue_creation(ue_config_update_event ev) override;
  bool handle_ue_reconfiguration(ue_config_update_event ev) override;
  bool handle_ue_deletion(ue_config_delete_event ev) override;
  bool handle_ue_config_applied(du_ue_index_t ue_idx) override;
  bool handle_ue_deactivation_request(du_ue_index_t ue_idx) override;
  void handle_slice_reconfiguration(const du_cell_slice_reconfig_request& req) override;

  // cell_group_ue_indication_handler methods.
  bool handle_ul_bsr_indication(const ul_bsr_indication_message& bsr) override;
  bool handle_ul_phr_indication(const ul_phr_indication_message& phr_ind) override;
  bool handle_ul_ta_report_indication(const ul_ta_report_indication_message& ta_report_ind) override;
  bool handle_dl_mac_ce_indication(const dl_mac_ce_indication& mac_ce) override;
  bool handle_crnti_ce_received(du_ue_index_t ue_index) override;
  bool handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& dl_bo) override;

  // cell_ue_event_notifier methods.
  void on_cfra_msg3_acked(du_ue_index_t ue_index) override;
  void on_conres_ce_acked(du_ue_index_t ue_index) override;
  void on_sr_detected(du_ue_index_t ue_index, slot_point uci_slot) override;
  void on_ul_n_ta_update(du_ue_index_t              ue_index,
                         time_alignment_group::id_t tag_id,
                         phy_time_unit              n_ta_diff,
                         float                      ul_sinr) override;

private:
  /// Log event with invalid UE index.
  void log_invalid_ue_index(du_ue_index_t ue_index, const char* event_name, bool warn_if_ignored = true) const;

  /// Log event when UE does not have a carrier for this cell.
  void log_invalid_cc(du_ue_index_t ue_idx, const char* event_name, bool warn_if_ignored = true) const;

  // shared parameters.
  ue_event_manager&       parent;
  ue_repository&          ue_db;
  ocudulog::basic_logger& logger;
  // cell parameters.
  const cell_configuration& cfg;
  ue_fallback_scheduler&    fallback_sched;
  uci_scheduler_impl&       uci_sched;
  inter_slice_scheduler&    slice_sched;
  srs_scheduler&            srs_sched;
  /// Configured Grant scheduler. Nullptr if CG is not configured for the cell.
  configured_grant_scheduler* cg_sched;
  ra_ue_repository&           ra_ue_repo;
};

/// \brief Class used to manage events that arrive to the scheduler and are directed at UEs.
/// This class acts as a facade for several of the ue_scheduler subcomponents, managing the asynchronous configuration
/// of the UEs and logging in a thread-safe manner.
class ue_event_manager
{
public:
  ue_event_manager(ue_repository& ue_db);

  std::unique_ptr<ue_cell_event_manager> add_cell(const cell_creation_event& cell_ev);

private:
  friend class ue_cell_event_manager;

  bool cell_exists(du_cell_index_t cell_index) const;

  ue_repository&          ue_db;
  ocudulog::basic_logger& logger;

  std::array<ue_cell_event_manager*, MAX_NOF_DU_CELLS> cells;
};

} // namespace ocudu
